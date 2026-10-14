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
std::uint32_t signExtend16(std::uint16_t value) noexcept {
    return static_cast<std::uint32_t>(static_cast<std::int32_t>(static_cast<std::int16_t>(value)));
}
void setAddFlags(V810State& cpu, std::uint32_t lhs, std::uint32_t rhs,
                 std::uint32_t result, std::uint64_t wide) noexcept {
    cpu.psw &= ~ArithmeticFlags;
    if (wide > 0xffffffffULL) cpu.psw |= FlagCY;
    if (((~(lhs ^ rhs) & (lhs ^ result)) & SignBit) != 0U) cpu.psw |= FlagOV;
    updateSz(cpu, result);
}
} // namespace

V810StepResult V810State::step(MemoryBus& bus) noexcept {
    const std::uint32_t instruction_pc = pc;
    std::uint16_t instruction = 0U;
    if (!bus.read16(instruction_pc, instruction)) return V810StepResult::BusFault;

    const std::uint32_t opcode = static_cast<std::uint32_t>(instruction >> 10U);
    const std::uint32_t reg2 = static_cast<std::uint32_t>((instruction >> 5U) & 0x1fU);
    const std::uint32_t reg1 = static_cast<std::uint32_t>(instruction & 0x1fU);

    // Format III: conditional branch, 9-bit signed displacement with bit 0 fixed to zero.
    // The PC-relative base is the address of the branch instruction itself.
    if ((instruction >> 13U) == 0x4U) {
        const std::uint32_t condition = (instruction >> 9U) & 0x0fU;
        const bool z = (psw & FlagZ) != 0U;
        const bool s = (psw & FlagS) != 0U;
        const bool ov = (psw & FlagOV) != 0U;
        const bool cy = (psw & FlagCY) != 0U;
        bool take = false;
        switch (condition) {
        case 0x0U: take = ov; break;                 // BV
        case 0x1U: take = cy; break;                 // BL/BC
        case 0x2U: take = z; break;                  // BE/BZ
        case 0x3U: take = cy || z; break;             // BNH
        case 0x4U: take = s; break;                  // BN
        case 0x5U: take = true; break;               // BR
        case 0x6U: take = s != ov; break;             // BLT
        case 0x7U: take = (s != ov) || z; break;     // BLE
        case 0x8U: take = !ov; break;                // BNV
        case 0x9U: take = !cy; break;                // BNL/ BNC
        case 0x0aU: take = !z; break;                // BNE/BNZ
        case 0x0bU: take = !cy && !z; break;         // BH
        case 0x0cU: take = !s; break;                // BP
        case 0x0dU: take = false; break;             // NOP
        case 0x0eU: take = s == ov; break;           // BGE
        case 0x0fU: take = (s == ov) && !z; break;   // BGT
        default: break;
        }
        std::int32_t displacement = static_cast<std::int32_t>(instruction & 0x01feU);
        if ((displacement & 0x0100) != 0) displacement -= 0x0200;
        pc = take ? instruction_pc + static_cast<std::uint32_t>(displacement)
                  : instruction_pc + 2U;
        gpr[0] = 0U;
        return V810StepResult::Executed;
    }

    // Format V memory operations: disp16[reg1] is the effective address.
    if (opcode == 0x30U || opcode == 0x31U || opcode == 0x33U ||
        opcode == 0x34U || opcode == 0x35U || opcode == 0x37U) {
        std::uint16_t immediate = 0U;
        if (instruction_pc > 0xfffffffdU ||
            !bus.read16(instruction_pc + 2U, immediate)) return V810StepResult::BusFault;
        const std::uint32_t address = gpr[reg1] + signExtend16(immediate);
        if (opcode == 0x30U) { // LD.B: sign-extend byte
            std::uint8_t value = 0U;
            if (!bus.read8(address, value)) return V810StepResult::BusFault;
            const std::uint32_t result = static_cast<std::uint32_t>(
                static_cast<std::int32_t>(static_cast<std::int8_t>(value)));
            if (reg2 != 0U) gpr[reg2] = result;
        } else if (opcode == 0x31U) { // LD.H: sign-extend halfword
            std::uint16_t value = 0U;
            if (!bus.read16(address, value)) return V810StepResult::BusFault;
            const std::uint32_t result = signExtend16(value);
            if (reg2 != 0U) gpr[reg2] = result;
        } else if (opcode == 0x33U) { // LD.W
            std::uint32_t value = 0U;
            if (!bus.read32(address, value)) return V810StepResult::BusFault;
            if (reg2 != 0U) gpr[reg2] = value;
        } else if (opcode == 0x34U) { // ST.B
            if (!bus.write8(address, static_cast<std::uint8_t>(gpr[reg2])))
                return V810StepResult::BusFault;
        } else if (opcode == 0x35U) { // ST.H
            if (!bus.write16(address, static_cast<std::uint16_t>(gpr[reg2])))
                return V810StepResult::BusFault;
        } else { // ST.W
            if (!bus.write32(address, gpr[reg2])) return V810StepResult::BusFault;
        }
        pc = instruction_pc + 4U;
        gpr[0] = 0U;
        return V810StepResult::Executed;
    }

    // Format V is 32 bits: first halfword followed by imm16 in little-endian memory.
    if (opcode == 0x28U || opcode == 0x29U ||
        opcode == 0x2cU || opcode == 0x2fU) {
        std::uint16_t immediate = 0U;
        if (instruction_pc > 0xfffffffdU ||
            !bus.read16(instruction_pc + 2U, immediate)) return V810StepResult::BusFault;
        const std::uint32_t source = gpr[reg1];
        std::uint32_t result = 0U;

        if (opcode == 0x28U) { // MOVEA imm16, reg1, reg2; signed immediate, no flags
            result = source + signExtend16(immediate);
        } else if (opcode == 0x29U) { // ADDI imm16, reg1, reg2
            const std::uint32_t rhs = signExtend16(immediate);
            const std::uint64_t wide = static_cast<std::uint64_t>(source) + rhs;
            result = static_cast<std::uint32_t>(wide);
            if (reg2 != 0U) gpr[reg2] = result;
            setAddFlags(*this, source, rhs, result, wide);
            pc = instruction_pc + 4U;
            gpr[0] = 0U;
            return V810StepResult::Executed;
        } else if (opcode == 0x2cU) { // ORI imm16, reg1, reg2; zero-extended immediate
            result = source | static_cast<std::uint32_t>(immediate);
            if (reg2 != 0U) gpr[reg2] = result;
            updateSz(*this, result);
            psw &= ~FlagOV;
            pc = instruction_pc + 4U;
            gpr[0] = 0U;
            return V810StepResult::Executed;
        } else { // MOVHI imm16, reg1, reg2; add imm16 << 16, no flags
            result = source + (static_cast<std::uint32_t>(immediate) << 16U);
        }

        if (reg2 != 0U) gpr[reg2] = result;
        pc = instruction_pc + 4U;
        gpr[0] = 0U;
        return V810StepResult::Executed;
    }

    pc = instruction_pc + 2U;
    // V810 Format I: opcode[15:10], reg2[9:5], reg1[4:0].
    if (opcode == 0x00U) { // MOV reg1, reg2; flags unchanged
        if (reg2 != 0U) gpr[reg2] = gpr[reg1];
    } else if (opcode == 0x01U) { // ADD reg1, reg2
        const std::uint32_t lhs = gpr[reg2];
        const std::uint32_t rhs = gpr[reg1];
        const std::uint64_t wide = static_cast<std::uint64_t>(lhs) + rhs;
        const std::uint32_t result = static_cast<std::uint32_t>(wide);
        if (reg2 != 0U) gpr[reg2] = result;
        setAddFlags(*this, lhs, rhs, result, wide);
    } else if (opcode == 0x0cU) { // OR reg1, reg2
        const std::uint32_t result = gpr[reg2] | gpr[reg1];
        if (reg2 != 0U) gpr[reg2] = result;
        updateSz(*this, result);
        psw &= ~FlagOV;
    } else {
        pc = instruction_pc;
        return V810StepResult::IllegalInstruction;
    }

    gpr[0] = 0U;
    return V810StepResult::Executed;
}
} // namespace pcfx
