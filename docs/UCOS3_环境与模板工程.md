# FlightControl μC/OS-III 环境与模板工程

## 1. 当前结论

仓库已增加一个与原 FreeRTOS 主程序并行存在的 μC/OS-III 模板：

- 应用工程：`FlightControl_ucos`；
- BSP 工程：`FlightControl_ucos_bsp`；
- 处理器：`ps7_cortexa9_0`；
- 操作系统：Micrium μC/OS-III 1.44；
- 工具链：Xilinx SDK 2019.1；
- 节拍频率：1 kHz；
- 控制台：PS UART0；
- 当前状态：BSP 和 Debug 应用均已完成离线编译。

该模板只证明“硬件平台 + μC/OS-III + Cortex-A9 工具链”已经连通。它不会立即替换现有
FreeRTOS 飞控应用。后续应先迁移独立驱动和公共服务，再逐步迁移飞控任务。

## 2. 第三方软件与许可边界

使用的本地资料为 Micrium Xilinx Repository 1.44。执行脚本时，参数必须指向其中真正的
`ucos` 子目录；该目录下应直接包含：

```text
ucos/
├─ bsp/
├─ components/
├─ drivers/
└─ sw_apps/
```

Micrium 源码受其自身许可约束。仓库不会复制或提交 `ucos_osiii`、`ucos_common`、
`ucos_standalone` 等第三方源码，只跟踪 BSP 的 `system.mss`、SDK 工程描述和本项目原创
应用代码。商业开发或对外分发前必须确认已经获得适用的 Micrium/Silicon Labs 授权。

## 3. 一键配置和构建

在 PowerShell 中运行：

```powershell
cd D:\ZynqWork\flightcontrol-firmware

.\scripts\setup_ucos3.ps1 `
  -SdkRoot 'D:\Xilinx\SDK\2019.1' `
  -MicriumRepository 'D:\资料目录\ucos_v1_44\ucos'
```

脚本会依次完成：

1. 校验 SDK、XSCT 和 Micrium 软件仓库；
2. 修正 SDK 2019.1 的 Java/Eclipse 用户目录；
3. 定位 SDK 自带的 ARM 编译器、预构建步骤和 GNU Make；
4. 注册 Micrium 本地软件仓库；
5. 导入已有 SDK 工程；
6. 写入 μC/OS-III BSP 配置并重新生成处理器专用库；
7. 构建 `FlightControl_ucos_bsp`，刷新应用 Makefile 后显式构建 `FlightControl_ucos`。

全新 Workspace 第一次生成 Debug Makefile 时，SDK 的 CDT 控制台仍可能短暂显示
`Program "make" not found in PATH`。脚本随后会显式调用 SDK 自带的 Make 完成真实构建；
应以脚本最终退出码和 `FlightControl_ucos.elf.size` 为准，而不是把这条中间提示当作最终结果。

仅重新生成 BSP、不构建应用时使用：

```powershell
.\scripts\setup_ucos3.ps1 `
  -MicriumRepository 'D:\资料目录\ucos_v1_44\ucos' `
  -SkipBuild
```

## 4. BSP 配置基线

`FlightControl_ucos_bsp/system.mss` 保存可审查的 BSP 配置，当前关键项如下：

| 配置项 | 当前值 | 用途 |
|---|---:|---|
| `OS_CFG_TICK_RATE_HZ` | 1000 | 1 ms 系统节拍 |
| `OS_CFG_STAT_TASK_EN` | true | 开启内核统计任务 |
| `OS_CFG_STAT_TASK_STK_CHK_EN` | true | 检查统计任务栈 |
| `OS_CFG_TASK_PROFILE_EN` | true | 记录任务运行信息 |
| `OS_CFG_SCHED_LOCK_TIME_MEAS_EN` | true | 测量调度锁定时间 |
| `OS_CFG_TS_EN` | true | 为统计和调度测量提供时间戳 |
| `stdin` / `stdout` | `ps7_uart_0` | 模板串口控制台 |

模板还使用 Micrium 提供的 Zynq 驱动替换项，包括 `ucos_l2cachec`、`ucos_scuc`、
`ucos_scutimer` 和 `ucos_uartps`。

当前本机 `gpiops_v3_5` 源码引用了较新工具链中的 `XPLAT_versal`，而 Micrium 1.44 的
`ucos_standalone` 未定义该枚举。脚本为 Cortex-A9 编译增加 `-DXPLAT_versal=5`。Zynq-7000
运行时的平台值是 `XPLAT_ZYNQ`，不会进入 Versal 分支。将来更换 SDK 或 Micrium 版本后，
应重新检查并删除不再需要的兼容宏。

## 5. 生成物和版本管理

进入 Git 的内容：

- `FlightControl_ucos/src/` 中的原创模板应用和链接描述；
- `FlightControl_ucos_bsp/system.mss`；
- 两个 SDK 工程的 `.project`、`.cproject`、`.sdkproject` 和 BSP Makefile；
- `scripts/setup_ucos3.*` 和本文档。

不进入 Git 的内容：

- `FlightControl_ucos/Debug/` 和 ELF；
- `FlightControl_ucos_bsp/ps7_cortexa9_0/`；
- `.metadata/`、SDK 缓存和日志；
- 本地 Micrium 软件仓库及其源码。

这样既保留了可重复生成能力，也避免把工具生成物和受许可约束的第三方源码复制到远端。

## 6. 下一阶段迁移顺序

驱动迁移不应从复制旧 `main.c` 开始，而应按依赖方向推进：

1. 建立 `platform`、`bsp`、`drivers`、`services` 四层目录和稳定接口；
2. 先迁移单调时间、串口日志、中断注册、GPIO 安全默认值和看门狗；
3. 再迁移 SPI、I²C、CAN、ADC、PS/PL UART 和自定义 AXI IP 访问；
4. 为每个驱动增加初始化、超时、错误码、统计和失效恢复接口；
5. 用 μC/OS-III 信号量、消息队列和任务封装替换 FreeRTOS 原语；
6. 完成传感器只读链路后，再迁移状态估计和飞控公共服务；
7. 执行机构、配电、继电器和 QSPI 写入保持默认禁用，最后单独审核迁移；
8. FreeRTOS 与 μC/OS-III 两套工程在功能对等验证前并行保留。

每迁移一个驱动都要同时保留可编译证据、接口说明、硬件资源表和断负载测试记录，避免
把旧工程中“任务、驱动、板级寄存器和业务逻辑互相调用”的结构原样搬到新系统。

## 7. 当前编译基线

首次验证生成的 `FlightControl_ucos.elf` 体积为：

```text
text    77576 B
data     2544 B
bss     35984 B
合计   116104 B
```

该数值只用于确认模板基线。替换模板应用或调整内核裁剪项后，应重新记录 ELF size 和
任务栈预算。
