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

## v0.3b — initial memory bus
- [x] Add a bus layer with a configurable RAM base address.
- [x] Reject unmapped and cross-boundary accesses without wrapping addresses.
- [x] Add tests for region selection, translation and boundary failures.
- [ ] Replace the test RAM window with a verified PC-FX address map.

## v0.4b — first V810 instruction execution slice
- [x] Fetch 16-bit instruction words through the memory bus.
- [x] Decode Format I register-register MOV, ADD and OR.
- [x] Keep r0 hardwired to zero and advance PC for supported instructions.
- [x] Add basic Z/S/OV/CY flag handling for this subset and instruction tests.
- [x] Report bus faults and unsupported opcodes explicitly.
- [ ] Implement more instruction formats, precise exception behavior and full PSW semantics.
- [ ] Verify behavior against an independent V810 test suite or trusted emulator.

## Next sequential milestone: v0.5b
1. Add a verified subset of immediate arithmetic/logical instructions.
2. Expand flags and test signed overflow/carry boundaries.
3. Add load/store only after memory access widths and alignment behavior are confirmed.
4. Keep host CI separate from Xbox 360 SDK/toolchain builds.

## Later milestones
- PC-FX memory map and MMIO devices.
- V810 interrupts, timers and DMA.
- Video controllers and sound hardware.
- CD-ROM interface, input and BIOS loading from user-provided files.
- Xbox 360 platform layer, deployment packaging and `default.xex`.
- Compatibility and performance testing on actual hardware.

A checked item means code exists; it does not imply game compatibility. No Xbox 360 binary or hardware test is claimed until the toolchain build and runtime are verified.
