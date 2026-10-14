#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace pcfx {
class Memory {
public:
    explicit Memory(std::size_t size = 2U * 1024U * 1024U);
    std::size_t size() const noexcept;
    bool read8(std::uint32_t address, std::uint8_t& value) const noexcept;
    bool read16(std::uint32_t address, std::uint16_t& value) const noexcept;
    bool read32(std::uint32_t address, std::uint32_t& value) const noexcept;
    bool write8(std::uint32_t address, std::uint8_t value) noexcept;
    bool write16(std::uint32_t address, std::uint16_t value) noexcept;
    bool write32(std::uint32_t address, std::uint32_t value) noexcept;
private:
    bool contains(std::uint32_t address, std::size_t count) const noexcept;
    std::vector<std::uint8_t> ram_;
};
} // namespace pcfx
