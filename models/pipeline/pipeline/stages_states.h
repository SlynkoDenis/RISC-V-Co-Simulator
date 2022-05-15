#ifndef HW_CO_SIMULATION_STAGES_STATES_H
#define HW_CO_SIMULATION_STAGES_STATES_H

#include "alu.h"
#include "cmp.h"
#include <iostream>


namespace pipeline {

#define LOG_FIELD(obj, field) os << #field << "=" << std::hex << obj.field << ", "

struct DecodeState {
    DecodeState(uint32_t value) : v_de(static_cast<bool>(value)),
                               pc_de(value),
                               instr(value) {};

    bool v_de;
    uint32_t pc_de;
    uint32_t instr;
};

inline std::ostream& operator<<(std::ostream& os, const DecodeState& decode_state) {
    LOG_FIELD(decode_state, v_de);
    LOG_FIELD(decode_state, pc_de);
    LOG_FIELD(decode_state, instr);
    return os;
}


struct ExecuteState {
    ExecuteState(uint32_t value) : funct3(static_cast<uint8_t>(value)),
                                is_jalr(static_cast<bool>(value)),
                                alu_op(static_cast<modules::ALUControl>(value)),
                                alu_src2(static_cast<bool>(value)),
                                mem_to_reg(static_cast<bool>(value)),
                                wb_we(static_cast<bool>(value)),
                                mem_we(static_cast<bool>(value)),
                                cmp_control(static_cast<modules::CmpControl>(value)),
                                brn_cond(static_cast<bool>(value)),
                                jmp_cond(static_cast<bool>(value)),
                                v_de(static_cast<bool>(value)),
                                data1(value),
                                data2(value),
                                pc_de(value),
                                instr(value) {};

    uint8_t funct3;
    bool is_jalr;
    modules::ALUControl alu_op;
    bool alu_src2;
    bool mem_to_reg;
    bool wb_we;
    bool mem_we;
    modules::CmpControl cmp_control;
    bool brn_cond;
    bool jmp_cond;
    bool v_de;
    uint32_t data1;
    uint32_t data2;
    uint32_t pc_de;
    uint32_t instr;
};

inline std::ostream& operator<<(std::ostream& os, const ExecuteState& execute_state) {
    os << "funct3=" << std::hex << static_cast<uint32_t>(execute_state.funct3) << ", ";
    LOG_FIELD(execute_state, is_jalr);
    LOG_FIELD(execute_state, alu_op);
    LOG_FIELD(execute_state, alu_src2);
    LOG_FIELD(execute_state, mem_to_reg);
    LOG_FIELD(execute_state, wb_we);
    LOG_FIELD(execute_state, mem_we);
    LOG_FIELD(execute_state, cmp_control);
    LOG_FIELD(execute_state, brn_cond);
    LOG_FIELD(execute_state, jmp_cond);
    LOG_FIELD(execute_state, v_de);
    LOG_FIELD(execute_state, data1);
    LOG_FIELD(execute_state, data2);
    LOG_FIELD(execute_state, pc_de);
    LOG_FIELD(execute_state, instr);
    return os;
}


struct MemoryState {
    MemoryState(uint32_t value) : mem_we(static_cast<bool>(value)),
                               mem_to_reg(static_cast<bool>(value)),
                               wb_we(static_cast<bool>(value)),
                               jmp_cond(static_cast<bool>(value)),
                               store_mode(static_cast<uint8_t>(value)),
                               write_data(value),
                               alu_res(value),
                               wb_a(value) {};

    bool mem_we;
    bool mem_to_reg;
    bool wb_we;
    bool jmp_cond;
    uint8_t store_mode;
    uint32_t write_data;
    uint32_t alu_res;
    uint32_t wb_a;
};

inline std::ostream& operator<<(std::ostream& os, const MemoryState& memory_state) {
    LOG_FIELD(memory_state, mem_we);
    LOG_FIELD(memory_state, mem_to_reg);
    LOG_FIELD(memory_state, wb_we);
    LOG_FIELD(memory_state, jmp_cond);
    os << "store_mode" << "=" << std::hex << static_cast<uint32_t>(memory_state.store_mode) << ", ";
    LOG_FIELD(memory_state, write_data);
    LOG_FIELD(memory_state, alu_res);
    LOG_FIELD(memory_state, wb_a);
    return os;
}


struct WriteBackState {
    WriteBackState(uint32_t value) : wb_we(static_cast<bool>(value)),
                                  wb_d(value),
                                  wb_a(value) {};

    bool wb_we;
    uint32_t wb_d;
    uint8_t wb_a;
};

inline std::ostream& operator<<(std::ostream& os, const WriteBackState& wb_state) {
    LOG_FIELD(wb_state, wb_we);
    LOG_FIELD(wb_state, wb_d);
    os << "wb_a" << "=" << std::hex << static_cast<uint32_t>(wb_state.wb_a);
    return os;
}

#undef LOG_FIELD
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_STAGES_STATES_H
