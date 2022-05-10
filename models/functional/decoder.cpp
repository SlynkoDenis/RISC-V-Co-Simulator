#include "decoder.h"

namespace functional {

RV32IDecodeResult Decode(uint32_t raw_instr) {
    RV32IInstruction instr{raw_instr};
    switch(instr.GetOpcode()) {
    default:
        return {new RV32IInstruction{raw_instr}, RV32I::UNKNOWN};
    case 0b0110111:
        return {new RV32ITypeU(raw_instr), RV32I::LUI};
    case 0b0010111:
        return {new RV32ITypeU(raw_instr), RV32I::AUIPC};
    case 0b1101111:
        return {new RV32ITypeJ(raw_instr), RV32I::JAL};
    case 0b1100111: {
        auto *res = new RV32ITypeI(raw_instr);
        ASSERT(res->GetFunct3() == 0);
        return {res, RV32I::JALR};
    }
    case 0b1100011: {
        // TODO: can be implemented conditioned BLT = BEQ + 4
        // by adding funct3 offset to BEQ
        auto *res = new RV32ITypeB(raw_instr);
        switch (res->GetFunct3())
        {
        default:
            return {res, RV32I::UNKNOWN};
        case 0:
            return {res, RV32I::BEQ};
        case 1:
            return {res, RV32I::BNE};
        case 0b100:
            return {res, RV32I::BLT};
        case 0b101:
            return {res, RV32I::BGE};
        case 0b110:
            return {res, RV32I::BLTU};
        case 0b111:
            return {res, RV32I::BGEU};
        }
    }
    case 0b0000011: {
        auto *res = new RV32ITypeI(raw_instr);
        switch (res->GetFunct3())
        {
        default:
            return {res, RV32I::UNKNOWN};
        case 0:
            return {res, RV32I::LB};
        case 1:
            return {res, RV32I::LH};
        case 0b010:
            return {res, RV32I::LW};
        case 0b100:
            return {res, RV32I::LBU};
        case 0b101:
            return {res, RV32I::LHU};
        }
    }
    case 0b0100011: {
        auto *res = new RV32ITypeS(raw_instr);
        switch (res->GetFunct3())
        {
        default:
            return {res, RV32I::UNKNOWN};
        case 0:
            return {res, RV32I::SB};
        case 1:
            return {res, RV32I::SH};
        case 2:
            return {res, RV32I::SW};
        }
    }
    case 0b0010011: {
        auto *res = new RV32ITypeI(raw_instr);
        switch (res->GetFunct3())
        {
        default:
            return {res, RV32I::UNKNOWN};
        case 0:
            return {res, RV32I::ADDI};
        case 0b010:
            return {res, RV32I::SLTI};
        case 0b011:
            return {res, RV32I::SLTIU};
        case 0b100:
            return {res, RV32I::XORI};
        case 0b110:
            return {res, RV32I::ORI};
        case 0b111:
            return {res, RV32I::ANDI};
        case 0b001:
            // TODO: here and below should verify that imm[11:5]/funct7 equals 0100000/0000000
            return {res, RV32I::SLLI};
        case 0b101: {
            auto type = raw_instr & (1 << 30) ? RV32I::SRAI : RV32I::SRLI;
            return {res, type};
        }
        }
    }
    case 0b0110011: {
        auto *res = new RV32ITypeR(raw_instr);
        switch (res->GetFunct3())
        {
        default:
            return {res, RV32I::UNKNOWN};
        case 0: {
            auto type = raw_instr & (1 << 30) ? RV32I::SUB : RV32I::ADD;
            return {res, type};
        }
        case 1:
            return {res, RV32I::SLL};
        case 0b010:
            return {res, RV32I::SLT};
        case 0b011:
            return {res, RV32I::SLTU};
        case 0b100:
            return {res, RV32I::XOR};
        case 0b101: {
            auto type = raw_instr & (1 << 30) ? RV32I::SRA : RV32I::SRL;
            return {res, type};
        }
        case 0b110:
            return {res, RV32I::OR};
        case 0b111:
            return {res, RV32I::AND};
        }
    }
    case 0b0001111:
        return {new RV32ITypeI(raw_instr), RV32I::FENCE};
    case 0b1110011:
        auto type = raw_instr & (1 << 20) ? RV32I::EBREAK : RV32I::ECALL;
        return {new RV32ITypeI(raw_instr), type};
    }
}
}   // end namespace functional
