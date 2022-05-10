#ifndef HW_CO_SIMULATION_INSTR_MEM_UNIT_H
#define HW_CO_SIMULATION_INSTR_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <vector>
#include "common.h"
#include "macros.h"
#include "mmu_fixed_offset.h"


namespace modules {
class InstrMemUnit {
public:
    InstrMemUnit(): address(0), mmu_(nullptr) {}
    explicit InstrMemUnit(memory::MMUFixedOffset *mmu): address(0), mmu_(mmu) {}
    NO_COPY_SEMANTIC(InstrMemUnit);
    DEFAULT_MOVE_SEMANTIC(InstrMemUnit);
    virtual ~InstrMemUnit() noexcept = default;

    [[nodiscard]] uint32_t GetData() {
        return mmu_->GetWord(address);
    }

#ifdef DEBUG
    virtual void Debug() {
        std::cout << "InstrMemUnit: read_data=" << GetData() << "; address=" << address << std::endl;
    }
#endif

    uint32_t address;

private:
    memory::MMUFixedOffset *mmu_;
};
}   // end namespace modules

#endif // HW_CO_SIMULATION_INSTR_MEM_UNIT_H
