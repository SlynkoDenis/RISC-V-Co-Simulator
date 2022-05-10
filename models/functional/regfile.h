#ifndef HW_CO_SIMULATION_REGFILE_H
#define HW_CO_SIMULATION_REGFILE_H

#include <array>
#include "macros.h"
#include "trace_writer.h"


namespace functional {
class RV32IRegFile {
public:
    RV32IRegFile() = default;
    DEFAULT_COPY_SEMANTIC(RV32IRegFile);
    DEFAULT_MOVE_SEMANTIC(RV32IRegFile);
    ~RV32IRegFile() noexcept {};

    void WriteReg(uint32_t value, size_t idx) {
        if (idx != 0) {
            trace::TraceWriter::GetWriter().TraceIfEnabled(trace::TraceLevel::REG_FILE,
                                                           set_string_, idx, ' ', value, '\n');
        }

        regs_.at(idx) = value;
        regs_.at(0) = 0;
    }
    uint32_t ReadReg(size_t idx) const { return regs_.at(idx); }

    void Dump() const {
        std::cout << "RegFile:\n";
        for (size_t i = 0; i < number_of_registers; ++i) {
            std::cout << 'x' << i << ":\t" << std::hex << regs_[i] << " (" << std::dec << static_cast<int32_t>(regs_[i]) << ")\n";
        }
    }

    static constexpr size_t number_of_registers = 32;

private:
    std::array<uint32_t, number_of_registers> regs_{0};

    static constexpr const char *set_string_ = "set ";
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_REGFILE_H
