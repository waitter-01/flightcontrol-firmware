# μC/OS-III 1.45 迁移说明

## 1. 工程边界

本分支使用独立 SDK 工作区：

```text
D:\ZynqWork\flightcontrol-ucos145
```

Micrium 软件仓库位于：

```text
D:\ZynqWork\ucos_v1_45\ucos
```

第三方 Micrium 源码和预编译库不提交到飞控仓库。Git 只记录应用源码、SDK 工程描述、
`system.mss`、恢复脚本和迁移文档。

## 2. 版本基线

| 项目 | 版本 |
|---|---|
| Xilinx SDK | 2019.1 |
| Micrium Xilinx Repository | 1.45 |
| μC/OS-III | 3.07.05 |
| 处理器 | Zynq-7000 Cortex-A9 / `ps7_cortexa9_0` |
| 构建配置 | ARM GNU Debug，硬浮点 |

## 3. 从 1.44 迁移的关键差异

v1.45 删除了三个旧版专用驱动。正确映射通过使用同一 HDF 生成纯净 v1.45 BSP 后确认：

| 硬件实例 | v1.44 | v1.45 |
|---|---|---|
| `ps7_l2cachec_0` | `ucos_l2cachec 1.44` | `generic 2.0` |
| `ps7_scuc_0` | `ucos_scuc 1.44` | `generic 2.0` |
| `ps7_scutimer_0` | `ucos_scutimer 1.44` | `scutimer 2.1` |
| `ps7_uart_0` | `ucos_uartps 1.44` | `ucos_uartps 1.45` |

此外，v1.45 将 `UCOS_INT_FNCT_PTR` 对齐到 Xilinx `Xil_InterruptHandler`，回调签名从
`void (*)(void *, CPU_INT32U)` 改为 `void (*)(void *)`。平台中断适配器已经同步调整。

v1.45 的 standalone 兼容层已经提供 SDK 2019.1 所需的平台枚举，不再需要手工定义
`XPLAT_versal`。

## 4. 重建命令

在 PowerShell 中执行：

```powershell
cd D:\ZynqWork\flightcontrol-ucos145

.\scripts\setup_ucos3.ps1 `
  -SdkRoot 'D:\Xilinx\SDK\2019.1' `
  -MicriumRepository 'D:\ZynqWork\ucos_v1_45\ucos'
```

脚本会执行安全扫描、注册 v1.45 仓库、导入 SDK 工程、重新生成 BSP、构建 BSP 和应用，
并检查最终 ELF 是否由本次构建产生。只生成 BSP 时可以增加 `-SkipBuild`。

## 5. 当前验证结果

离线构建产物：

```text
FlightControl_ucos\Debug\FlightControl_ucos.elf
```

大小：

```text
text   127432 B
data     2508 B
bss     41312 B
total  171252 B
```

业务应用源码在 `-Wall -Wextra` 下没有新增迁移告警。仍存在的告警来自历史自定义 AXI
self-test 和 Micrium v1.45 PS UART 波特率搜索实现，不影响本次链接成功，但上机前应结合
对应硬件专项审查。

## 6. 上机边界

当前结果只证明 v1.45 BSP 与飞控应用可以离线生成、编译和链接。尚未验证 J-Link 启动、
DDR、系统节拍、GIC、中断回调、PS UART 或任何 PL 外设。首次上机必须断开螺旋桨、电机、
舵机、继电器和配电负载，并按统一上机测试计划逐级执行。
