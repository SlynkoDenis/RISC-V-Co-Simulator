#ifndef HW_CO_SIMULATION_MODEL_H
#define HW_CO_SIMULATION_MODEL_H

#include "decoder.h"
#include "interpreter.h"
#include <memory>
#include "mmu.h"
#include "mmu_fixed_offset.h"
#include "regfile.h"


namespace functional {
class FunctionalModel : public RV32IInterpreter {
public:
    FunctionalModel(): RV32IInterpreter(), ticks_counter(0), pc(0), readable_traces_(false) {}
    DEFAULT_COPY_SEMANTIC(FunctionalModel);
    DEFAULT_MOVE_SEMANTIC(FunctionalModel);
    ~FunctionalModel() noexcept override = default;

    long long GetTicksCounter() const { return ticks_counter; }

    void SetPC(uint32_t new_pc) {
        pc = new_pc;
    }
    void InitSP() {
        auto sp = mmu.AllocateStack();
        registers.WriteReg(sp, SPReg);
    }

    void DumpState() const override;

    runtime::ReturnCodes RunProgram(const char *path) override;

    const memory::MMUFixedOffset &GetMMU() const {
        return mmu;
    }
    const RV32IRegFile &GetRegFile() const {
        return registers;
    }

    void MakeTracesReadable(bool readable) {
        readable_traces_ = readable;
        mmu.MakeTracesReadable(readable);
        registers.MakeTracesReadable(readable);
    }

    static constexpr uint8_t SPReg = 2;

protected:
    runtime::ReturnCodes Run() override;

private:
    uint32_t LoadFromPC() const {
        ASSERT(pc % 4 == 0);
        return mmu.GetWord(pc);
    }

    long long ticks_counter;
    memory::MMUFixedOffset mmu;
    RV32IRegFile registers;
    uint32_t pc;
    bool readable_traces_;
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_MODEL_H
