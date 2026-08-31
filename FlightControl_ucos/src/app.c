#include <Source/os.h>
#include <ucos_bsp.h>

#define APP_HEARTBEAT_PERIOD_MS 1000u

static void AppStartTask(void *argument);

int main(void)
{
    UCOSStartup(AppStartTask);
    return 0;
}

static void AppStartTask(void *argument)
{
    OS_ERR error;

    (void)argument;
    UCOS_Print("FlightControl uC/OS-III template started.\r\n");

    while (DEF_TRUE) {
        OSTimeDlyHMSM(0u,
                      0u,
                      0u,
                      APP_HEARTBEAT_PERIOD_MS,
                      OS_OPT_TIME_HMSM_STRICT,
                      &error);

        if (error != OS_ERR_NONE) {
            UCOS_Print("AppStartTask delay failed.\r\n");
        } else {
            UCOS_Print("FlightControl uC/OS-III heartbeat.\r\n");
        }
    }
}
