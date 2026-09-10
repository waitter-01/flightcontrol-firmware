# FlightControl 飞控开发副本

## 工作区定位

- 当前开发副本：`D:\ZynqWork\flightcontrol-ucos145`
- 当前 Git 分支：`feature/ucos145-migration`
- Micrium 仓库：`D:\ZynqWork\ucos_v1_45\ucos`（版本 1.45）
- 旧开发副本：`D:\ZynqWork\flightcontrol-dev-2019.1\FlightControl`（仅作对照）
- 只读基线：`D:\ZynqWork\legacy-2019.1\FlightControl`
- 工具版本：Xilinx SDK 2019.1

日常开发只修改本目录。基线目录用于对照和回退，不要与本目录同时作为同一个 SDK Workspace 打开。

## 已完成的迁移

- 复制硬件平台、FSBL、两个 BSP、FlightControl 源码、bitstream、HDF 和 bootimage 参考文件；
- 未复制旧 Workspace 的 `.metadata`，已重新导入五个 SDK 项目；
- 未复制旧 Debug/Release 构建目录，已在新路径重新生成；
- 调试 Tcl 脚本中的 HDF、bitstream、PS7 初始化文件和 ELF 路径已迁移；
- `libmetal` 工具链路径及 CMake 构建缓存已在新目录重新生成；
- 将 μC/OS-III BSP 和库从 Micrium 1.44 升级到 1.45；
- 按 1.45 组件清单重新映射 L2 Cache、SCU、SCU Timer 和 PS UART 驱动；
- `FlightControl_ucos_bsp` 与 `FlightControl_ucos/Debug` 已离线构建成功。

## 当前构建产物

- `FlightControl_ucos\Debug\FlightControl_ucos.elf`
- `FlightControl_ucos_bsp\ps7_cortexa9_0\lib\libxil.a`

## 安全边界

当前只验证了离线构建，没有执行 Program FPGA、JTAG 下载、运行飞控程序或擦写 QSPI。首次上板前应断开电机、舵机、配电输出等危险负载，并先让调试器停在 `main()`。

## 已知警告

- 自定义 AXI UART、AD7606 驱动仍有符号比较和未使用变量警告；
- Micrium 1.45 的 `ucos_uartps.c` 存在波特率局部变量可能未初始化警告；
- `xtime_l.h` 输出兼容性提示。

这些警告没有阻止当前 Debug 构建，但进入实际上板和控制逻辑开发前应分级处理。
