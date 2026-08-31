# FlightControl Firmware

[![Version](https://img.shields.io/badge/version-0.1.0--alpha.1-orange)](./VERSION)
[![Platform](https://img.shields.io/badge/platform-Zynq--7000-blue)](#hardware-and-toolchain)
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-2ea44f)](#hardware-and-toolchain)
[![Status](https://img.shields.io/badge/status-pre--flight%20development-red)](#project-status)

FlightControl Firmware is an experimental flight-control software platform for a
Zynq-7000 SoC. The current codebase targets the ARM Cortex-A9 Processing System
(PS), uses FreeRTOS, and retains the existing FPGA bitstream and Xilinx SDK 2019.1
hardware platform.

The project is being developed first as a safe, testable ARM-side foundation for
multirotor flight control. Hexacopter support is the first target; fixed-wing and
other vehicle types are planned through vehicle-specific control and allocation
modules.

> [!WARNING]
> This repository is pre-flight software. Offline compilation has been verified,
> but the current development image has not completed JTAG, FPGA programming,
> actuator, propulsion, power-distribution, or flight validation. Do not connect
> propellers, motors, servos, relays, or other hazardous loads when running an
> unreviewed build. Historical boot images must not be flashed without a separate
> verification procedure.

## Project Status

Current version: [`0.1.0-alpha.1`](./VERSION)

| Area | Status |
|---|---|
| Xilinx SDK 2019.1 workspace migration | Complete |
| FSBL BSP offline build | Verified |
| FSBL Debug build | Verified |
| FlightControl FreeRTOS BSP build | Verified |
| FlightControl Debug build | Verified |
| FlightControl Release build | Not yet fixed |
| JTAG chain on the current development workspace | Not verified |
| FPGA programming and ELF execution | Not verified |
| Sensor and I/O validation on hardware | Not verified |
| Actuator and propulsion output | Disabled/not approved for testing |
| Flight-control algorithms | Initial framework only |
| QGroundControl integration | Planned |
| Vitis 2024.2 migration | Deferred |

The immediate development priority is:

1. establish a reproducible source and build baseline;
2. make actuator, power, and flash writes safe by default;
3. add monotonic time, diagnostics, and task health monitoring;
4. introduce timestamped data models and non-blocking sensor paths;
5. implement standard MAVLink services and QGroundControl connectivity;
6. add host-tested estimation and hexacopter control modules;
7. validate through SIL/HIL before powered hardware tests.

## Architecture

```text
Sensors and external inputs
  IMU / GF404 / GNSS / ADC / RC / MAVLink / CAN
                         |
                         v
Board support and drivers
  UART / SPI / CAN / GPIO / QSPI / custom AXI IP
                         |
                         v
Flight-control platform services
  time / status / health / parameters / logging / message bus
                         |
                         v
Estimation -> guidance -> control -> control allocation
                         |
                         v
Safety and actuator arbitration
                         |
                         v
PWM / CAN ESC / servo / WP40 / power distribution
```

The intended design separates:

- board and Xilinx-specific code from platform-independent algorithms;
- sensor drivers from state estimation;
- control laws from actuator drivers;
- vehicle-independent services from hexacopter or fixed-wing behavior;
- operator commands from the final onboard safety decision.

Real-time stabilization and failsafe behavior must remain onboard. A ground
station is used for monitoring, configuration, mission planning, and command
delivery, but is never part of the inner control loop.

## Repository Layout

```text
.
├─ design_top_wrapper_hw_platform_0/  Imported hardware platform, HDF and bitstream
├─ FSBL_bsp/                          Standalone BSP used by the FSBL
├─ FSBL/                              Zynq first-stage bootloader application
├─ FlightControl_bsp/                 FreeRTOS/Xilinx BSP for the main application
├─ FlightControl/                     Main ARM flight-control application
│  └─ src/
│     ├─ main.c                       FreeRTOS entry point
│     ├─ flight_control/              Current 1 ms control-task framework
│     ├─ ucas/                        Existing board and peripheral drivers
│     └─ mavlink/                     Generated MAVLink C headers
├─ bootimage/                         Historical BIF and boot-image references
├─ AI_HANDOFF.md                      Detailed engineering state and safety notes
├─ MIGRATION_README.md                SDK workspace migration record
├─ CHANGELOG.md                       User-visible version history
└─ VERSION                            Canonical firmware version
```

Eclipse workspace metadata, compiler outputs, processor-specific BSP generation
trees, logs, and historical boot binaries are excluded from version control. BSP
project descriptors and `system.mss` files are tracked so SDK 2019.1 can regenerate
the libraries. Application source, custom hardware-platform drivers, hardware
handoff files, and the reference bitstream remain versioned so the current SDK
platform can be reconstructed.

## Hardware and Toolchain

| Item | Current baseline |
|---|---|
| Device family | Zynq-7000 |
| HDF device identifier | `XA7Z020CLG484-1Q` |
| CPU | `ps7_cortexa9_0` / ARM Cortex-A9 |
| SDK | Xilinx SDK 2019.1 |
| Application OS | `freertos10_xilinx 1.3` |
| FSBL OS | `standalone 7.0` |
| Application compiler | ARM GNU, hard-float configuration |
| Ground station direction | QGroundControl through MAVLink 2 |

The device identifier must be checked against the physical component marking and
schematic before hardware regeneration or boot-image programming.

The repository contains an imported HDF and existing bitstream, not a complete
Vivado source project. It does not currently provide a reliable way to modify pin
constraints, clocks, block design, or custom IP RTL and regenerate the PL design.

## Getting Started

### 1. Clone

Use a short, ASCII-only path when possible because the legacy SDK and generated
build tools can be sensitive to spaces and non-ASCII paths.

```powershell
git clone https://github.com/waitter-01/flightcontrol-firmware.git D:\ZynqWork\flightcontrol-firmware
cd D:\ZynqWork\flightcontrol-firmware
```

### 2. Install the supported toolchain

Install Xilinx SDK 2019.1. The currently verified installation is located at:

```text
D:\Xilinx\SDK\2019.1
```

Other SDK releases may regenerate BSP content or change compiler behavior. Do not
upgrade the project implicitly; toolchain migration is a separate tracked task.

### 3. Open a fresh workspace

The repository intentionally does not track `.metadata`. Start SDK with the clone
directory as the workspace, then import the existing projects.

```powershell
& 'D:\Xilinx\SDK\2019.1\bin\xsdk.bat' `
  -workspace 'D:\ZynqWork\flightcontrol-firmware' `
  -eclipseargs -clean
```

Import these projects using **File → Import → General → Existing Projects into
Workspace** without copying them:

1. `design_top_wrapper_hw_platform_0`
2. `FSBL_bsp`
3. `FSBL`
4. `FlightControl_bsp`
5. `FlightControl`

### 4. Build

Recommended order:

1. regenerate/build `FSBL_bsp` after a fresh clone or when its hardware/BSP
   settings changed;
2. build `FSBL` using the Debug configuration;
3. regenerate/build `FlightControl_bsp` after a fresh clone or when required;
4. build `FlightControl` using the Debug configuration.

Expected local artifacts:

```text
FSBL/Debug/FSBL.elf
FlightControl/Debug/FlightControl.elf
FlightControl_bsp/ps7_cortexa9_0/lib/libmetal.a
```

These artifacts are intentionally ignored by Git. A command-line reproducible
build script is planned; until then, record the SDK console output when changing
project or BSP configuration.

## Runtime Notes

- FlightControl `stdin`/`stdout` use `ps7_coresight_comp_0`, so application
  `printf`/`xil_printf` output is expected through JTAG DCC rather than a normal
  serial terminal.
- FSBL output uses physical `ps7_uart_0`.
- The application later configures PS UART0 for a receiver link at 420000 baud,
  8 data bits, even parity, and 2 stop bits.
- The current `flight_control_task` runs on a 1 ms schedule but the actual control
  algorithm is largely unimplemented.
- Existing initialization includes power-distribution GPIO, ADC, PS/PL UART,
  QSPI, and sensor setup. Servo and WP40 initialization are currently commented
  out, but that alone is not a complete safety guarantee.

## Safety Rules

Before any hardware run:

1. disconnect motors, propellers, servos, relays, and hazardous loads;
2. retain an immediate physical power-removal method;
3. verify the JTAG chain using read-only target discovery first;
4. halt at `main()` before allowing initialization to continue;
5. inspect GPIO and power-distribution defaults one subsystem at a time;
6. do not invoke QSPI erase/write functions;
7. do not call `SetPD_*`, `SetSX_*`, `Servo_init`, or `wp40_Init` until the
   channel-to-load mapping and safe state are documented and reviewed;
8. do not flash historical `MAINBOOT.bin` or `BACKBOOT.bin` images.

Safety behavior is part of the firmware architecture, not an operator procedure
alone. Development builds will evolve toward actuator and flash writes being
disabled by default and enabled only through explicit, audited state transitions.

## Development Roadmap

- [x] Restore the SDK 2019.1 offline Debug build baseline.
- [x] Create a clean development workspace separate from the legacy reference.
- [ ] Add default actuator/power/flash-write guards.
- [ ] Add monotonic time, status events, and RTOS health monitoring.
- [ ] Audit linker memory, heap, BSS, and task stack usage.
- [ ] Replace infinite waits in control paths with timestamped validity checks.
- [ ] Add static data topics and a non-blocking message path.
- [ ] Refactor GF404, ADC, RC, and communication drivers behind stable interfaces.
- [ ] Implement RAM-backed parameters and standard MAVLink microservices.
- [ ] Verify basic connectivity with unmodified QGroundControl.
- [ ] Add host-tested attitude estimation.
- [ ] Add hexacopter rate/attitude control and control allocation.
- [ ] Establish repeatable SIL and HIL fault-injection tests.
- [ ] Perform a documented, load-disconnected hardware validation.
- [ ] Extend the vehicle layer for position control and additional airframes.
- [ ] Migrate to Vitis only after the 2019.1 baseline is reproducible on hardware.

## Versioning and Releases

This project follows [Semantic Versioning](https://semver.org/):

```text
MAJOR.MINOR.PATCH[-PRERELEASE]
```

- `0.x.y` indicates active development with no stable flight interface guarantee;
- prerelease identifiers such as `alpha`, `beta`, and `rc` indicate validation
  maturity;
- the canonical current version is stored in [`VERSION`](./VERSION);
- user-visible changes are recorded in [`CHANGELOG.md`](./CHANGELOG.md);
- release tags use the form `vMAJOR.MINOR.PATCH[-PRERELEASE]`;
- a release must identify the firmware commit, hardware revision, HDF/bitstream
  hash, parameter schema, toolchain, and validation level.

Suggested branch names:

```text
feature/<short-name>
fix/<short-name>
docs/<short-name>
test/<short-name>
release/<version>
```

Suggested commit prefixes follow the Conventional Commits style:

```text
feat:     new behavior
fix:      defect correction
docs:     documentation only
refactor: behavior-preserving code change
test:     tests and test infrastructure
build:    build system or toolchain
chore:    repository maintenance
```

See [`CONTRIBUTING.md`](./CONTRIBUTING.md) for the required development and safety
checks.

## Known Technical Debt

- FlightControl Release configuration is missing include paths.
- The linked BSS is unusually large and the linker script reserves a large heap;
  DDR and memory layout require a dedicated audit.
- Some task and queue handles are defined in headers rather than declared `extern`.
- Control and sensor paths contain indefinite FreeRTOS waits.
- Some MAVLink status values may be used before initialization.
- Existing code has macro redefinitions, implicit declarations, discarded
  qualifiers, and unused variables.
- The current application has minimal runtime diagnostics and no periodic health
  heartbeat.

Detailed context is available in [`AI_HANDOFF.md`](./AI_HANDOFF.md).

## Contributing

Contributions must preserve the default-safe behavior and keep the legacy
reference workspace untouched. Every change should include:

- a narrow objective and explicit non-goals;
- successful Debug build evidence;
- warning and ELF size comparison;
- tests for platform-independent logic;
- an assessment of actuator, power, QSPI, timing, and memory impact;
- updated documentation and changelog entries when behavior changes.

## License

A project-level license has not yet been selected. Do not assume permission to
redistribute the repository or third-party code beyond the terms already attached
to individual components. The Xilinx, FreeRTOS, libmetal, and MAVLink-derived
files may carry their own licenses and notices; these must be preserved.
