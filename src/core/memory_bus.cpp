#include "pcfx/memory_bus.hpp"

namespace pcfx {

MemoryBus::MemoryBus(Memory& ram, std::uint32_t ram_base) noexcept
    : ram_(ram), ram_base_(ram_base) {}

bool MemoryBus::translate(std::uint32_t address, std::uint32_t width,
                          std::uint32_t& offset) const noexcept {
    if (width == 0U || address < ram_base_) return false;
    // Reject accesses that would wrap the 32-bit physical address space.
    if (static_cast<std::uint64_t>(address) + width > 0x100000000ULL) return false;
    const std::uint32_t relative = address - ram_base_;
    if (static_cast<std::uint64_t>(relative) + width >
        static_cast<std::uint64_t>(ram_.size())) return false;
    offset = relative;
    return true;
}

BusRegion MemoryBus::region(std::uint32_t address, std::uint32_t width) const noexcept {
    std::uint32_t offset = 0U;
    return translate(address, width, offset) ? BusRegion::Ram : BusRegion::Unmapped;
}
bool MemoryBus::read8(std::uint32_t a, std::uint8_t& v) const noexcept {
    std::uint32_t o = 0U; return translate(a, 1U, o) && ram_.read8(o, v);
}
bool MemoryBus::read16(std::uint32_t a, std::uint16_t& v) const noexcept {
    std::uint32_t o = 0U; return translate(a, 2U, o) && ram_.read16(o, v);
}
bool MemoryBus::read32(std::uint32_t a, std::uint32_t& v) const noexcept {
    std::uint32_t o = 0U; return translate(a, 4U, o) && ram_.read32(o, v);
}
bool MemoryBus::write8(std::uint32_t a, std::uint8_t v) noexcept {
    std::uint32_t o = 0U; return translate(a, 1U, o) && ram_.write8(o, v);
}
bool MemoryBus::write16(std::uint32_t a, std::uint16_t v) noexcept {
    std::uint32_t o = 0U; return translate(a, 2U, o) && ram_.write16(o, v);
}
bool MemoryBus::write32(std::uint32_t a, std::uint32_t v) noexcept {
    std::uint32_t o = 0U; return translate(a, 4U, o) && ram_.write32(o, v);
}

} // namespace pcfx
