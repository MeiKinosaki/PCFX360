# PCFX360 Roadmap

## v0.1b — initial scaffold
- Project concept and preliminary memory/CPU-state scaffolding.

## v0.2b — host build foundation
- [x] CMake project and host test target.
- [x] Bounded little-endian RAM access API.
- [x] Explicit V810 architectural-state structure.
- [x] CI workflow for host compilation and tests.
- [ ] V810 instruction interpreter — not implemented.
- [ ] Verify CPU reset behavior and system-register definitions from primary documentation.

## Next sequential milestone: v0.3b
1. Add a memory-bus abstraction and address-map tests.
2. Implement only V810 instructions whose encoding and semantics are verified against reliable technical documentation.
3. Add instruction-level tests before claiming CPU execution support.
4. Keep host CI separate from Xbox 360 SDK/toolchain builds.

## Later milestones
- PC-FX memory map and MMIO devices.
- V810 interrupts, timers and DMA.
- Video controllers and sound hardware.
- CD-ROM interface, input and BIOS loading from user-provided files.
- Xbox 360 platform layer, deployment packaging and `default.xex`.
- Compatibility and performance testing on actual hardware.

A checked item means code exists; it does not imply game compatibility. No Xbox 360 binary or hardware test is claimed until the toolchain build and runtime are verified.
