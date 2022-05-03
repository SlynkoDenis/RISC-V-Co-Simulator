#ifndef HW_CO_SIMULATION_MODEL_H
#define HW_CO_SIMULATION_MODEL_H

#include "decoder.h"
#include "interpreter.h"
#include <memory>
#include "memory.h"


namespace functional {
class ECALLException : public std::exception {
};

class EBREAKException : public std::exception {
};


#define HANDLER(name) \
    bool Handle##name(RV32IInstruction &instr) override

class FunctionalModel : public RV32IInterpreter {
public:
    FunctionalModel(): RV32IInterpreter(), ticks_counter(0), pc(0), cur_instr(nullptr) {}
    DEFAULT_COPY_SEMANTIC(FunctionalModel);
    DEFAULT_MOVE_SEMANTIC(FunctionalModel);
    ~FunctionalModel() noexcept override = default;

    void LoadIntoMemory(const std::vector<uint32_t>& data, uint32_t start_addr) {
        memory.SetRange(data, start_addr);
    }

    void SetPC(uint32_t new_pc) {
        pc = new_pc;
    }

    void Run();

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
        return memory.GetWord(pc);
    }

    size_t ticks_counter;
    MemoryUnit memory;
    RegFile registers;
    uint32_t pc;
    std::unique_ptr<RV32IInstruction> cur_instr;
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_MODEL_H
