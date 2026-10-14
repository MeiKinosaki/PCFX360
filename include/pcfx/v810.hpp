#pragma once
#include <array>
#include <cstdint>

namespace pcfx {
// Architectural state only; instruction execution is not implemented yet.
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
};
} // namespace pcfx
