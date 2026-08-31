# Contributing to FlightControl Firmware

FlightControl is safety-sensitive embedded software. A successful compilation is
necessary but is not sufficient evidence that a change is safe or correct.

## Development Baseline

- Develop only in the `flightcontrol-firmware` working tree.
- Keep the legacy SDK workspace read-only.
- Use Xilinx SDK 2019.1 unless a dedicated toolchain-migration change is being
  reviewed.
- Do not edit generated MAVLink headers directly; update the dialect source and
  regenerate them.
- Do not place application logic in generated BSP code.

## Branches

Create focused branches from `main`:

```text
feature/<short-name>
fix/<short-name>
docs/<short-name>
test/<short-name>
release/<version>
```

Keep changes small enough to build, review, and revert independently.

## Commits

Use an imperative Conventional Commits-style subject:

```text
feat: add monotonic microsecond time service
fix: reject stale sensor samples in control task
docs: document JTAG output channels
refactor: isolate GF404 parsing from UART driver
test: add control allocator saturation cases
build: exclude SDK-generated outputs
```

Do not combine generated workspace noise, formatting changes, and functional
changes in the same commit.

## Required Checks

Before proposing or merging a functional change:

- [ ] Build the FlightControl Debug configuration.
- [ ] Build the FSBL when boot or hardware-platform inputs changed.
- [ ] Review new and changed compiler warnings.
- [ ] Record `text`, `data`, and `bss` changes when meaningful.
- [ ] Run host tests for platform-independent code.
- [ ] Confirm no new unbounded wait was added to a real-time path.
- [ ] Confirm no unbounded buffer or runtime allocation was added.
- [ ] Confirm data has explicit units, frame, timestamp, and validity where needed.
- [ ] Assess actuator, power-distribution, QSPI, watchdog, and boot impact.
- [ ] Update `CHANGELOG.md` for user-visible behavior.
- [ ] Update documentation when interfaces, procedures, or risks change.

## Safety Rules

Changes that can operate an actuator, switch power, erase/write QSPI, program the
FPGA, or alter a boot image require explicit review and a separate hardware test
plan. Tests must begin with hazardous loads disconnected and an immediate physical
power-removal method available.

Never weaken a safety guard merely to make a test convenient. Add a deliberate,
auditable test mode with clear entry and timeout behavior instead.

## Pull Requests

A pull request should state:

1. the problem and intended outcome;
2. what is deliberately out of scope;
3. affected modules and interfaces;
4. build and test evidence;
5. timing, memory, actuator, power, flash, and boot impact;
6. known limitations and unverified assumptions;
7. a safe rollback method.

## Versioning

The canonical version is in `VERSION`. Releases use Semantic Versioning and Git
tags in the form `vMAJOR.MINOR.PATCH[-PRERELEASE]`. Do not change the version for
every commit. Update it as part of a deliberate release change together with the
changelog and validation record.
