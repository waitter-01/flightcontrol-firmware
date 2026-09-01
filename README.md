# FlightControl 飞控固件

[![版本](https://img.shields.io/badge/版本-0.1.0--alpha.1-orange)](./VERSION)
[![平台](https://img.shields.io/badge/平台-Zynq--7000-blue)](#硬件与工具链)
[![实时系统](https://img.shields.io/badge/实时系统-FreeRTOS%20%7C%20μC%2FOS--III-2ea44f)](#硬件与工具链)
[![状态](https://img.shields.io/badge/状态-飞控开发阶段-red)](#项目状态)

FlightControl 是一个面向 Zynq-7000 SoC 的实验性飞控软件平台。当前代码运行在
ARM Cortex-A9 处理系统（PS）上。现有主应用使用 FreeRTOS；仓库同时提供已完成离线
构建的 μC/OS-III 1.44 模板，作为后续驱动解耦和系统迁移的目标工程。两套工程沿用同一
FPGA bitstream 和 Xilinx SDK 2019.1 硬件平台。

项目首先建设安全、可测试的 ARM 端飞控基础平台，首个目标机型为六旋翼；后续通过
机型专用的控制器和控制分配模块扩展固定翼及其他飞行器。

> [!WARNING]
> 当前仓库属于飞行前开发软件。离线编译已经验证，但当前开发版本尚未完成 JTAG、
> FPGA 下载、执行机构、动力系统、配电和实飞验证。运行未经审查的版本时，不得连接
> 螺旋桨、电机、舵机、继电器或其他危险负载。历史启动镜像未经专项验证不得烧写。

## 项目状态

当前版本：[`0.1.0-alpha.1`](./VERSION)

| 模块 | 状态 |
|---|---|
| Xilinx SDK 2019.1 工作区迁移 | 已完成 |
| FSBL BSP 离线构建 | 已验证 |
| FSBL Debug 构建 | 已验证 |
| FlightControl FreeRTOS BSP 构建 | 已验证 |
| FlightControl Debug 构建 | 已验证 |
| FlightControl μC/OS-III BSP 构建 | 已验证 |
| FlightControl μC/OS-III 模板构建 | 已验证 |
| μC/OS-III 基础分层与安全门 | 已完成 |
| FreeRTOS 外设驱动向 μC/OS-III 迁移 | 第四阶段已完成 |
| FlightControl Release 构建 | 尚未修复 |
| 当前开发工作区 JTAG 链 | 尚未验证 |
| FPGA 下载与 ELF 运行 | 尚未验证 |
| 传感器和板级 I/O 实机验证 | 尚未验证 |
| 执行机构和动力输出 | 禁止测试/尚未批准 |
| 飞行控制算法 | 仅有初始框架 |
| QGroundControl 集成 | MAVLink 基础接口已完成，待联机验证 |
| Vitis 2024.2 迁移 | 暂缓 |

近期开发优先级：

1. 建立可重复恢复和构建的源码基线；
2. 让执行机构、配电和 Flash 写入默认处于禁止状态；
3. 增加单调时间、诊断和任务健康监测；
4. 建立带时间戳的数据模型和非阻塞传感器通路；
5. 实现标准 MAVLink 服务和 QGroundControl 连接；
6. 增加可在 PC 上测试的状态估计和六旋翼控制模块；
7. 在带动力硬件测试前完成 SIL/HIL 验证。

## 总体架构

```text
传感器与外部输入
  IMU / GF404 / GNSS / ADC / RC / MAVLink / CAN
                         │
                         ▼
板级支持与驱动
  UART / SPI / CAN / GPIO / QSPI / 自定义 AXI IP
                         │
                         ▼
飞控公共服务
  时间 / 状态 / 健康监测 / 参数 / 日志 / 消息总线
                         │
                         ▼
状态估计 → 导航制导 → 控制器 → 控制分配
                         │
                         ▼
安全状态机与执行机构仲裁
                         │
                         ▼
PWM / CAN ESC / 舵机 / WP40 / 配电控制
```

设计目标是明确分离：

- 板卡/Xilinx 专用代码与平台无关算法；
- 传感器驱动与状态估计；
- 控制律与执行机构驱动；
- 公共飞控服务与六旋翼、固定翼等机型逻辑；
- 地面站操作命令与机载最终安全决策。

实时稳定控制和失效保护必须留在机载端。地面站用于监控、配置、任务规划和命令下发，
不能成为内环控制链的一部分。

## 仓库结构

```text
.
├─ design_top_wrapper_hw_platform_0/  导入的硬件平台、HDF 和 bitstream
├─ FSBL_bsp/                          FSBL 使用的 Standalone BSP 配置
├─ FSBL/                              Zynq 第一阶段启动程序
├─ FlightControl_bsp/                 主应用的 FreeRTOS/Xilinx BSP 配置
├─ FlightControl/                     ARM 飞控主应用
│  └─ src/
│     ├─ main.c                       FreeRTOS 程序入口
│     ├─ flight_control/              当前 1 ms 控制任务框架
│     ├─ ucas/                        现有板级和外设驱动
│     └─ mavlink/                     生成的 MAVLink C 头文件
├─ FlightControl_ucos_bsp/            μC/OS-III 1.44 BSP 配置
├─ FlightControl_ucos/                μC/OS-III ARM 模板应用
│  └─ src/
│     ├─ core/                        公共状态码
│     ├─ platform/                    单调时间和日志
│     ├─ bsp/                         板级安全状态
│     ├─ drivers/                     独立硬件驱动适配
│     └─ services/                    健康监测等公共服务
├─ scripts/                            环境恢复和构建脚本
├─ docs/                               专题开发文档
├─ bootimage/                         历史 BIF 参考文件
├─ AI_HANDOFF.md                      工程状态和安全交接说明
├─ MIGRATION_README.md                SDK 工作区迁移记录
├─ CHANGELOG.md                       版本变更记录
└─ VERSION                            当前固件版本
```

仓库不跟踪 Eclipse Workspace 元数据、编译输出、处理器专用 BSP 生成目录、日志和历史
BOOT.BIN。BSP 的工程描述和 `system.mss` 会被跟踪，由 SDK 2019.1 重新生成相关库。
应用源码、硬件平台中的自定义驱动、硬件交接文件和参考 bitstream 会进入版本管理，以便
恢复当前 SDK 硬件平台。

## 硬件与工具链

| 项目 | 当前基线 |
|---|---|
| 器件系列 | Zynq-7000 |
| HDF 器件标识 | `XA7Z020CLG484-1Q` |
| CPU | `ps7_cortexa9_0` / ARM Cortex-A9 |
| SDK | Xilinx SDK 2019.1 |
| 现有主应用操作系统 | `freertos10_xilinx 1.3` |
| 迁移模板操作系统 | Micrium `ucos 1.44` |
| FSBL 操作系统 | `standalone 7.0` |
| 应用编译器 | ARM GNU，硬浮点配置 |
| 地面站方向 | QGroundControl + MAVLink 2 |

在重新生成硬件或烧写启动镜像前，必须根据芯片丝印和原理图复核器件型号，不能只依据
HDF 中的标识选型。

仓库包含导入的 HDF 和现有 bitstream，但不包含完整 Vivado 源工程。目前无法可靠修改
管脚约束、时钟、Block Design 或定制 IP RTL，也不能保证重新生成相同的 PL 设计。

## 快速开始

### 1. 克隆仓库

旧版 SDK 和生成工具可能对空格、中文路径较敏感，建议使用较短的纯英文目录。

```powershell
git clone https://github.com/waitter-01/flightcontrol-firmware.git D:\ZynqWork\flightcontrol-firmware
cd D:\ZynqWork\flightcontrol-firmware
```

### 2. 安装指定工具链

安装 Xilinx SDK 2019.1。当前已经验证的安装位置是：

```text
D:\Xilinx\SDK\2019.1
```

其他 SDK 版本可能重新生成 BSP 或改变编译器行为。不得隐式升级工程；工具链升级必须作为
独立任务进行验证和记录。

### 3. 打开全新 Workspace

仓库不会跟踪 `.metadata`。使用克隆目录作为 Workspace 启动 SDK，然后导入现有项目。

```powershell
& 'D:\Xilinx\SDK\2019.1\bin\xsdk.bat' `
  -workspace 'D:\ZynqWork\flightcontrol-firmware' `
  -eclipseargs -clean
```

在 SDK 中选择 **File → Import → General → Existing Projects into Workspace**，
依次导入以下项目，且不要勾选复制：

1. `design_top_wrapper_hw_platform_0`
2. `FSBL_bsp`
3. `FSBL`
4. `FlightControl_bsp`
5. `FlightControl`
6. `FlightControl_ucos_bsp`
7. `FlightControl_ucos`

### 4. 构建工程

推荐构建顺序：

1. 首次克隆后生成/构建 `FSBL_bsp`，硬件或 BSP 设置变化后重新生成；
2. 使用 Debug 配置构建 `FSBL`；
3. 首次克隆后生成/构建 `FlightControl_bsp`，需要时重新生成；
4. 使用 Debug 配置构建 `FlightControl`。

μC/OS-III 工程需要先在 SDK 中注册本地 Micrium Xilinx Repository 1.44。推荐使用
[`scripts/setup_ucos3.ps1`](./scripts/setup_ucos3.ps1) 自动恢复环境、重新生成 BSP 并构建；
完整说明见 [`docs/UCOS3_环境与模板工程.md`](./docs/UCOS3_环境与模板工程.md)。
驱动分层、安全门和上机前检查见
[`docs/UCOS3_驱动解耦第一阶段.md`](./docs/UCOS3_驱动解耦第一阶段.md)。
硬件资源表、统一中断和只读 GPIO 规则见
[`docs/UCOS3_第二阶段_硬件资源与中断GPIO.md`](./docs/UCOS3_第二阶段_硬件资源与中断GPIO.md)。

预期本地产物：

```text
FSBL/Debug/FSBL.elf
FlightControl/Debug/FlightControl.elf
FlightControl_bsp/ps7_cortexa9_0/lib/libmetal.a
FlightControl_ucos/Debug/FlightControl_ucos.elf
```

这些产物不会进入 Git。μC/OS-III 已提供命令行恢复和构建脚本；现有 FreeRTOS 主工程仍按
上述 SDK 构建顺序维护。修改项目或 BSP 配置时应保存 SDK Console 的完整构建输出。

## 运行说明

- FlightControl 的 `stdin`/`stdout` 使用 `ps7_coresight_comp_0`，应用层
  `printf`/`xil_printf` 默认通过 JTAG DCC 输出，而不是普通串口；
- FSBL 输出使用物理 `ps7_uart_0`；
- 应用随后会把 PS UART0 配置为接收机链路：420000 波特率、8 数据位、偶校验、2 停止位；
- 当前 `flight_control_task` 按 1 ms 周期运行，但实际控制算法基本尚未实现；
- 现有初始化包含配电 GPIO、ADC、PS/PL UART、QSPI 和传感器；
- 舵机和 WP40 初始化虽然已被注释，但这并不构成完整的安全保证。

## 安全规则

任何上板运行前必须：

1. 断开电机、螺旋桨、舵机、继电器和其他危险负载；
2. 保留可以立即物理断电的手段；
3. 先用只读目标发现验证 JTAG 链；
4. 下载 ELF 后先停在 `main()`，再决定是否继续初始化；
5. 分模块检查 GPIO 和配电默认状态；
6. 不调用任何 QSPI 擦除或写入函数；
7. 通道—负载映射和安全状态完成记录、复核前，不调用 `SetPD_*`、`SetSX_*`、
   `Servo_init` 或 `wp40_Init`；
8. 不烧写历史 `MAINBOOT.bin` 或 `BACKBOOT.bin`。

安全必须由固件架构保证，不能只依赖操作人员记住流程。后续开发版本将让执行机构和 Flash
写入默认关闭，只允许通过明确、可审计的安全状态转换启用。

## 开发路线

- [x] 恢复 SDK 2019.1 离线 Debug 构建基线；
- [x] 建立与 legacy 对照区分离的开发工作区；
- [x] 建立 GitHub 仓库、版本规范和首个预发布版本；
- [x] 建立 μC/OS-III 1.44 BSP、模板应用和可重复构建脚本；
- [x] 建立独立驱动接口、单调时间、日志、安全门和健康监测；
- [ ] 断负载验证基础模板的串口、Tick、任务调度和全局定时器；
- [ ] 在硬件资源表确认后实现 GPIO 输入、LED 和硬件看门狗后端；
- [ ] 增加执行机构、配电和 Flash 写入默认保护；
- [ ] 增加单调时间、状态事件和 RTOS 健康监测；
- [ ] 审计链接内存、heap、BSS 和任务栈；
- [ ] 用带时间戳的有效性检查替换控制路径中的无限等待；
- [ ] 建立静态数据主题和非阻塞消息通路；
- [ ] 将 GF404、ADC、RC 和通信驱动封装到稳定接口后；
- [ ] 实现 RAM 参数和标准 MAVLink 微服务；
- [ ] 使用未修改的 QGroundControl 验证基础连接；
- [ ] 增加可在 PC 上测试的姿态估计；
- [ ] 增加六旋翼角速度/姿态控制和控制分配；
- [ ] 建立可重复的 SIL/HIL 故障注入测试；
- [ ] 完成有记录的断负载上板验证；
- [ ] 扩展位置控制和其他机型；
- [ ] 2019.1 实机基线可重复后再迁移 Vitis。

## 版本与发布

项目遵循[语义化版本](https://semver.org/lang/zh-CN/)：

```text
主版本.次版本.修订版本[-预发布标识]
MAJOR.MINOR.PATCH[-PRERELEASE]
```

- `0.x.y` 表示处于快速开发期，不保证飞控接口稳定；
- `alpha`、`beta`、`rc` 等预发布标识表示不同验证成熟度；
- [`VERSION`](./VERSION) 是当前版本的唯一事实来源；
- 面向使用者的变化记录在 [`CHANGELOG.md`](./CHANGELOG.md)；
- 发布标签采用 `vMAJOR.MINOR.PATCH[-PRERELEASE]`；
- 每个发布版本必须记录固件提交、硬件版本、HDF/bitstream 哈希、参数 schema、
  工具链和验证等级。

建议分支命名：

```text
feature/<简短名称>
fix/<简短名称>
docs/<简短名称>
test/<简短名称>
release/<版本号>
```

提交信息采用 Conventional Commits 风格，类型前缀保持英文，说明使用中文：

```text
feat: 新增功能
fix: 修复缺陷
docs: 仅修改文档
refactor: 不改变行为的重构
test: 测试或测试基础设施
build: 构建系统或工具链
chore: 仓库维护
```

开发和安全检查要求见 [`CONTRIBUTING.md`](./CONTRIBUTING.md)。

## 已知技术债

- FlightControl Release 配置缺少部分 include path；
- ELF 的 BSS 异常大，链接脚本还保留了较大的 heap，需要专项审计 DDR 和内存布局；
- 部分任务和队列句柄直接定义在头文件中，没有使用 `extern`；
- 控制和传感器路径存在 FreeRTOS 无限等待；
- 部分 MAVLink 状态变量可能在初始化前使用；
- 现有代码存在宏重复定义、隐式声明、限定符丢失和未使用变量；
- 当前应用缺少完整的运行诊断和周期健康心跳。

详细工程状态见 [`AI_HANDOFF.md`](./AI_HANDOFF.md)。

## 参与开发

所有改动必须保持默认安全行为，且不得修改 legacy 只读基线。每个变更至少应包含：

- 清晰、单一的目标和明确的非目标；
- FlightControl Debug 构建证据；
- warning 和 ELF size 对比；
- 平台无关逻辑的测试；
- 对执行机构、配电、QSPI、实时性和内存的影响判断；
- 行为变化对应的文档和变更记录。

详细规范见 [`CONTRIBUTING.md`](./CONTRIBUTING.md)。

## 许可证

项目尚未选择顶层许可证。在许可证明确以前，不应自行假定可以重新分发本仓库或其中的
第三方代码。Xilinx、FreeRTOS、libmetal、MAVLink 和 Micrium 派生文件可能包含各自的
许可证与版权声明，必须保留并分别遵守。Micrium 源码不进入本仓库，商业使用 μC/OS-III
前必须确认具备有效授权。
