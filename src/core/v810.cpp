#include "pcfx/v810.hpp"
#include "pcfx/memory_bus.hpp"

#include <cstdint>

namespace pcfx {
namespace {
constexpr std::uint32_t FlagZ = 1U << 0U;
constexpr std::uint32_t FlagS = 1U << 1U;
constexpr std::uint32_t FlagOV = 1U << 2U;
constexpr std::uint32_t FlagCY = 1U << 3U;
constexpr std::uint32_t ArithmeticFlags = FlagZ | FlagS | FlagOV | FlagCY;
constexpr std::uint32_t SignBit = 0x80000000U;

void updateSz(V810State& cpu, std::uint32_t value) noexcept {
    cpu.psw &= ~(FlagZ | FlagS);
    if (value == 0U) cpu.psw |= FlagZ;
    if ((value & SignBit) != 0U) cpu.psw |= FlagS;
}
} // namespace

V810StepResult V810State::step(MemoryBus& bus) noexcept {
    std::uint16_t instruction = 0U;
    if (!bus.read16(pc, instruction)) return V810StepResult::BusFault;

    const std::uint32_t opcode = static_cast<std::uint32_t>(instruction >> 10U);
    const std::uint32_t reg2 = static_cast<std::uint32_t>((instruction >> 5U) & 0x1fU);
    const std::uint32_t reg1 = static_cast<std::uint32_t>(instruction & 0x1fU);
    pc += 2U;

    // V810 Format I: opcode[15:10], reg2[9:5], reg1[4:0].
    // Only the verified initial integer subset is enabled here.
    if (opcode == 0x00U) { // MOV reg1, reg2
        const std::uint32_t value = gpr[reg1];
        if (reg2 != 0U) gpr[reg2] = value;
        // MOV does not modify arithmetic flags on the V810.
    } else if (opcode == 0x01U) { // ADD reg1, reg2
        const std::uint32_t lhs = gpr[reg2];
        const std::uint32_t rhs = gpr[reg1];
        const std::uint64_t wide = static_cast<std::uint64_t>(lhs) + rhs;
        const std::uint32_t value = static_cast<std::uint32_t>(wide);
        if (reg2 != 0U) gpr[reg2] = value;
        psw &= ~ArithmeticFlags;
        if (wide > 0xffffffffULL) psw |= FlagCY;
        if (((~(lhs ^ rhs) & (lhs ^ value)) & SignBit) != 0U) psw |= FlagOV;
        updateSz(*this, value);
    } else if (opcode == 0x0cU) { // OR reg1, reg2
        const std::uint32_t value = gpr[reg2] | gpr[reg1];
        if (reg2 != 0U) gpr[reg2] = value;
        updateSz(*this, value);
        psw &= ~FlagOV;
    } else {
        return V810StepResult::IllegalInstruction;
    }

    gpr[0] = 0U;
    return V810StepResult::Executed;
}
} // namespace pcfx
