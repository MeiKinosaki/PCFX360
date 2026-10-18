#include "pcfx/memory.hpp"
#include "pcfx/memory_bus.hpp"
#include "pcfx/v810.hpp"
#include <cassert>
#include <cstdint>
#include <limits>

static std::uint16_t encode(std::uint16_t opcode, std::uint16_t reg2, std::uint16_t reg1) {
    return static_cast<std::uint16_t>((opcode << 10U) | (reg2 << 5U) | reg1);
}

int main() {
    pcfx::Memory memory(64U);
    assert(memory.size() == 64U);
    assert(memory.write32(0U, 0x12345678U));
    std::uint16_t value16 = 0U;
    std::uint32_t value32 = 0U;
    assert(memory.read16(0U, value16) && value16 == 0x5678U);
    assert(memory.read32(0U, value32) && value32 == 0x12345678U);
    std::uint8_t value8 = 0U;
    assert(!memory.read8(64U, value8));
    assert(!memory.read32(std::numeric_limits<std::uint32_t>::max(), value32));
    assert(!memory.write32(61U, value32));

    pcfx::MemoryBus bus(memory, 0x1000U);
    assert(bus.region(0x1000U) == pcfx::BusRegion::Ram);
    assert(bus.region(0x103fU) == pcfx::BusRegion::Ram);
    assert(bus.region(0x1040U) == pcfx::BusRegion::Unmapped);
    assert(bus.region(0x103fU, 2U) == pcfx::BusRegion::Unmapped);
    assert(bus.region(0x0fffU) == pcfx::BusRegion::Unmapped);
    assert(bus.write16(0x1004U, 0xbeefU));
    assert(bus.read16(0x1004U, value16) && value16 == 0xbeefU);
    assert(!bus.read8(0x1040U, value8));
    assert(!bus.write32(0x103eU, 0U));

    // Format I: ADD r1,r2; MOV r2,r3; OR r1,r3.
    assert(bus.write16(0x1000U, encode(0x01U, 2U, 1U)));
    assert(bus.write16(0x1002U, encode(0x00U, 3U, 2U)));
    assert(bus.write16(0x1004U, encode(0x0cU, 3U, 1U)));
    pcfx::V810State cpu;
    cpu.reset(0x1000U);
    cpu.gpr[1] = 7U;
    cpu.gpr[2] = 5U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[2] == 12U && cpu.pc == 0x1002U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[3] == 12U && cpu.pc == 0x1004U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[3] == 15U && cpu.pc == 0x1006U);
    assert(cpu.gpr[0] == 0U);

    // MOV preserves PSW; OR updates Z/S and clears OV while preserving CY.
    assert(bus.write16(0x1008U, encode(0x00U, 4U, 1U)));
    assert(bus.write16(0x100aU, encode(0x0cU, 4U, 1U)));
    cpu.reset(0x1008U);
    cpu.gpr[1] = 0x80000000U;
    cpu.psw = 0x0fU;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[4] == 0x80000000U && cpu.psw == 0x0fU);
    cpu.gpr[4] = 0U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[4] == 0x80000000U);
    assert((cpu.psw & 0x0fU) == 0x0aU);

    // Format V: ADDI, MOVEA, ORI, MOVHI (second halfword is imm16).
    assert(bus.write16(0x1010U, encode(0x29U, 2U, 1U)));
    assert(bus.write16(0x1012U, 0xffffU)); // -1
    assert(bus.write16(0x1014U, encode(0x28U, 3U, 1U)));
    assert(bus.write16(0x1016U, 2U));
    assert(bus.write16(0x1018U, encode(0x2cU, 4U, 1U)));
    assert(bus.write16(0x101aU, 0x00f0U));
    assert(bus.write16(0x101cU, encode(0x2fU, 5U, 1U)));
    assert(bus.write16(0x101eU, 1U));
    cpu.reset(0x1010U);
    cpu.gpr[1] = 10U;
    cpu.gpr[2] = 100U;
    cpu.psw = 0x08U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[2] == 9U && cpu.pc == 0x1014U);
    assert((cpu.psw & 0x0fU) == 0x08U);
    cpu.gpr[1] = 10U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[3] == 12U && cpu.pc == 0x1018U);
    assert(cpu.psw == 0x08U);
    cpu.gpr[1] = 0x100U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[4] == 0x1f0U && cpu.pc == 0x101cU);
    cpu.gpr[1] = 0x1234U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[5] == 0x00011234U && cpu.pc == 0x1020U);
    assert(cpu.psw == 0x08U); // MOVHI also preserves flags.

    // Signed overflow: 0x7fffffff + 1 -> 0x80000000, OV and S set.
    assert(bus.write16(0x1020U, encode(0x29U, 2U, 1U)));
    assert(bus.write16(0x1022U, 1U));
    cpu.reset(0x1020U);
    cpu.gpr[1] = 0x7fffffffU;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[2] == 0x80000000U);
    assert((cpu.psw & 0x0fU) == 0x06U);

    // Format V memory operations: signed LD.B/LD.H, LD.W and ST.B/ST.H/ST.W.
    const auto writeMemoryOp = [&](std::uint32_t address, std::uint16_t opcode,
                                   std::uint16_t target, std::uint16_t base,
                                   std::uint16_t displacement) {
        assert(bus.write16(address, encode(opcode, target, base)));
        assert(bus.write16(address + 2U, displacement));
    };
    writeMemoryOp(0x1008U, 0x34U, 2U, 1U, 0U); // ST.B r2, disp16[r1]
    writeMemoryOp(0x100cU, 0x35U, 2U, 1U, 2U); // ST.H r2, disp16[r1]
    writeMemoryOp(0x1010U, 0x37U, 2U, 1U, 4U); // ST.W r2, disp16[r1]
    writeMemoryOp(0x1014U, 0x30U, 3U, 1U, 0U); // LD.B r3, disp16[r1]
    writeMemoryOp(0x1018U, 0x31U, 4U, 1U, 2U); // LD.H r4, disp16[r1]
    writeMemoryOp(0x101cU, 0x33U, 5U, 1U, 4U); // LD.W r5, disp16[r1]
    cpu.reset(0x1008U);
    cpu.gpr[1] = 0x1038U;
    cpu.gpr[2] = 0x12345680U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.pc == 0x100cU);
    cpu.gpr[2] = 0x00008001U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    cpu.gpr[2] = 0x12345678U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(bus.read8(0x1038U, value8) && value8 == 0x80U);
    assert(bus.read16(0x103aU, value16) && value16 == 0x8001U);
    assert(bus.read32(0x103cU, value32) && value32 == 0x12345678U);
    cpu.reset(0x1014U);
    cpu.gpr[1] = 0x1038U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[3] == 0xffffff80U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[4] == 0xffff8001U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[5] == 0x12345678U);

    // Additional logical instructions: AND, XOR and NOT.
    assert(bus.write16(0x1030U, encode(0x0dU, 6U, 1U)));
    assert(bus.write16(0x1032U, encode(0x0eU, 7U, 1U)));
    assert(bus.write16(0x1034U, encode(0x0fU, 8U, 1U)));
    cpu.reset(0x1030U);
    cpu.gpr[1] = 0x0ff00ff0U;
    cpu.gpr[6] = 0x00ffff00U;
    cpu.gpr[7] = 0xffffffffU;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[6] == 0x00f00f00U && cpu.pc == 0x1032U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[7] == 0xf00ff00fU && cpu.pc == 0x1034U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[8] == 0xf00ff00fU && cpu.pc == 0x1036U);

    // Format III conditional branches: BE, BNE, BR backward, and NOP.
    const auto encodeBranch = [](std::uint16_t condition, std::int16_t displacement) {
        const auto masked = static_cast<std::uint16_t>(displacement) & 0x01feU;
        return static_cast<std::uint16_t>(0x8000U | (condition << 9U) | masked);
    };
    assert(bus.write16(0x1024U, encodeBranch(0x2U, 4)));   // BE +4
    assert(bus.write16(0x1026U, encodeBranch(0xaU, 4)));   // BNE +4
    assert(bus.write16(0x1028U, encodeBranch(0x5U, -4)));  // BR -4
    assert(bus.write16(0x102aU, encodeBranch(0xdU, 0)));   // NOP
    cpu.reset(0x1024U);
    cpu.psw = 0x01U; // Z=1: BE taken
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.pc == 0x1028U);
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed); // BR -4
    assert(cpu.pc == 0x1024U);
    cpu.reset(0x1026U);
    cpu.psw = 0x01U; // Z=1: BNE not taken
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.pc == 0x1028U);
    cpu.reset(0x102aU);
    cpu.psw = 0x0fU;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.pc == 0x102cU && cpu.psw == 0x0fU);

    // Failed fetch and unsupported opcode do not leave PC advanced.
    cpu.reset(0x1040U);
    assert(cpu.step(bus) == pcfx::V810StepResult::BusFault);
    assert(cpu.pc == 0x1040U);
    assert(bus.write16(0x1000U, encode(0x3fU, 0U, 0U)));
    cpu.reset(0x1000U);
    assert(cpu.step(bus) == pcfx::V810StepResult::IllegalInstruction);
    assert(cpu.pc == 0x1000U);
    return 0;
}
