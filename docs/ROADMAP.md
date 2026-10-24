# PCFX360 Roadmap

## v0.1b — initial scaffold
- Project concept and preliminary memory/CPU-state scaffolding.

## v0.2b — host build foundation
- [x] CMake project and host test target.
- [x] Bounded little-endian RAM access API.
- [x] Explicit V810 architectural-state structure.
- [x] CI workflow for host compilation and tests.
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

## v0.5b — initial Format V immediate instructions
- [x] Decode 32-bit Format V instructions and fetch the second halfword safely.
- [x] Implement ADDI, MOVEA, ORI and MOVHI in the current experimental core.
- [x] Add tests for signed and zero-extended immediates and PC advancement.
- [x] Keep PC unchanged on failed fetches and unsupported opcodes.
- [x] Add a signed-overflow boundary test for ADDI and correct carry expectations for negative immediates.
- [ ] Compare behavior against an independent V810 test suite.

## v0.6b — initial Format III conditional branches
- [x] Decode Format III branch instructions and sign-extend the displacement.
- [x] Implement the 16 branch condition codes, including unconditional BR and NOP.
- [x] Add tests for taken BE, not-taken BNE, backward BR and NOP flag preservation.
- [ ] Compare branch conditions and PC-relative base against an independent V810 test suite.
- [ ] Implement memory load/store instructions in the next sequential milestone.

## v0.7b — initial memory load/store instructions
- [x] Implement Format V LD.B, LD.H and LD.W reads using a signed 16-bit displacement.
- [x] Implement Format V ST.B, ST.H and ST.W writes using a signed 16-bit displacement.
- [x] Add tests for byte/halfword/word stores and signed byte/halfword loads.
- [ ] Verify opcode semantics, alignment behavior and exception handling against an independent V810 reference.

## v0.8b — expanded logical instruction subset
- [x] Add Format I SUB, CMP, AND, XOR and NOT operations.
- [x] Add Format V ANDI and XORI immediate operations.
- [x] Update Z/S flags and clear OV for these logical operations in the current model.
- [x] Add basic instruction/result/PC tests for the new operations, including CMP preserving its destination register and ANDI/XORI immediate results.
- [ ] Verify exact opcode and PSW semantics against a trusted V810 reference.

## v0.9b — register shift operations
- [x] Implement SHL, SHR and SAR using the low five bits of the shift-count register.
- [x] Update Z/S/CY and clear OV for nonzero shifts in the experimental PSW model.
- [x] Use portable sign-fill logic for arithmetic right shift.
- [x] Add basic result, PC and flag tests for the three shift operations.
- [ ] Verify exact opcode, shift-count and PSW behavior against an independent V810 reference.

## v1.0b — CPU boundary and invariant tests
- [x] Add regression coverage that register r0 remains zero after a write-target instruction.
- [x] Add tests for a truncated 32-bit instruction fetch and unmapped data read returning BusFault without advancing PC.
- [x] Add a zero-count shift regression test for the current experimental flag model.
- [ ] Run host build and CTest in GitHub Actions and resolve any failures before treating this milestone as validated.
- [ ] Cross-check instruction encodings, PSW bits, alignment and exception semantics against an independent V810 reference.
- [ ] Do not mark Xbox 360 support or default.xex complete until a real toolchain build is confirmed.

## Later milestones
- Additional V810 arithmetic/logical instruction formats and complete exception handling.
- PC-FX memory map and MMIO devices.
- V810 interrupts, timers and DMA.
- Video controllers and sound hardware.
- CD-ROM interface, input and BIOS loading from user-provided files.
- Xbox 360 platform layer, deployment packaging and `default.xex`.
- Compatibility and performance testing on actual hardware.

A checked item means code exists; it does not imply game compatibility. The V810 subset is experimental and requires comparison against an independent test suite. No Xbox 360 binary or hardware test is claimed until the toolchain build and runtime are verified.


## v1.0b — validação paralela adicional
- [x] Expandir CI de host para GCC e Clang.
- [x] Adicionar job de sanitizers ASan/UBSan.
- [x] Aplicar avisos de compilador ao alvo de testes.
- [x] Testar limites do barramento em endereços próximos do limite de 32 bits.
- [ ] Aguardar resultados do CI; alterações de teste não equivalem a testes aprovados.
