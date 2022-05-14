#ifndef HW_CO_SIMULATION_DECODER_H
#define HW_CO_SIMULATION_DECODER_H

#include "instruction.h"
#include "macros.h"


#define SWITCH_DECODER(instr_name, untyped_instr, r_instr, i_instr, s_instr, b_instr, u_instr, j_instr) \
    switch(untyped_instr.GetOpcode()) {                                                         \
    default:                                                                                    \
        /* TODO: better error-handling */                                                       \
        UNREACHABLE("invalid instruction");                                                     \
        break;                                                                                  \
    case 0b0110111:                                                                             \
        u_instr.instr.raw = untyped_instr.instr.raw;                                            \
        instr_name = RV32I::LUI;                                                                \
        break;                                                                                  \
    case 0b0010111:                                                                             \
        u_instr.instr.raw = untyped_instr.instr.raw;                                            \
        instr_name = RV32I::AUIPC;                                                              \
        break;                                                                                  \
    case 0b1101111:                                                                             \
        j_instr.instr.raw = untyped_instr.instr.raw;                                            \
        instr_name = RV32I::JAL;                                                                \
        break;                                                                                  \
    case 0b1100111:                                                                             \
        i_instr.instr.raw = untyped_instr.instr.raw;                                            \
        ASSERT(i_instr.GetFunct3() == 0);                                                       \
        instr_name = RV32I::JALR;                                                               \
        break;                                                                                  \
    case 0b1100011: {                                                                           \
        /* TODO: can be implemented conditioned BLT = BEQ + 4 by adding funct3 offset to BEQ */ \
        b_instr.instr.raw = untyped_instr.instr.raw;                                            \
        switch (b_instr.GetFunct3()) {                                                          \
        default:                                                                                \
            instr_name = RV32I::UNKNOWN;                                                        \
            break;                                                                              \
        case 0:                                                                                 \
            instr_name = RV32I::BEQ;                                                            \
            break;                                                                              \
        case 1:                                                                                 \
            instr_name = RV32I::BNE;                                                            \
            break;                                                                              \
        case 0b100:                                                                             \
            instr_name = RV32I::BLT;                                                            \
            break;                                                                              \
        case 0b101:                                                                             \
            instr_name = RV32I::BGE;                                                            \
            break;                                                                              \
        case 0b110:                                                                             \
            instr_name = RV32I::BLTU;                                                           \
            break;                                                                              \
        case 0b111:                                                                             \
            instr_name = RV32I::BGEU;                                                           \
            break;                                                                              \
        }                                                                                       \
        break;                                                                                  \
    }                                                                                           \
    case 0b0000011: {                                                                           \
        i_instr.instr.raw = untyped_instr.instr.raw;                                            \
        switch (i_instr.GetFunct3()) {                                                          \
        default:                                                                                \
            instr_name = RV32I::UNKNOWN;                                                        \
            break;                                                                              \
        case 0:                                                                                 \
            instr_name = RV32I::LB;                                                             \
            break;                                                                              \
        case 1:                                                                                 \
            instr_name = RV32I::LH;                                                             \
            break;                                                                              \
        case 0b010:                                                                             \
            instr_name = RV32I::LW;                                                             \
            break;                                                                              \
        case 0b100:                                                                             \
            instr_name = RV32I::LBU;                                                            \
            break;                                                                              \
        case 0b101:                                                                             \
            instr_name = RV32I::LHU;                                                            \
            break;                                                                              \
        }                                                                                       \
        break;                                                                                  \
    }                                                                                           \
    case 0b0100011: {                                                                           \
        s_instr.instr.raw = untyped_instr.instr.raw;                                            \
        switch (s_instr.GetFunct3()) {                                                          \
        default:                                                                                \
            instr_name = RV32I::UNKNOWN;                                                        \
            break;                                                                              \
        case 0:                                                                                 \
            instr_name = RV32I::SB;                                                             \
            break;                                                                              \
        case 1:                                                                                 \
            instr_name = RV32I::SH;                                                             \
            break;                                                                              \
        case 2:                                                                                 \
            instr_name = RV32I::SW;                                                             \
            break;                                                                              \
        }                                                                                       \
        break;                                                                                  \
    }                                                                                           \
    case 0b0010011: {                                                                           \
        i_instr.instr.raw = untyped_instr.instr.raw;                                            \
        switch (i_instr.GetFunct3()) {                                                          \
        default:                                                                                \
            instr_name = RV32I::UNKNOWN;                                                        \
            break;                                                                              \
        case 0:                                                                                 \
            instr_name = RV32I::ADDI;                                                           \
            break;                                                                              \
        case 0b010:                                                                             \
            instr_name = RV32I::SLTI;                                                           \
            break;                                                                              \
        case 0b011:                                                                             \
            instr_name = RV32I::SLTIU;                                                          \
            break;                                                                              \
        case 0b100:                                                                             \
            instr_name = RV32I::XORI;                                                           \
            break;                                                                              \
        case 0b110:                                                                             \
            instr_name = RV32I::ORI;                                                            \
            break;                                                                              \
        case 0b111:                                                                             \
            instr_name = RV32I::ANDI;                                                           \
            break;                                                                              \
        case 0b001:                                                                             \
            /* TODO: here and below should verify imm[11:5]/funct7 equals 0100000/0000000 */    \
            instr_name = RV32I::SLLI;                                                           \
            break;                                                                              \
        case 0b101:                                                                             \
            instr_name = untyped_instr.instr.raw & (1 << 30) ? RV32I::SRAI : RV32I::SRLI;       \
            break;                                                                              \
        }                                                                                       \
        break;                                                                                  \
    }                                                                                           \
    case 0b0110011: {                                                                           \
        r_instr.instr.raw = untyped_instr.instr.raw;                                            \
        switch (r_instr.GetFunct3()) {                                                          \
        default:                                                                                \
            instr_name = RV32I::UNKNOWN;                                                        \
            break;                                                                              \
        case 0:                                                                                 \
            instr_name = untyped_instr.instr.raw & (1 << 30) ? RV32I::SUB : RV32I::ADD;         \
            break;                                                                              \
        case 1:                                                                                 \
            instr_name = RV32I::SLL;                                                            \
            break;                                                                              \
        case 0b010:                                                                             \
            instr_name = RV32I::SLT;                                                            \
            break;                                                                              \
        case 0b011:                                                                             \
            instr_name = RV32I::SLTU;                                                           \
            break;                                                                              \
        case 0b100:                                                                             \
            instr_name = RV32I::XOR;                                                            \
            break;                                                                              \
        case 0b101:                                                                             \
            instr_name = untyped_instr.instr.raw & (1 << 30) ? RV32I::SRA : RV32I::SRL;         \
            break;                                                                              \
        case 0b110:                                                                             \
            instr_name = RV32I::OR;                                                             \
            break;                                                                              \
        case 0b111:                                                                             \
            instr_name = RV32I::AND;                                                            \
            break;                                                                              \
        }                                                                                       \
        break;                                                                                  \
    }                                                                                           \
    case 0b0001111:                                                                             \
        i_instr.instr.raw = untyped_instr.instr.raw;                                            \
        instr_name = RV32I::FENCE;                                                              \
        break;                                                                                  \
    case 0b1110011:                                                                             \
        i_instr.instr.raw = untyped_instr.instr.raw;                                            \
        instr_name = untyped_instr.instr.raw & (1 << 20) ? RV32I::EBREAK : RV32I::ECALL;        \
        break;                                                                                  \
    }

#endif // HW_CO_SIMULATION_DECODER_H
