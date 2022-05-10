#ifndef HW_CO_SIMULATION_INSTRUCTION_H
#define HW_CO_SIMULATION_INSTRUCTION_H

#include <array>
#include <iostream>
#include "macros.h"


namespace functional {
static constexpr std::array<const char*, 41> instructions_names = {
    "LUI",
    "AUIPC",
    "JAL",
    "JALR",
    "BEQ",
    "BNE",
    "BLT",
    "BGE",
    "BLTU",
    "BGEU",
    "LB",
    "LH",
    "LW",
    "LBU",
    "LHU",
    "SB",
    "SH",
    "SW",
    "ADDI",
    "SLTI",
    "SLTIU",
    "XORI",
    "ORI",
    "ANDI",
    "SLLI",
    "SRLI",
    "SRAI",
    "ADD",
    "SUB",
    "SLL",
    "SLT",
    "SLTU",
    "XOR",
    "SRL",
    "SRA",
    "OR",
    "AND",
    "FENCE",
    "ECALL",
    "EBREAK",
    "UNKNOWN"
};

enum class RV32I : uint8_t {
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    FENCE,
    ECALL,
    EBREAK,
    UNKNOWN,
};

inline std::ostream &operator<<(std::ostream &os, RV32I instr) {
    os << instructions_names[static_cast<size_t>(instr)];
    return os;
}


union InstrLayout {
    uint32_t raw;
    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t funct3  : 3;
        uint32_t rs1     : 5;
        uint32_t rs2     : 5;
        uint32_t funct7  : 7;
    } __attribute__((packed)) r_type;
    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t funct3  : 3;
        uint32_t rs1     : 5;
        uint32_t imm     : 12;
    } __attribute__((packed)) i_type;
    struct {
        uint32_t opcode  : 7;
        uint32_t imm0    : 5;
        uint32_t funct3  : 3;
        uint32_t rs1     : 5;
        uint32_t rs2     : 5;
        uint32_t imm1    : 7;
    } __attribute__((packed)) s_type;
    struct {
        uint32_t opcode  : 7;
        uint32_t imm2    : 1;
        uint32_t imm0    : 4;
        uint32_t funct3  : 3;
        uint32_t rs1     : 5;
        uint32_t rs2     : 5;
        uint32_t imm1    : 6;
        uint32_t imm3    : 1;
    } __attribute__((packed)) b_type;
    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t imm     : 20;
    } __attribute__((packed)) u_type;
    struct {
        uint32_t opcode  : 7;
        uint32_t rd      : 5;
        uint32_t imm2    : 8;
        uint32_t imm1    : 1;
        uint32_t imm0    : 10;
        uint32_t imm3    : 1;
    } __attribute__((packed)) j_type;
};

#define GETTER(name, instr_type, struct_name)   \
    constexpr inline uint32_t Get##name() const { return instr.instr_type.struct_name; }

struct RV32IInstruction {
    explicit RV32IInstruction(uint32_t raw_instr): instr{.raw = raw_instr} {}

    GETTER(Opcode, r_type, opcode);

    virtual void Dump() const { DumpImpl(std::cout); }
    virtual void DumpImpl(std::ostream &os) const {
        os << std::hex << instr.raw;
    }

    InstrLayout instr;
};

struct RV32ITypeR : public RV32IInstruction {
    explicit RV32ITypeR(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, r_type, rd);
    GETTER(Funct3, r_type, funct3);
    GETTER(Rs1, r_type, rs1);
    GETTER(Rs2, r_type, rs2);
    GETTER(Funct7, r_type, funct7);

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRd() << ", x" << GetRs1() << ", x" << GetRs2() << std::endl;
    }
};

struct RV32ITypeI : public RV32IInstruction {
    explicit RV32ITypeI(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, i_type, rd);
    GETTER(Funct3, i_type, funct3);
    GETTER(Rs1, i_type, rs1);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t raw_instr;
            struct {
                uint32_t imm : 12;
                uint32_t se  : 20;
            } __attribute__((packed)) imm_i_type;
        } out{.raw_instr = 0};

        out.imm_i_type.imm = instr.i_type.imm;
        if ((instr.raw >> 31) & 1) {
            out.imm_i_type.se--;
        }
        return out.raw_instr;
    }

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRd() << ", x" << GetRs1() << ", " << static_cast<int32_t>(GetImm()) << std::endl;
    }
};

struct RV32ITypeS : public RV32IInstruction {
    explicit RV32ITypeS(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Funct3, s_type, funct3);
    GETTER(Rs1, s_type, rs1);
    GETTER(Rs2, s_type, rs2);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t raw_instr;
            struct {
                uint32_t imm0 : 5;
                uint32_t imm1 : 7;
                uint32_t se : 20;
            } __attribute__((packed)) imm_s_type;
        } out{.raw_instr = 0};

        out.imm_s_type.imm0 = instr.s_type.imm0;
        out.imm_s_type.imm1 = instr.s_type.imm1;
        if ((instr.raw >> 31) & 1) {
            out.imm_s_type.se--;
        }
        return out.raw_instr;
    }

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRs1() << ", x" << GetRs2() << ", " << static_cast<int32_t>(GetImm()) << std::endl;
    }
};

struct RV32ITypeB : public RV32IInstruction {
    explicit RV32ITypeB(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Funct3, b_type, funct3);
    GETTER(Rs1, b_type, rs1);
    GETTER(Rs2, b_type, rs2);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t raw_instr;
            struct {
                uint32_t zero : 1;
                uint32_t imm0 : 4;
                uint32_t imm1 : 6;
                uint32_t imm2 : 1;
                uint32_t se : 20;
            } __attribute__((packed)) imm_b_type;
        } out{.raw_instr = 0};

        out.imm_b_type.imm0 = instr.b_type.imm0;
        out.imm_b_type.imm1 = instr.b_type.imm1;
        out.imm_b_type.imm2 = instr.b_type.imm2;
        if (instr.b_type.imm3) {
            out.imm_b_type.se--;
        }
        return out.raw_instr;
    }

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRs1() << ", x" << GetRs2() << ", " << static_cast<int32_t>(GetImm()) << std::endl;
    }
};

struct RV32ITypeU : public RV32IInstruction {
    explicit RV32ITypeU(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, u_type, rd);
    constexpr inline uint32_t GetImm() const {
        return instr.raw & (-(1u << 12));
    }

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRd() << ", " << GetImm() << std::endl;
    }
};

struct RV32ITypeJ : public RV32IInstruction {
    explicit RV32ITypeJ(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, j_type, rd);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t raw_instr;
            struct {
                uint32_t zero : 1;
                uint32_t imm0 : 10;
                uint32_t imm1 : 1;
                uint32_t imm2 : 8;
                uint32_t se : 12;
            } __attribute__((packed)) imm_j_type;
        } out{.raw_instr = 0};

        out.imm_j_type.imm0 = instr.j_type.imm0;
        out.imm_j_type.imm1 = instr.j_type.imm1;
        out.imm_j_type.imm2 = instr.j_type.imm2;
        if (instr.j_type.imm3) {
            out.imm_j_type.se--;
        }
        return out.raw_instr;
    }

    void DumpImpl(std::ostream &os) const override {
        os << std::dec;
        os << 'x' << GetRd() << ", " << static_cast<int32_t>(GetImm()) << std::endl;
    }
};

#undef GETTER
}   // end namespace functional

#endif // HW_CO_SIMULATION_INSTRUCTION_H
