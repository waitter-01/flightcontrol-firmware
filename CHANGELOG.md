# 变更记录

本文件记录 FlightControl 飞控固件的所有重要变化。

格式参考 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，版本号遵循
[语义化版本](https://semver.org/lang/zh-CN/)。

## [未发布]

### 新增

- 增加由当前 HDF 固化的板级硬件资源表和风险等级；
- 增加不重新初始化 GIC 的 μC/OS-III 中断注册接口；
- 增加始终把通道配置为输入的 AXI GPIO 只读驱动；
- 增加主要函数的中文 Doxygen 注释规范；
- 增加基于 Micrium Xilinx Repository 1.44 的 `FlightControl_ucos` 模板应用；
- 增加 μC/OS-III BSP 配置，启用 1 kHz 系统节拍、运行统计、任务栈检查和任务性能统计；
- 增加可重复注册 Micrium 软件仓库、再生成 BSP 和构建模板工程的 XSCT 脚本；
- 增加 μC/OS-III 环境搭建、许可边界和后续驱动迁移说明。
- 建立 `core`、`platform`、`bsp`、`drivers`、`services` 五层 ARM 软件骨架；
- 增加基于 Zynq 全局定时器的单调时间接口和 μC/OS-III 分级日志接口；
- 增加默认锁定的板级安全状态机、组件心跳监测和健康监测任务；
- 增加硬件看门狗受控适配接口，当前版本明确禁止启动硬件后端；
- 增加 μC/OS-III 危险硬件调用静态检查脚本。

### 变更

- 将 README、变更记录、开发规范和仓库配置注释统一为中文。
- μC/OS-III 模板的标准输入和标准输出改用 `ps7_uart_0`。

### 已验证

- 使用 Xilinx SDK 2019.1 和 Micrium Xilinx Repository 1.44 生成 μC/OS-III BSP；
- `FlightControl_ucos_bsp` 离线构建；
- `FlightControl_ucos` Debug 构建，生成 ARM Cortex-A9 ELF。
- 新增基础层和健康监测任务在 ARM GNU Debug 配置下无新增编译警告；
- 安全检查未发现 GPIO 输出、配电/继电器、执行机构、QSPI 写入或硬件看门狗启动调用。

## [0.1.0-alpha.1] - 2026-08-31

### 新增

- 增加仓库主页 README，说明架构、构建、安全边界和开发路线；
- 增加 Git 忽略和文本规范化规则，明确可重复维护的仓库边界；
- 增加 `VERSION`、变更记录和发布规则；
- 增加开发贡献与安全检查流程；
- 恢复面向 Zynq-7000 硬件平台的 Xilinx SDK 2019.1 工作区；
- 导入硬件平台、FSBL、Standalone BSP、FreeRTOS BSP 和 FlightControl 应用项目；
- 纳入现有 ARM 应用源码、板级驱动、自定义 AXI IP 驱动和 MAVLink 生成头文件；
- 纳入现有 HDF 和参考 FPGA bitstream；
- 增加工程交接和 Workspace 迁移文档。

### 已验证

- `FSBL_bsp` 离线构建；
- `FSBL` Debug 构建；
- `FlightControl_bsp` 离线构建，包括 libmetal；
- `FlightControl` 在独立开发 Workspace 中完成 Debug 构建。

### 已知限制

- 当前开发 Workspace 尚未完成 JTAG 和真实硬件验证；
- FlightControl Release 配置尚不能成功构建；
- 执行机构、动力系统、配电和 QSPI 写入路径尚未批准用于硬件测试；
- 飞行控制算法尚不完整；
- 缺少完整 Vivado 源工程。
