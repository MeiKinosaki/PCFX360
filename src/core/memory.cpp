#include "pcfx/memory.hpp"

namespace pcfx {
Memory::Memory(std::size_t size) : ram_(size, 0U) {}
std::size_t Memory::size() const noexcept { return ram_.size(); }

bool Memory::contains(std::uint32_t address, std::size_t count) const noexcept {
    const auto start = static_cast<std::size_t>(address);
    return start <= ram_.size() && count <= ram_.size() - start;
}

bool Memory::read8(std::uint32_t address, std::uint8_t& value) const noexcept {
    if (!contains(address, 1U)) return false;
    value = ram_[static_cast<std::size_t>(address)];
    return true;
}
bool Memory::read16(std::uint32_t address, std::uint16_t& value) const noexcept {
    if (!contains(address, 2U)) return false;
    const auto i = static_cast<std::size_t>(address);
    value = static_cast<std::uint16_t>(ram_[i] | (static_cast<std::uint16_t>(ram_[i + 1U]) << 8U));
    return true;
}
bool Memory::read32(std::uint32_t address, std::uint32_t& value) const noexcept {
    if (!contains(address, 4U)) return false;
    const auto i = static_cast<std::size_t>(address);
    value = static_cast<std::uint32_t>(ram_[i]) |
            (static_cast<std::uint32_t>(ram_[i + 1U]) << 8U) |
            (static_cast<std::uint32_t>(ram_[i + 2U]) << 16U) |
            (static_cast<std::uint32_t>(ram_[i + 3U]) << 24U);
    return true;
}
bool Memory::write8(std::uint32_t address, std::uint8_t value) noexcept {
    if (!contains(address, 1U)) return false;
    ram_[static_cast<std::size_t>(address)] = value;
    return true;
}
bool Memory::write16(std::uint32_t address, std::uint16_t value) noexcept {
    if (!contains(address, 2U)) return false;
    const auto i = static_cast<std::size_t>(address);
    ram_[i] = static_cast<std::uint8_t>(value & 0xffU);
    ram_[i + 1U] = static_cast<std::uint8_t>(value >> 8U);
    return true;
}
bool Memory::write32(std::uint32_t address, std::uint32_t value) noexcept {
    if (!contains(address, 4U)) return false;
    const auto i = static_cast<std::size_t>(address);
    for (std::size_t byte = 0; byte < 4U; ++byte)
        ram_[i + byte] = static_cast<std::uint8_t>((value >> (byte * 8U)) & 0xffU);
    return true;
}
} // namespace pcfx
