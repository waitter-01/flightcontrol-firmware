# μC/OS-III 驱动解耦第一阶段

## 1. 阶段结果

本阶段已经把 μC/OS-III 工程从单文件 Hello World 扩展为可继续迁移飞控驱动的 ARM
软件骨架，同时保持所有危险输出关闭。

```text
应用任务 app.c
      │
      ├── services/health_monitor   组件心跳与超时判断
      │
      ├── drivers/watchdog          硬件看门狗受控接口
      │
      ├── bsp/board_safety          板级安全状态与输出许可
      │
      ├── platform                  单调时间、日志
      │
      └── core/fc_status            公共错误模型
```

当前代码只访问 Zynq 全局计时器和 PS UART0 控制台。没有调用 GPIO 输出、配电、继电器、
PWM、舵机、电机、QSPI 写入或硬件看门狗启动函数。

## 2. 分层职责

| 层 | 当前内容 | 禁止事项 |
|---|---|---|
| `core` | 统一 `fc_status_t` 错误码 | 不依赖 Xilinx 或 RTOS |
| `platform` | 全局定时器、μC/OS 日志 | 不包含板卡业务规则 |
| `bsp` | 板级安全状态和输出许可 | 不直接实现控制算法 |
| `drivers` | 外设后端接口 | 不绕过安全状态机 |
| `services` | 健康监测、后续参数和消息服务 | 不直接访问寄存器 |

旧 FreeRTOS 工程中的 `ucas` 目录混合了 RTOS 同步对象、Xilinx 驱动、板级地址和业务逻辑。
新工程不直接复制该结构，而是先确定接口和安全边界，再按设备逐个迁移。

## 3. 已实现模块

### 3.1 公共错误模型

`core/fc_status.h` 定义稳定的返回值，包括参数错误、状态错误、超时、不支持、安全锁定、
平台错误和资源不足。驱动不再只返回含义不明确的 `0/-1`。

### 3.2 单调时间

`platform/platform_time.c` 使用 Zynq Cortex-A9 全局定时器：

- 初始化时记录起始计数；
- 提供微秒和 32 位毫秒时间；
- 通过无符号减法支持毫秒计数回绕；
- 不占用 μC/OS-III 的 SCU Timer Tick；
- 不依赖任务调度次数计算时间。

### 3.3 日志

`platform/platform_log.c` 提供 `DEBUG/INFO/WARN/ERROR/CRITICAL` 五级日志，并统一输出
组件名和错误状态。当前后端是 BSP 的 `UCOS_Print`，经 PS UART0 输出。

当前日志接口没有加互斥锁，不允许从中断上下文调用。以后出现多个高频写日志任务时，
应改为固定内存队列和单独日志任务。

### 3.4 板级安全状态

安全状态包括：

1. `BOOT_LOCKED`：启动阶段；
2. `DISARMED`：基础服务正常，但危险输出仍禁止；
3. `FAULT_LOCKED`：初始化或健康监测失败，锁存故障。

当前版本故意没有 `ARMED` 状态，也没有解除危险输出的方法。以下输出域始终返回“不允许”：

- 动力；
- 舵机；
- 配电；
- 继电器；
- Flash 写入。

### 3.5 健康监测

主应用每 1 秒更新一次心跳，独立健康任务每 250 ms 检查一次，主任务超时阈值为 2.5 秒。
一旦超时，安全状态进入 `FAULT_LOCKED` 并输出一次严重错误日志。

健康时间戳使用 32 位读写，适配 32 位 Cortex-A9，避免跨任务读取 64 位时间时出现撕裂。

### 3.6 看门狗接口

旧 `usrwatchdog.c` 会自行初始化 GIC 和 SCU 私有定时器，与 μC/OS-III 中断控制器和 1 kHz
Tick 冲突，因此不能直接迁移。

当前 `drivers/watchdog` 只保存期望超时时间，并让 `start()` 返回 `FC_STATUS_LOCKED`：

- 不重新初始化 GIC；
- 不占用 SCU Timer；
- 不启动 SCU WDT；
- 不驱动旧 MIO9 外部喂狗信号。

硬件后端必须在确认原理图、复位链、WDT 时钟和调试恢复方法后另行实现。

## 4. 自动安全检查

构建脚本会先执行：

```powershell
.\scripts\check_ucos3_safety.ps1
```

脚本会拒绝当前 μC/OS-III 源码中未经批准的调用，包括 GPIO 输出、配电/继电器、舵机、
电机、QSPI 传输、SCU Timer 启动和 SCU WDT 启动。

这是一道辅助防线，不替代代码审查、原理图核对和断负载测试。

## 5. 离线验证结果

构建命令：

```powershell
.\scripts\setup_ucos3.ps1 `
  -SdkRoot 'D:\Xilinx\SDK\2019.1' `
  -MicriumRepository 'D:\ZynqWork\ucos_v1_45\ucos'
```

验证结果：

- μC/OS-III BSP 构建通过；
- 所有新增源文件参与 ARM GNU 编译；
- 新增代码没有编译器 warning；
- 应用链接通过；
- ELF：`text 127432 B`、`data 2508 B`、`bss 41312 B`，合计 `171252 B`。

BSP 中原有的自定义 AXI self-test 和 Micrium UART 驱动仍有历史 warning，和本阶段新增代码
无关，后续应单独处理。

## 6. 你上机时的第一轮测试范围

第一轮只做断负载基础验证：

1. 断开电机、螺旋桨、舵机、继电器和可控配电负载；
2. 不烧写 QSPI，只通过 JTAG 下载 bitstream 和 ELF；
3. 停在 `main()` 检查程序入口，再继续运行；
4. 观察 PS UART0 是否输出 `platform initialized; outputs locked`；
5. 确认每 5 秒出现应用心跳；
6. 确认没有 GPIO、PWM、继电器和电源通道动作；
7. 如需验证超时，把 `core/fc_build_config.h` 中的
   `FC_CFG_HEALTH_FAULT_INJECTION_ENABLED` 临时改为 `1` 后重编译；程序会在 15 秒后停止
   主任务心跳，健康任务应在约 2.5 秒后报告超时并锁定；
8. 记录串口日志、JTAG 状态、任务栈使用率和异常复位情况。

完成故障注入后应把宏恢复为 `0`。不要在本轮测试中启用硬件看门狗，也不要调用旧
FreeRTOS 工程的初始化总入口。

## 7. 上机通过后的下一阶段

基础模板通过断负载验证后，按以下顺序继续：

1. 建立经原理图确认的硬件资源表；
2. 先迁移纯输入 GPIO 和板载诊断 LED；
3. 建立统一中断注册接口；
4. 迁移一个只读传感器链路；
5. 实现无中断控制器重初始化的 SCU WDT 后端；
6. 再迁移 CAN、SPI、I²C、ADC 和 UART；
7. 最后迁移动力、舵机、配电、继电器和 Flash 写入。
