#ifndef HW_CO_SIMULATION_MEMORY_H
#define HW_CO_SIMULATION_MEMORY_H

#include <array>
#include "macros.h"
#include <vector>


namespace functional {
// TODO: implement fully-functional MMU
class MemoryUnit {
public:
    MemoryUnit() = default;
    DEFAULT_COPY_SEMANTIC(MemoryUnit);
    DEFAULT_MOVE_SEMANTIC(MemoryUnit);
    DEFAULT_DTOR(MemoryUnit);

    // TODO: memory address computations done by the hardware ignore overflow and instead wrap around
    uint32_t GetWord(size_t addr) const {
        return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }
    uint32_t &GetWord(size_t addr) {
        return *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }
    uint16_t GetHalf(size_t addr) const {
        return *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }
    uint16_t &GetHalf(size_t addr) {
        return *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }
    uint8_t GetByte(size_t addr) const {
        return *(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }
    uint8_t &GetByte(size_t addr) {
        return *(reinterpret_cast<uint8_t*>(memory_.data()) + addr);
    }

    void SetWord(uint32_t value, size_t addr) {
        *reinterpret_cast<uint32_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr) = value;
    }
    void SetHalf(uint16_t value, size_t addr) {
        *reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(memory_.data()) + addr) = value;
    }
    void SetByte(uint8_t value, size_t addr) {
        *(reinterpret_cast<uint8_t*>(memory_.data()) + addr) = value;
    }
    void SetRange(const std::vector<uint32_t>& values, size_t start_idx) {
        std::move(values.begin(), values.end(), memory_.begin() + start_idx);
    }

    static constexpr size_t memory_size = (1 << 16);

private:
    mutable std::array<uint32_t, memory_size> memory_{0};
};


class RegFile {
public:
    RegFile() = default;
    DEFAULT_COPY_SEMANTIC(RegFile);
    DEFAULT_MOVE_SEMANTIC(RegFile);
    DEFAULT_DTOR(RegFile);

    void WriteReg(uint32_t value, size_t idx) {
        regs_.at(idx) = value;
        regs_.at(0) = 0;
    }
    uint32_t ReadReg(size_t idx) const { return regs_.at(idx); }

    void dump() const {
        std::cout << "RegFile:\n";
        for (size_t i = 0; i < number_of_registers; ++i) {
            std::cout << 'r' << i << ":\t" << std::hex << regs_[i] << " (" << std::dec << static_cast<int32_t>(regs_[i]) << ")\n";
        }
    }

    static constexpr size_t number_of_registers = 32;

private:
    std::array<uint32_t, number_of_registers> regs_{0};
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_MEMORY_H
