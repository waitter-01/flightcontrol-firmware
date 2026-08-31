# Changelog

All notable changes to FlightControl Firmware will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and the project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

No changes yet.

## [0.1.0-alpha.1] - 2026-08-31

### Added

- Repository-level README with architecture, build, safety, and roadmap guidance.
- Git ignore and text-normalization rules for a reproducible repository boundary.
- Canonical `VERSION` file, changelog, and documented release policy.
- Contribution and safety-check workflow.
- Recovered Xilinx SDK 2019.1 workspace for the Zynq-7000 hardware platform.
- Imported hardware platform, FSBL, standalone BSP, FreeRTOS BSP, and
  FlightControl application projects.
- Existing ARM application source, board drivers, custom AXI IP drivers, and
  generated MAVLink headers.
- Existing HDF and reference FPGA bitstream.
- Engineering handoff and workspace migration documentation.

### Verified

- Offline build of `FSBL_bsp`.
- Debug build of `FSBL`.
- Offline build of `FlightControl_bsp`, including libmetal.
- Debug build of `FlightControl` in the clean development workspace.

### Known limitations

- No successful JTAG or physical hardware validation has been completed for the
  development workspace.
- FlightControl Release configuration does not currently build.
- Actuator, propulsion, power-distribution, and QSPI write paths are not approved
  for hardware use.
- Flight-control algorithms are incomplete.
- The full Vivado source project is not available.
