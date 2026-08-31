# FlightControl 飞控开发副本

## 工作区定位

- 开发副本：`D:\ZynqWork\flightcontrol-dev-2019.1\FlightControl`
- 只读基线：`D:\ZynqWork\legacy-2019.1\FlightControl`
- 工具版本：Xilinx SDK 2019.1

日常开发只修改本目录。基线目录用于对照和回退，不要与本目录同时作为同一个 SDK Workspace 打开。

## 已完成的迁移

- 复制硬件平台、FSBL、两个 BSP、FlightControl 源码、bitstream、HDF 和 bootimage 参考文件；
- 未复制旧 Workspace 的 `.metadata`，已重新导入五个 SDK 项目；
- 未复制旧 Debug/Release 构建目录，已在新路径重新生成；
- 调试 Tcl 脚本中的 HDF、bitstream、PS7 初始化文件和 ELF 路径已迁移；
- `libmetal` 工具链路径及 CMake 构建缓存已在新目录重新生成；
- `FSBL_bsp`、`FlightControl_bsp`、`FSBL/Debug`、`FlightControl/Debug` 均已构建成功。

## 当前构建产物

- `FSBL\Debug\FSBL.elf`
- `FlightControl\Debug\FlightControl.elf`
- `FlightControl_bsp\ps7_cortexa9_0\lib\libmetal.a`

## 安全边界

当前只验证了离线构建，没有执行 Program FPGA、JTAG 下载、运行飞控程序或擦写 QSPI。首次上板前应断开电机、舵机、配电输出等危险负载，并先让调试器停在 `main()`。

## 已知警告

- `FALSE` 宏重复定义；
- `SetPD_Enable`、`SetPD_Status` 存在隐式函数声明；
- MAVLink SYS_STATUS 中的 `volt`、`current`、`RMPercent` 可能未初始化；
- 部分变量未使用、CRC 接口丢弃 `const` 限定。

这些警告没有阻止当前 Debug 构建，但进入实际上板和控制逻辑开发前应分级处理。
