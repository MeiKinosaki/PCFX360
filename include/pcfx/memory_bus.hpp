#pragma once
#include "pcfx/memory.hpp"
#include <cstdint>

namespace pcfx {

enum class BusRegion : std::uint8_t { Unmapped, Ram };

// Minimal bus abstraction. The RAM window is a test window, not a verified
// representation of the real PC-FX hardware address map.
class MemoryBus {
public:
    explicit MemoryBus(Memory& ram, std::uint32_t ram_base = 0U) noexcept;
    BusRegion region(std::uint32_t address, std::uint32_t width = 1U) const noexcept;
    bool read8(std::uint32_t address, std::uint8_t& value) const noexcept;
    bool read16(std::uint32_t address, std::uint16_t& value) const noexcept;
    bool read32(std::uint32_t address, std::uint32_t& value) const noexcept;
    bool write8(std::uint32_t address, std::uint8_t value) noexcept;
    bool write16(std::uint32_t address, std::uint16_t value) noexcept;
    bool write32(std::uint32_t address, std::uint32_t value) noexcept;
private:
    bool translate(std::uint32_t address, std::uint32_t width,
                   std::uint32_t& offset) const noexcept;
    Memory& ram_;
    std::uint32_t ram_base_;
};

} // namespace pcfx
