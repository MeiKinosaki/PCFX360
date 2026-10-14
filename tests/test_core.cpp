#include "pcfx/memory.hpp"
#include "pcfx/v810.hpp"
#include <cassert>
#include <cstdint>
#include <limits>

int main() {
    pcfx::Memory memory(16U);
    assert(memory.size() == 16U);
    assert(memory.write32(0U, 0x12345678U));
    std::uint16_t value16 = 0U;
    std::uint32_t value32 = 0U;
    assert(memory.read16(0U, value16) && value16 == 0x5678U);
    assert(memory.read32(0U, value32) && value32 == 0x12345678U);
    std::uint8_t value8 = 0U;
    assert(!memory.read8(16U, value8));
    assert(!memory.read32(std::numeric_limits<std::uint32_t>::max(), value32));
    assert(!memory.write32(13U, value32));

    pcfx::V810State cpu;
    cpu.gpr[1] = 123U;
    cpu.reset(0x100U);
    assert(cpu.pc == 0x100U);
    assert(cpu.gpr[1] == 0U);
    return 0;
}
