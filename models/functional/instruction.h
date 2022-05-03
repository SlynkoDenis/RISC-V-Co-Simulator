#ifndef HW_CO_SIMULATION_INSTRUCTION_H
#define HW_CO_SIMULATION_INSTRUCTION_H

#include <array>
#include <iostream>
#include "macros.h"


namespace functional {
enum class RV32IType : uint8_t {
    RType,
    IType,
    SType,
    BType,
    UType,
    JType,
    UNKNOWN,
};

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

std::ostream &operator<<(std::ostream &os, RV32I instr);


#define GETTER(name, low, high)     \
    constexpr inline uint32_t Get##name() const { return GetRange(low, high); }

struct RV32IInstruction {
    explicit RV32IInstruction(uint32_t raw_instr): raw(raw_instr) {}

    GETTER(Opcode, 0, 7);

    virtual void dump() const {
        std::cout << std::hex << raw;
    }

    uint32_t raw;

protected:
    constexpr inline uint32_t GetRange(size_t low, size_t high) const {
        ASSERT(high <= 32 && low < high);
        if (UNLIKELY(high - low == 32)) {   // prevent overflow
            return raw;
        }
        return (raw >> low) & ((1u << (high - low)) - 1u);
    }
};

struct RV32ITypeR : public RV32IInstruction {
    explicit RV32ITypeR(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, 7, 12);
    GETTER(Funct3, 12, 15);
    GETTER(Rs1, 15, 20);
    GETTER(Rs2, 20, 25);
    GETTER(Funct7, 25, 32);

    void dump() const override;
};

struct RV32ITypeI : public RV32IInstruction {
    explicit RV32ITypeI(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, 7, 12);
    GETTER(Funct3, 12, 15);
    GETTER(Rs1, 15, 20);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t instr;
            struct {
                uint32_t pad0 : 20;
                uint32_t imm0 : 12;
            } __attribute__((packed)) instr_i_type;
            struct {
                uint32_t imm0 : 12;
                uint32_t se : 20;
            } __attribute__((packed)) imm_i_type;
        } in{.instr = raw}, out{.instr = 0};
        uint8_t sign = (in.instr >> 31) & 1;

        out.imm_i_type.imm0 = in.instr_i_type.imm0;
        if (sign) {
            out.imm_i_type.se--;
        }
        return out.instr;
    }

    void dump() const override;
};

struct RV32ITypeS : public RV32IInstruction {
    explicit RV32ITypeS(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Funct3, 12, 15);
    GETTER(Rs1, 15, 20);
    GETTER(Rs2, 20, 25);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t instr;
            struct {
                uint32_t pad0 : 7;
                uint32_t imm0 : 5;
                uint32_t pad1 : 13;
                uint32_t imm1 : 7;
            } __attribute__((packed)) instr_s_type;
            struct {
                uint32_t imm0 : 5;
                uint32_t imm1 : 7;
                uint32_t se : 20;
            } __attribute__((packed)) imm_s_type;
        } in{.instr = raw}, out{.instr = 0};
        uint8_t sign = (in.instr >> 31) & 1;

        out.imm_s_type.imm0 = in.instr_s_type.imm0;
        out.imm_s_type.imm1 = in.instr_s_type.imm1;
        if (sign) {
            out.imm_s_type.se--;
        }
        return out.instr;
    }

    void dump() const override;
};

struct RV32ITypeB : public RV32IInstruction {
    explicit RV32ITypeB(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Funct3, 12, 15);
    GETTER(Rs1, 15, 20);
    GETTER(Rs2, 20, 25);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t instr;
            struct {
                uint32_t pad0 : 7;
                uint32_t imm2 : 1;
                uint32_t imm0 : 4;
                uint32_t pad2 : 13;
                uint32_t imm1 : 6;
                uint32_t sign : 1;
            } __attribute__((packed)) instr_b_type;
            struct {
                uint32_t zero : 1;
                uint32_t imm0 : 4;
                uint32_t imm1 : 6;
                uint32_t imm2 : 1;
                uint32_t se : 20;
            } __attribute__((packed)) imm_b_type;
        } in{.instr = raw}, out{.instr = 0};
        uint8_t sign = in.instr_b_type.sign;

        out.imm_b_type.imm0 = in.instr_b_type.imm0;
        out.imm_b_type.imm1 = in.instr_b_type.imm1;
        out.imm_b_type.imm2 = in.instr_b_type.imm2;
        if (sign) {
            out.imm_b_type.se--;
        }
        return out.instr;
    }

    void dump() const override;
};

struct RV32ITypeU : public RV32IInstruction {
    explicit RV32ITypeU(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, 7, 12);
    GETTER(Imm, 12, 32);

    void dump() const override;
    constexpr inline uint32_t GetImm() const {
        return raw & (-(1u << 12));
    }
};

struct RV32ITypeJ : public RV32IInstruction {
    explicit RV32ITypeJ(uint32_t raw_instr) : RV32IInstruction(raw_instr) {}

    GETTER(Rd, 7, 12);
    constexpr inline uint32_t GetImm() const {
        union imm_layout {
            uint32_t instr;
            struct {
                uint32_t pad0 : 12;
                uint32_t imm2 : 8;
                uint32_t imm1 : 1;
                uint32_t imm0 : 10;
                uint32_t sign : 1;
            } __attribute__((packed)) instr_j_type;
            struct {
                uint32_t zero : 1;
                uint32_t imm0 : 10;
                uint32_t imm1 : 1;
                uint32_t imm2 : 8;
                uint32_t se : 12;
            } __attribute__((packed)) imm_j_type;
        } in{.instr = raw}, out{.instr = 0};
        uint8_t sign = in.instr_j_type.sign;

        out.imm_j_type.imm0 = in.instr_j_type.imm0;
        out.imm_j_type.imm1 = in.instr_j_type.imm1;
        out.imm_j_type.imm2 = in.instr_j_type.imm2;
        if (sign) {
            out.imm_j_type.se--;
        }
        return out.instr;
    }

    void dump() const override;
};

#undef GETTER
}   // end namespace functional

#endif // HW_CO_SIMULATION_INSTRUCTION_H
