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
    assert((cpu.psw & 0x0fU) == 0U);
    cpu.gpr[1] = 10U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[3] == 12U && cpu.pc == 0x1018U);
    assert(cpu.psw == 0U);
    cpu.gpr[1] = 0x100U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[4] == 0x1f0U && cpu.pc == 0x101cU);
    cpu.gpr[1] = 0x1234U;
    assert(cpu.step(bus) == pcfx::V810StepResult::Executed);
    assert(cpu.gpr[5] == 0x00011234U && cpu.pc == 0x1020U);

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
