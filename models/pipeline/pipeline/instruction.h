#ifndef HW_CO_SIMULATION_PIPELINE_INSTRUCTION_H
#define HW_CO_SIMULATION_PIPELINE_INSTRUCTION_H

#include "common.h"
#include "instr_exceptions.h"
#include <iostream>


namespace pipeline::instruction {
enum class InstructionType : uint8_t {
    UNKNOWN,
    RType,
    IType,
    SType,
    BType,
    UType,
    JType
};

constexpr inline uint8_t getFunct7(uint32_t instruction) {
    return static_cast<uint8_t>((instruction >> 25) & 0x0000007f);
}

constexpr inline uint8_t getRs2(uint32_t instruction) {
    return static_cast<uint8_t>((instruction >> 20) & 0x0000001f);
}

constexpr inline uint8_t getRs1(uint32_t instruction) {
    return static_cast<uint8_t>((instruction >> 15) & 0x0000001f);
}

constexpr inline uint8_t getFunct3(uint32_t instruction) {
    return static_cast<uint8_t>((instruction >> 12) & 0x00000007);
}

constexpr inline uint8_t getRd(uint32_t instruction) {
    return static_cast<uint8_t>((instruction >> 7) & 0x0000001f);
}

constexpr inline uint8_t getOpcode(uint32_t instruction) {
    return static_cast<uint8_t>(instruction & 0x0000007f);
}

constexpr inline InstructionType GetInstructionType(uint8_t opcode) {
    switch (opcode) {
        case 0b0110011:
            return InstructionType::RType;
        case 0b0100011:
            return InstructionType::SType;
        case 0b1100011:
            return InstructionType::BType;
        case 0b0000011:
        case 0b0010011:
        case 0b1110011:
        case 0b1100111:
            return InstructionType::IType;
        case 0b0110111:
        case 0b0010111:
            return InstructionType::UType;
        case 0b1101111:
            return InstructionType::JType;
        default:
            return InstructionType::UNKNOWN;
            throw InvalidInstructionException("invalid opcode: " + std::to_string(opcode));
    }
}

constexpr inline bool hasRs1(InstructionType type) {
    return (type != InstructionType::UType && type != InstructionType::JType);
}

constexpr inline bool hasRs2(InstructionType type) {
    return (type == InstructionType::RType || type == InstructionType::IType ||
            type == InstructionType::SType || type == InstructionType::BType);
}

constexpr inline uint32_t ImmediateExtensionBlock(uint32_t instr) {
    auto instr_t = GetInstructionType(getOpcode(instr));
    if (instr_t == InstructionType::UNKNOWN) {
        return 0;
    }
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
        struct {
            uint32_t pad : 12;
            uint32_t imm : 20;
        } __attribute__((packed)) instr_u_type;
        struct {
            uint32_t zeros : 12;
            uint32_t imm : 20;
        } __attribute__((packed)) imm_u_type;
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
    } in{.instr = instr}, out{.instr = 0};
    uint8_t sign = (in.instr >> 31) & 1;

    switch (instr_t) {
    default:
        throw std::logic_error("invalid instruction type: " + std::to_string(static_cast<uint8_t>(instr_t)));
    case InstructionType::RType:
        break;
    case InstructionType::IType:
        out.imm_i_type.imm0 = in.instr_i_type.imm0;
        if (sign) {
            out.imm_i_type.se--;
        }
        break;
    case InstructionType::SType:
        out.imm_s_type.imm0 = in.instr_s_type.imm0;
        out.imm_s_type.imm1 = in.instr_s_type.imm1;
        if (sign) {
            out.imm_s_type.se--;
        }
        break;
    case InstructionType::BType:
        out.imm_b_type.imm0 = in.instr_b_type.imm0;
        out.imm_b_type.imm1 = in.instr_b_type.imm1;
        out.imm_b_type.imm2 = in.instr_b_type.imm2;
        if (sign) {
            out.imm_b_type.se--;
        }
        break;
    case InstructionType::JType:
        out.imm_j_type.imm0 = in.instr_j_type.imm0;
        out.imm_j_type.imm1 = in.instr_j_type.imm1;
        out.imm_j_type.imm2 = in.instr_j_type.imm2;
        if (sign) {
            out.imm_j_type.se--;
        }
        break;
    case InstructionType::UType:
        out.imm_u_type.imm = in.instr_u_type.imm;
        break;
    }

    return out.instr;
}
}   // end namespace pipeline::instruction

#endif // HW_CO_SIMULATION_PIPELINE_INSTRUCTION_H
