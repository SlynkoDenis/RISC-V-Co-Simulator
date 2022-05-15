#ifndef HW_CO_SIMULATION_CONTROL_UNIT_H
#define HW_CO_SIMULATION_CONTROL_UNIT_H

#include "alu.h"
#include "cmp.h"
#include "common.h"
#include "instr_exceptions.h"
#include "instruction.h"
#include "macros.h"
#include "stages_states.h"


namespace pipeline {
using modules::ALUControl;


class ControlUnit {
public:
    virtual ~ControlUnit() noexcept = default;

    void LoadControlSignals(ExecuteState &next_state) const {
        next_state.alu_op = alu_op;
        next_state.alu_src2 = alu_src2;
        next_state.is_jalr = is_jalr;
        next_state.wb_we = wb_we;
        next_state.mem_we = mem_we;
        next_state.mem_to_reg = mem_to_reg;
        next_state.brn_cond = brn_cond;
        next_state.jmp_cond = jmp_cond;
        next_state.cmp_control = cmp_control;
        next_state.funct3 = funct3;
    }

    auto GetInstructionType() const {
        return instr_type;
    }

    virtual void Tick() {
        // if (opcode == 0b0001111) {
        //     // TODO: verify EFENCE does nothing with current implementation
        // }
        if (opcode == 0b1110011) {
            ASSERT(funct3 == 0 && funct7 == 0);
            switch(rs2) {
            default:
                throw InvalidInstructionException("invalid imm[11:0] for ECALL/EBREAK instruction");
            case 0:
                throw EcallException();
            case 1:
                throw EbreakException();
            }
        }

        is_jalr = (opcode == 0b1100111);

        // for stores, branches and ecall we don't write anything back
        if ((opcode == 0b0100011) ||
            (opcode == 0b1100011) ||
            (opcode == 0b1110011)) {
            wb_we = false;
        } else {
            wb_we = true;
        }

        // if instruction is store
        mem_we = (opcode == 0b0100011);

        instr_type = instruction::GetInstructionType(opcode);
        if (instr_type == instruction::InstructionType::IType ||
            instr_type == instruction::InstructionType::SType ||
            instr_type == instruction::InstructionType::UType) {
            alu_src2 = true;
        } else {
            alu_src2 = false;
        }

        if (opcode == 0b0110011) {
            switch (funct3) {
            default:
                throw InvalidInstructionException("invalid funct3: " + std::to_string(funct3));
            case 0b000:
                if (funct7 == 0) {
                    alu_op = ALUControl::ADD;
                } else if (funct7 == 0b0100000) {
                    alu_op = ALUControl::SUB;
                } else {
                    throw InvalidInstructionException("invalid funct7 for ADD/SUB with funct3 == 0: " +\
                        std::to_string(funct7));
                }
                break;
            case 0b001:
                alu_op = ALUControl::SLL;
                break;
            case 0b010:
                alu_op = ALUControl::SLT;
                break;
            case 0b011:
                alu_op = ALUControl::SLTU;
                break;
            case 0b100:
                alu_op = ALUControl::XOR;
                break;
            case 0b101:
                if (funct7 == 0) {
                    alu_op = ALUControl::SRL;
                } else if (funct7 == 0b0100000) {
                    alu_op = ALUControl::SRA;
                } else {
                    throw InvalidInstructionException("invalid funct7 for SRL/SRA with funct3 == 0: " +\
                        std::to_string(funct7));
                }
                break;
            case 0b110:
                alu_op = ALUControl::OR;
                break;
            case 0b111:
                alu_op = ALUControl::AND;
                break;
            }
        } else if (opcode == 0b0010011) {
            switch (funct3) {
            default:
                throw InvalidInstructionException("invalid funct3: " + std::to_string(funct3));
            case 0b000:
                alu_op = ALUControl::ADD;
                break;
            case 0b010:
                alu_op = ALUControl::SLT;
                break;
            case 0b011:
                alu_op = ALUControl::SLTU;
                break;
            case 0b100:
                alu_op = ALUControl::XOR;
                break;
            case 0b110:
                alu_op = ALUControl::OR;
                break;
            case 0b111:
                alu_op = ALUControl::AND;
                break;
            case 0b001:
                if (funct7 == 0) {
                    alu_op = ALUControl::SLL;
                } else {
                    throw InvalidInstructionException("invalid funct7 for SLLI op: "+\
                        std::to_string(funct7));
                }
                break;
            case 0b101:
                if (funct7 == 0) {
                    alu_op = ALUControl::SRL;
                } else if (funct7 == 0b0100000) {
                    alu_op = ALUControl::SRA;
                } else {
                    throw InvalidInstructionException("invalid funct7 for SRLI/SRAI op: "+\
                        std::to_string(funct7));
                }
                break;
            }
        } else if (instr_type == instruction::InstructionType::UType || opcode == 0b1100111 ||
                   opcode == 0b0000011 || opcode == 0b0100011) {
            // addition is used for LUI, AUIPC, JALR, loads and stores
            alu_op = ALUControl::ADD;
        } else {
            alu_op = ALUControl::NOP;
        }

        brn_cond = (opcode == 0b1100011);

        if (opcode == 0b1101111 || opcode == 0b1100111) {
            jmp_cond = true;
        } else {
            jmp_cond = false;
        }

        // 0000011 - load instructions' opcode
        mem_to_reg = (opcode == 0b0000011);

        if (opcode == 0b1100011) {
            switch (funct3) {
            default:
                throw InvalidInstructionException("invalid funct3 for branch instruction: " + std::to_string(funct3));
            case 0:
                cmp_control = modules::CmpControl::EQ;
                break;
            case 0b001:
                cmp_control = modules::CmpControl::NE;
                break;
            case 0b100:
                cmp_control = modules::CmpControl::LT;
                break;
            case 0b101:
                cmp_control = modules::CmpControl::GE;
                break;
            case 0b110:
                cmp_control = modules::CmpControl::LTU;
                break;
            case 0b111:
                cmp_control = modules::CmpControl::GEU;
                break;
            }
        } else {
            cmp_control = modules::CmpControl::NOP;
        }
    }

#ifdef DEBUG
    virtual void Debug() const {
        std::cout << "ControlUnit: opcode=" << static_cast<uint32_t>(opcode) << "; funct3=";
        std::cout << static_cast<uint32_t>(funct3) << "; funct7=" << static_cast<uint32_t>(funct7);
        std::cout << "; wb_we=" << wb_we;
        std::cout << "; mem_to_reg=" << mem_to_reg << "; brn_cond=" << brn_cond << "; jmp_cond=" << jmp_cond;
        std::cout << "; cmp_cond=" << cmp_control << "; alu_src2=" << alu_src2 << "; alu_op=" << alu_op << std::endl;
    }
#endif

    uint8_t opcode = 0;
    uint8_t funct3 = 0;
    uint8_t funct7 = 0;
    uint8_t rs2 = 0;

private:
    bool is_jalr = false;
    bool wb_we = false;
    bool mem_we = false;
    bool mem_to_reg = false;
    bool brn_cond = false;
    bool jmp_cond = false;
    modules::CmpControl cmp_control;
    bool alu_src2 = false;
    ALUControl alu_op = ALUControl::NOP;

    instruction::InstructionType instr_type;
};
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_CONTROL_UNIT_H
