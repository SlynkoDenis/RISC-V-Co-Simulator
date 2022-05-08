#ifndef HW_CO_SIMULATION_MEMORY_H
#define HW_CO_SIMULATION_MEMORY_H

#include <array>
#include "macros.h"
#include <vector>


namespace functional {
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

    void Dump() const {
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
