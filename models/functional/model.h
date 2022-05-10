#ifndef HW_CO_SIMULATION_MODEL_H
#define HW_CO_SIMULATION_MODEL_H

#include "decoder.h"
#include "interpreter.h"
#include <memory>
#include "mmu.h"
#include "mmu_fixed_offset.h"
#include "regfile.h"


namespace functional {
class ECALLException : public std::exception {
};

class EBREAKException : public std::exception {
};


#define HANDLER(name) \
    bool Handle##name(RV32IInstruction &instr) override

class FunctionalModel : public RV32IInterpreter {
public:
    FunctionalModel(): RV32IInterpreter(), ticks_counter(0), pc(0), cur_instr(nullptr), readable_traces_(false) {}
    DEFAULT_COPY_SEMANTIC(FunctionalModel);
    DEFAULT_MOVE_SEMANTIC(FunctionalModel);
    ~FunctionalModel() noexcept override = default;

    void SetPC(uint32_t new_pc) {
        pc = new_pc;
    }
    void InitSP() {
        auto sp = mmu.AllocateStack();
        registers.WriteReg(sp, SPReg);
    }

    void DumpState() const override;

    runtime::ReturnCodes Run();
    runtime::ReturnCodes RunProgram(const char *path) override;

    const memory::MMUFixedOffset &GetMMU() const {
        return mmu;
    }
    const RV32IRegFile &GetRegFile() const {
        return registers;
    }

    void MakeTracesReadable(bool readable) { readable_traces_ = readable; }

    static constexpr uint8_t SPReg = 2;

protected:
    HANDLER(LUI);
    HANDLER(AUIPC);
    HANDLER(JAL);
    HANDLER(JALR);
    HANDLER(BEQ);
    HANDLER(BNE);
    HANDLER(BLT);
    HANDLER(BGE);
    HANDLER(BLTU);
    HANDLER(BGEU);
    HANDLER(LB);
    HANDLER(LH);
    HANDLER(LW);
    HANDLER(LBU);
    HANDLER(LHU);
    HANDLER(SB);
    HANDLER(SH);
    HANDLER(SW);
    HANDLER(ADDI);
    HANDLER(SLTI);
    HANDLER(SLTIU);
    HANDLER(XORI);
    HANDLER(ORI);
    HANDLER(ANDI);
    HANDLER(SLLI);
    HANDLER(SRLI);
    HANDLER(SRAI);
    HANDLER(ADD);
    HANDLER(SUB);
    HANDLER(SLL);
    HANDLER(SLT);
    HANDLER(SLTU);
    HANDLER(XOR);
    HANDLER(SRL);
    HANDLER(SRA);
    HANDLER(OR);
    HANDLER(AND);
    bool HandleFENCE([[maybe_unused]] RV32IInstruction &instr) override;
    bool HandleECALL([[maybe_unused]] RV32IInstruction &instr) override;
    bool HandleEBREAK([[maybe_unused]] RV32IInstruction &instr) override;

private:
    uint32_t LoadFromPC() const {
        ASSERT(pc % 4 == 0);
        return mmu.GetWord(pc);
    }

    void TraceExecutedInstruction(uint32_t instr, uint32_t location);

    size_t ticks_counter;
    memory::MMUFixedOffset mmu;
    RV32IRegFile registers;
    uint32_t pc;
    std::unique_ptr<RV32IInstruction> cur_instr;
    bool readable_traces_;
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_MODEL_H
