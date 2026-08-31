# 变更记录

本文件记录 FlightControl 飞控固件的所有重要变化。

格式参考 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)，版本号遵循
[语义化版本](https://semver.org/lang/zh-CN/)。

## [未发布]

### 变更

- 将 README、变更记录、开发规范和仓库配置注释统一为中文。

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
