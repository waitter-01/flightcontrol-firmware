# FlightControl 飞控开发当前进度与 AI 交接

> 更新时间：2026-09-10
> 当前阶段：μC/OS-III 七个离线软件阶段已完成；JTAG、真实硬件、执行机构和飞行均未验证。

## 1. 一句话状态

后续 μC/OS-III 开发只使用 `D:\ZynqWork\flightcontrol-ucos145`。原 FreeRTOS 与 μC/OS-III 1.44 工作区继续保留用于对照，新工作区中的 `FlightControl_ucos` 和 `FlightControl_ucos_bsp` 已基于 Micrium Xilinx Repository 1.45 完成离线构建。该工程具备独立驱动、输入数据模型、MAVLink/参数基础服务、姿态估计、PID、六旋翼混控和输出安全仲裁，但没有下载或运行当前 ELF，因此不能声称板卡或飞控已经正常工作。

最新开发总览见 `docs/UCOS3_开发阶段总览.md`，统一硬件验收步骤见
`docs/统一上机测试计划.md`。当前分支为 `feature/ucos145-migration`，按阶段保留小提交并推送。

## 2. 关键目录

| 目录 | 内容 | 使用规则 |
|---|---|---|
| `D:\ZynqWork\legacy-2019.1\FlightControl` | 已修复并验证过的 2019.1 基线 | 只读对照和回退，不在这里继续开发 |
| `D:\ZynqWork\flightcontrol-ucos145` | 当前 μC/OS-III 1.45 开发工作区 | 后续 μC/OS-III 修改、构建和调试都在这里 |
| `D:\ZynqWork\flightcontrol-dev-2019.1\FlightControl` | μC/OS-III 1.44 与 FreeRTOS 对照工作区 | 保留对照，不继续开展 1.45 开发 |
| `D:\ZynqWork\ucos_v1_45\ucos` | Micrium Xilinx Repository 1.45 | 当前 BSP 与库的生成来源 |
| `D:\飞控资料\敏捷飞控资料` | 开发板、敏捷飞控等用户资料 | 核对供电、接口、针脚和板卡使用方法 |
| `D:\飞控资料\Xilinx\_Vivado\_SDK\_2018.3\_1207\_2324` | 另一份 2018.3 归档 | 尚未确认是否为完整原工程，不作为当前输入 |
| `D:\Xilinx\SDK\2019.1` | 当前实际使用的 SDK | 编译器、XSCT、Eclipse SDK 均从这里运行 |
| `D:\ZynqWork\vitis-2024.2` | 未来迁移预留位置 | 先固化 2019.1 硬件基线，再迁移 |
| `D:\我的笔记\硬件开发复习笔记\Zynq学习` | Obsidian 笔记 | 环境、工程结构、迁移和交接文档 |

## 3. 开发工作区内部结构

根目录：`D:\ZynqWork\flightcontrol-ucos145`

| 子目录/文件 | 作用 | 注意事项 |
|---|---|---|
| `.metadata` | Eclipse Workspace、索引和问题标记 | 全新创建，未复制旧缓存；不是源码 |
| `.sdk\launch_scripts` | System Debugger Tcl 脚本 | HDF、bit、PS7 init、ELF 已迁移到新路径 |
| `bootimage` | `MAINBOOT.bin`、`BACKBOOT.bin`、`FlightControl.bif` | 仅作历史参考，内容未验证，禁止直接烧写 |
| `design_top_wrapper_hw_platform_0` | `system.hdf`、bitstream、`ps7_init.*`、自定义驱动 | FSBL/BSP 的硬件依据；含 AXI UART、AD7606 等驱动 |
| `design_top_wrapper.hdf` | 根目录另一份 HDF | 当前项目实际使用硬件平台目录内的 `system.hdf` |
| `FSBL_bsp` | FSBL standalone BSP | stdout/stdin 走物理 `ps7_uart_0` |
| `FSBL` | Zynq FSBL | Debug 已构建成功 |
| `FlightControl_bsp` | FreeRTOS BSP 和 libmetal | stdout/stdin 走 `ps7_coresight_comp_0`，即 JTAG DCC |
| `FlightControl` | 飞控应用 | 当前主要开发对象；Debug 成功，Release 未修好 |
| `MIGRATION_README.md` | 副本迁移说明 | 记录基线与开发副本的边界 |
| `AI_HANDOFF.md` | 本文档 | 下一个 AI 首先阅读 |

### 3.1 FlightControl 主要源码

