#include <Source/os.h>
#include <ucos_bsp.h>

#include "bsp/board_safety.h"
#include "core/fc_build_config.h"
#include "drivers/watchdog/watchdog.h"
#include "platform/platform_log.h"
#include "platform/platform_time.h"
#include "services/health_monitor.h"
#include "services/input_store.h"
#include "services/output_arbiter.h"
#include "services/parameter_store.h"

#define APP_HEARTBEAT_PERIOD_MS        1000u
#define APP_HEALTH_TIMEOUT_MS          2500u
#define APP_STATUS_LOG_DIVIDER         5u
#define APP_HEALTH_COMPONENT_MAIN      0u
#define APP_HEALTH_TASK_PRIORITY       6u
#define APP_HEALTH_TASK_STACK_SIZE     512u
#define APP_FAULT_INJECTION_DELAY_MS   15000u

static OS_TCB g_health_task_tcb;
static CPU_STK g_health_task_stack[APP_HEALTH_TASK_STACK_SIZE];

static void AppStartTask(void *argument);
static void AppHealthTask(void *argument);
static fc_status_t AppPlatformInit(void);

/**
 * @brief μC/OS-III 飞控程序入口。
 *
 * @return 正常情况下不会返回；若内核启动意外返回则返回 0。
 * @note 入口只把控制权交给 Micrium 启动流程，不直接操作板级外设。
 */
int main(void)
{
    UCOSStartup(AppStartTask);
    return 0;
}

/**
 * @brief 按安全顺序初始化平台公共服务。
 *
 * @return 所有基础服务就绪返回 FC_STATUS_OK，否则返回首个失败状态。
 * @note 本函数保持危险输出锁定，并且不会启动硬件看门狗后端。
 */
static fc_status_t AppPlatformInit(void)
{
    fc_status_t status;

    status = fc_time_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_board_safety_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_health_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_input_store_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_parameter_store_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_output_arbiter_init();
    if (status != FC_STATUS_OK) {
        return status;
    }

    status = fc_health_register(APP_HEALTH_COMPONENT_MAIN,
                                "app_main",
                                APP_HEALTH_TIMEOUT_MS);
    if (status != FC_STATUS_OK) {
        return status;
    }

    return fc_watchdog_prepare(3000u);
}

/**
 * @brief 创建健康监测任务并维持主应用心跳。
 *
 * @param argument Micrium 任务参数，当前未使用。
 * @note 运行于任务上下文；任何初始化或调度错误都会锁存安全故障。
 */
static void AppStartTask(void *argument)
{
    OS_ERR os_error;
    fc_status_t status;
    uint32_t heartbeat_count = 0u;
    bool send_heartbeat;
#if (FC_CFG_HEALTH_FAULT_INJECTION_ENABLED == 1)
    bool injection_logged = false;
#endif

    (void)argument;
    status = AppPlatformInit();
    if (status != FC_STATUS_OK) {
        fc_board_safety_force_lock(FC_SAFETY_FAULT_INITIALIZATION);
        fc_log_status(FC_LOG_CRITICAL, "startup", "platform_init", status);
    } else {
        OSTaskCreate(&g_health_task_tcb,
                     "Flight health monitor",
                     AppHealthTask,
                     0,
                     APP_HEALTH_TASK_PRIORITY,
                     &g_health_task_stack[0],
                     APP_HEALTH_TASK_STACK_SIZE / 10u,
                     APP_HEALTH_TASK_STACK_SIZE,
                     0u,
                     0u,
                     0,
                     OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR,
                     &os_error);

        if (os_error != OS_ERR_NONE) {
            fc_board_safety_force_lock(FC_SAFETY_FAULT_INITIALIZATION);
            fc_log_write(FC_LOG_CRITICAL, "startup", "health task creation failed");
        } else {
            status = fc_board_safety_enter_disarmed();
            if (status != FC_STATUS_OK) {
                fc_board_safety_force_lock(FC_SAFETY_FAULT_INITIALIZATION);
                fc_log_status(FC_LOG_CRITICAL, "startup", "enter_disarmed", status);
            } else {
                fc_log_write(FC_LOG_INFO, "startup", "platform initialized; outputs locked");
                fc_log_write(FC_LOG_INFO, "watchdog", "hardware backend held disabled");
            }
        }
    }

    while (DEF_TRUE) {
        send_heartbeat = true;
#if (FC_CFG_HEALTH_FAULT_INJECTION_ENABLED == 1)
        if (fc_time_now_ms() >= APP_FAULT_INJECTION_DELAY_MS) {
            send_heartbeat = false;
            if (!injection_logged) {
                injection_logged = true;
                fc_log_write(FC_LOG_WARNING, "test", "health heartbeat fault injected");
            }
        }
#endif

        if (send_heartbeat) {
            status = fc_health_heartbeat(APP_HEALTH_COMPONENT_MAIN);
            if (status != FC_STATUS_OK) {
                fc_board_safety_force_lock(FC_SAFETY_FAULT_HEALTH_TIMEOUT);
                fc_log_status(FC_LOG_ERROR, "health", "main_heartbeat", status);
            }
        }

        ++heartbeat_count;
        if ((heartbeat_count % APP_STATUS_LOG_DIVIDER) == 0u) {
            fc_log_write(FC_LOG_INFO, "app", "heartbeat; dangerous outputs remain locked");
        }

        OSTimeDlyHMSM(0u,
                      0u,
                      0u,
                      APP_HEARTBEAT_PERIOD_MS,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_error);

        if (os_error != OS_ERR_NONE) {
            fc_board_safety_force_lock(FC_SAFETY_FAULT_HEALTH_TIMEOUT);
            fc_log_write(FC_LOG_CRITICAL, "app", "main task delay failed");
        }
    }
}

/**
 * @brief 周期检查关键组件心跳并触发故障锁定。
 *
 * @param argument Micrium 任务参数，当前未使用。
 * @note 运行于任务上下文；检测到超时后只允许进入更安全的锁定状态。
 */
static void AppHealthTask(void *argument)
{
    OS_ERR os_error;
    bool was_healthy = true;
    bool is_healthy;

    (void)argument;

    while (DEF_TRUE) {
        OSTimeDlyHMSM(0u,
                      0u,
                      0u,
                      250u,
                      OS_OPT_TIME_HMSM_STRICT,
                      &os_error);

        if (os_error != OS_ERR_NONE) {
            fc_board_safety_force_lock(FC_SAFETY_FAULT_HEALTH_TIMEOUT);
            fc_log_write(FC_LOG_CRITICAL, "health", "monitor task delay failed");
            continue;
        }

        is_healthy = fc_health_all_healthy();
        if (!is_healthy) {
            fc_board_safety_force_lock(FC_SAFETY_FAULT_HEALTH_TIMEOUT);
            if (was_healthy) {
                fc_log_write(FC_LOG_CRITICAL, "health", "critical component timeout; outputs locked");
            }
        }

        was_healthy = is_healthy;
    }
}
