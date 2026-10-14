#pragma once
#include <array>
#include <cstdint>

namespace pcfx {
class MemoryBus;

enum class V810StepResult : std::uint8_t {
    Executed,
    Halted,
    BusFault,
    IllegalInstruction
};

struct V810State {
    std::array<std::uint32_t, 32> gpr{};
    std::uint32_t pc = 0;
    std::uint32_t psw = 0;
    std::array<std::uint32_t, 32> system_registers{};

    void reset(std::uint32_t reset_vector) noexcept {
        gpr.fill(0U);
        pc = reset_vector;
        psw = 0U;
        system_registers.fill(0U);
    }

    V810StepResult step(MemoryBus& bus) noexcept;
};
} // namespace pcfx