| 路径 | 作用 |
|---|---|
| `FlightControl\src\main.c` | Cache、`start_task`、FreeRTOS 调度器和启动打印 |
| `FlightControl\src\ucas\drv_main.c` | GPIO/配电、ADC、PS/PL UART、QSPI、传感器总初始化 |
| `FlightControl\src\flight_control\flight_control.c` | 1 ms 飞控任务框架；控制算法主体基本为空 |
| `FlightControl\src\Sensors.c` | GF404 等传感器数据接口 |
| `FlightControl\src\adc_read.c` | AD7606/ADC 读取 |
| `FlightControl\src\Commulink.c` | 通信链路与 MAVLink 发送入口 |
| `FlightControl\src\MavlinkCMDProcess.c` | MAVLink 命令处理 |
| `FlightControl\src\MavlinkRCProcess.c` | MAVLink 遥控数据处理 |
| `FlightControl\src\mavlink_SendFuncs.c` | MAVLink 状态封装；存在未初始化变量警告 |
| `FlightControl\src\RcControl.c` | PS UART0 接收机链路，运行时设置为 420000、8E2 |
| `FlightControl\src\ServoControl.c` | 舵机及配电；`Servo_init()` 当前被注释 |
| `FlightControl\src\WP40Motor.c` | WP40 电机；`wp40_Init()` 当前被注释 |
| `FlightControl\src\ucas\service\usr\usrpdsxcontrol.c` | PD/SX 配电和时序 GPIO |
| `FlightControl\src\ucas\Basic.c` | PS/PL UART 等基础驱动 |
| `FlightControl\src\ucas\qflash.c` | QSPI Flash 接口；擦除/写入必须单独确认 |
| `FlightControl\src\ucas\hal` | CAN、UART、SPI 底层驱动 |
| `FlightControl\src\ucas\platform` | FIFO、内存池、CRC 和通用外设抽象 |
| `FlightControl\src\mavlink` | MAVLink 生成头文件 |
| `FlightControl\src\lscript.ld` | 链接脚本；`_HEAP_SIZE = 0x08000000`，需后续审计 |

## 4. 已完成工作

### 4.1 环境和 IDE

- Xilinx SDK 2019.1 已安装在 `D:\Xilinx\SDK\2019.1`。
- SDK 能打开当前开发工作区。
- 曾发生 CDT 编辑器插件无法加载，原因是 Eclipse 启动时序/缓存异常；用 `-eclipseargs -clean` 已恢复。
- Project Explorer 消失时：`Window → Show View → Project Explorer`。
- 布局异常时：`Window → Perspective → Reset Perspective`。

重新打开当前工作区：

```powershell
& 'D:\Xilinx\SDK\2019.1\bin\xsdk.bat' `
  -workspace 'D:\ZynqWork\flightcontrol-ucos145' `
  -eclipseargs -clean
```

文档生成时没有 SDK 进程占用该工作区。

### 4.2 路径迁移

- 原工程曾位于 `C:/whb/VTOL/FlightControl`，旧依赖导致 `ps7_init.c` 构建失败。
- legacy 基线已修复并成功构建。
- 开发副本未复制旧 `.metadata`、Debug/Release 输出和失效 CMake 缓存。
- 五个项目已重新导入：
  - `design_top_wrapper_hw_platform_0`
  - `FSBL_bsp`
  - `FSBL`
  - `FlightControl_bsp`
  - `FlightControl`
- `libmetal` 工具链路径和 CMake 缓存已在新目录重建。
- 调试 Tcl 的 HDF、bitstream、PS7 init 和 ELF 路径已迁移。
- 活跃配置中旧 legacy 路径和 `C:/whb/VTOL/FlightControl` 功能引用均为 0。
- `allegro.jrl` 仍记录原作者机器路径，但它只是历史日志，不参与构建。
- Problems 曾显示四条 `C:\Xilinx\SDK\2019.1` 无效路径。实际 `FlightControl.sc` 已有 12 条正确 D 盘路径、0 条 C 盘路径；旧 `.markers` 已备份为 `.markers.pre-path-refresh-backup` 并刷新。

## 5. 构建状态

| 项目 | 配置 | 状态 |
|---|---|---|
| `FSBL_bsp` | BSP libraries | 成功 |
| `FSBL` | Debug | 成功 |
| `FSBL` | Release | legacy 基线成功；开发副本主要验证 Debug |
| `FlightControl_bsp` | FreeRTOS + libmetal | 成功 |
| `FlightControl` | Debug | 成功 |
| `FlightControl` | Release | 未通过，原配置缺少 `tempsensor.h` 等 include path |

关键产物：

- `FSBL\Debug\FSBL.elf`
  - 文件大小 509052 字节
  - text 64000、data 11568、bss 72504
- `FlightControl\Debug\FlightControl.elf`
  - 文件大小 1331492 字节
  - text 252032、data 4612、bss 335623136
- `FlightControl_bsp\ps7_cortexa9_0\lib\libmetal.a`
  - 文件大小 256768 字节

FlightControl 的 BSS 很大。虽然链接成功，实际运行前仍要结合 DDR 地址、链接脚本和 FreeRTOS heap 专项审计。

## 6. 已知 warning 和技术债

1. 自定义 AXI UART、AD7606 self-test：
   - `u32` 与 `int` 比较；
   - 未使用变量 `Index`；
   - 两个 BSP 各生成一份，所以 Problems 重复。
