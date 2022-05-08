#ifndef HW_CO_SIMULATION_INTERPRETER_H
#define HW_CO_SIMULATION_INTERPRETER_H

#include "decoder.h"
#include "macros.h"
#include "runtime.h"


namespace functional {
#define HANDLER(name) \
    virtual bool Handle##name(RV32IInstruction &instr) = 0

#define HANLDER_CASE(name)      \
    case RV32I::name:           \
        return Handle##name(instr)

class RV32IInterpreter : runtime::Runtime {
public:
    RV32IInterpreter() = default;
    DEFAULT_COPY_SEMANTIC(RV32IInterpreter);
    DEFAULT_MOVE_SEMANTIC(RV32IInterpreter);
    virtual ~RV32IInterpreter() noexcept = default;

    virtual void DumpState() const = 0;

    virtual runtime::ReturnCodes RunProgram(const char *path) = 0;

protected:
    bool Handle(RV32IInstruction &instr, RV32I name) {
        // TODO: implement Handle method with the dispatch table
        switch (name)
        {
        default:
            // TODO: handle unknown instruction
            ASSERT(false);
            return false;
        HANLDER_CASE(LUI);
        HANLDER_CASE(AUIPC);
        HANLDER_CASE(JAL);
        HANLDER_CASE(JALR);
        HANLDER_CASE(BEQ);
        HANLDER_CASE(BNE);
        HANLDER_CASE(BLT);
        HANLDER_CASE(BGE);
        HANLDER_CASE(BLTU);
        HANLDER_CASE(BGEU);
        HANLDER_CASE(LB);
        HANLDER_CASE(LH);
        HANLDER_CASE(LW);
        HANLDER_CASE(LBU);
        HANLDER_CASE(LHU);
        HANLDER_CASE(SB);
        HANLDER_CASE(SH);
        HANLDER_CASE(SW);
        HANLDER_CASE(ADDI);
        HANLDER_CASE(SLTI);
        HANLDER_CASE(SLTIU);
        HANLDER_CASE(XORI);
        HANLDER_CASE(ORI);
        HANLDER_CASE(ANDI);
        HANLDER_CASE(SLLI);
        HANLDER_CASE(SRLI);
        HANLDER_CASE(SRAI);
        HANLDER_CASE(ADD);
        HANLDER_CASE(SUB);
        HANLDER_CASE(SLL);
        HANLDER_CASE(SLT);
        HANLDER_CASE(SLTU);
        HANLDER_CASE(XOR);
        HANLDER_CASE(SRL);
        HANLDER_CASE(SRA);
        HANLDER_CASE(OR);
        HANLDER_CASE(AND);
        HANLDER_CASE(FENCE);
        HANLDER_CASE(ECALL);
        HANLDER_CASE(EBREAK);
        }
    }

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
    HANDLER(FENCE);
    HANDLER(ECALL);
    HANDLER(EBREAK);
};

#undef HANDLER
#undef HANLDER_CASE
}   // end namespace functional

#endif // HW_CO_SIMULATION_INTERPRETER_H