2. `FALSE` 在用户头文件和 Xilinx 头文件中重复定义。
3. CRC 参数丢弃 `const`。
4. `ServoControl.c` 的 `SetPD_Enable/SetPD_Status` 存在隐式声明。
5. `mavlink_SendFuncs.c` 的 `volt/current/RMPercent` 可能未初始化。
6. 若干变量未使用或只赋值不使用。
7. 构建会尝试不存在的 `a9-linaro-pre-build-step`，错误被 Makefile 忽略，ELF 仍成功生成。
8. FlightControl 只有启动两行日志，没有周期心跳。

不要只修改 BSP 生成副本来消除自定义 IP warning。应优先修改硬件平台的驱动源模板，再同步/重新生成两个 BSP。

## 7. JTAG 与串口

### 7.1 尚未通过的硬件验证

此前 Program FPGA 报告：

```text
Could not find FPGA device on the board for connection 'Local'.
```

开发副本创建后只完成离线构建，没有重新执行 JTAG `targets`，没有 Program FPGA，没有下载 ELF，也没有运行 FlightControl。

### 7.2 输出通道

FlightControl BSP：

```text
stdin  = ps7_coresight_comp_0
stdout = ps7_coresight_comp_0
```

因此应用 `xil_printf/printf` 走 ARM CoreSight DCC/JTAG 虚拟串口。XSCT 操作：

```tcl
connect
targets
targets -set -nocase -filter {name =~ "ARM*#0"}
jtagterminal
```

或用 `readjtaguart` 在 XSCT Console 中读取。

FSBL BSP：

```text
stdin  = ps7_uart_0
stdout = ps7_uart_0
```

FSBL 输出走物理 PS UART0，不会出现在 DCC。FlightControl 初始化时会把 PS UART0 改为 420000、8E2，供接收机使用，不能当普通 115200 应用日志口。

`start_task` 完成初始化后预期输出：

```text
FreeRTOS MainApp
Free heap: ... bytes
```

之后没有周期日志。

## 8. 安全边界

当前未执行：

- 开发副本 Program FPGA；
- JTAG 下载并运行 FlightControl；
- QSPI 擦写；
- 重新制作或烧写 BOOT.BIN；
- 电机、舵机、继电器和配电通道实机验证。

板卡有 28 V 输入和上电控制信号，程序包含 PD/SX、舵机和电机控制。首次运行前必须：

1. 断开电机、舵机、继电器及危险负载；
2. 保留立即断电手段；
3. 先只读验证 JTAG 链；
4. 下载后停在 `main()`，不要直接全速 Run；
5. 分段执行初始化并检查 GPIO 默认状态；
6. 不调用 QSPI 擦除/写入；
7. 未完成通道—负载映射前，不执行 `SetPD_*`、`SetSX_*`、`Servo_init`、`wp40_Init`。

`init_drv_Main()` 当前会执行 `PD_SX_Init()`、ADC、PS/PL UART、QSPI 和传感器初始化。虽然舵机、电机初始化已注释，仍应视为尚未完成安全审计。

## 9. 下一步建议

1. 危险负载断开后安全供电。
2. 打开 dev Workspace，不要打开 legacy 作为开发环境。
3. XSCT 执行：
   ```tcl
   connect
   targets
   ```
4. 若看不到 Zynq/ARM，检查板上电源、上电控制、下载器驱动、USB/JTAG接线、电平、状态灯、复位和启动模式。
5. 看到 ARM 后，在 `main()` 设断点，通过 System Debugger 下载 Debug ELF。
6. 确认停在 `main()` 后启动 `jtagterminal` 或 `readjtaguart`。
7. 分段运行到 FreeRTOS 和 `start_task`，记录卡点与 Call Stack。
8. 两行启动信息只证明主流程启动，之后继续验证 GPIO、ADC、传感器、CAN、PL UART、PS UART。
9. 增加不改变外部输出的低频心跳和初始化结果汇总。
10. 处理未初始化变量、隐式声明和内存布局。
11. 建立“接口—针脚—FPGA IP—地址—BSP驱动—应用模块—实际负载”映射表。
12. 固化 2019.1 硬件基线后迁移到 Vitis 2024.2，2026 版本最后评估。

## 10. 给下一个 AI 的约束

- 默认只修改 `D:\ZynqWork\flightcontrol-ucos145`。
- 不覆盖、不清理 `D:\ZynqWork\legacy-2019.1\FlightControl`。
- 不要同时把两个目录当成同一个 SDK Workspace。
- 五个 SDK 项目名故意保持原名，暂不重命名。
- 不把“编译成功”描述成“板卡已验证成功”。
- JTAG 先做 `connect/targets` 等只读检查，再决定下载。
- Program FPGA、运行、BOOT.BIN、QSPI 操作前重新检查硬件负载和用户授权。
- 修改 BSP 时区分生成文件与驱动源模板，避免 regenerate 后丢失修复。
- 修改飞控逻辑前先审计输出默认状态和初始化调用链。
- SDK 再出现 CDT 错误时，正常退出后用 `-workspace ... -eclipseargs -clean` 重启。
