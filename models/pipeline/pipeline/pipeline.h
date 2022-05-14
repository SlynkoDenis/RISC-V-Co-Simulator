#ifndef HW_CO_SIMULATION_PIPELINE_H
#define HW_CO_SIMULATION_PIPELINE_H

#include <array>
#include "control_unit.h"
#include "hazard_unit.h"
#include "alu.h"
#include "cmp.h"
#include "common.h"
#include "data_mem_unit.h"
#include "instr_mem_unit.h"
#include "macros.h"
#include "mmu_fixed_offset.h"
#include "register.h"
#include "regfile.h"
#include "runtime.h"
#include <unordered_map>
#include <vector>


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


class PipelineModel : public runtime::Runtime {
public:
    PipelineModel(): mmu(), instr_mem_unit(&mmu), data_mem_unit(&mmu) {}
    DEFAULT_COPY_SEMANTIC(PipelineModel);
    DEFAULT_MOVE_SEMANTIC(PipelineModel);
    ~PipelineModel() noexcept override {};

    long long GetTicksCounter() { return ticks_counter; }

    virtual void SetPC(uint32_t pc) {
        program_counter.next = pc;
        program_counter.Tick();
    }
    virtual void InitSP() {
        auto sp = mmu.AllocateStack();
        reg_file.InitSP(sp);
    }

    virtual void Tick();

    virtual runtime::ReturnCodes Run();
    runtime::ReturnCodes RunProgram(const char *path) override;

    const modules::RegFile &GetRegFile() const {
        return reg_file;
    }

    void MakeTracesReadable(bool readable) {
        readable_traces_ = readable;
        mmu.MakeTracesReadable(readable);
        reg_file.MakeTracesReadable(readable);
    }

#ifdef DEBUG
    virtual void PrintRegisters() const {
        reg_file.PrintRegisters();
    }

    virtual void Debug();
#endif

private:
    virtual void DoFetch();
    virtual void DoDecode();
    virtual void DoExecute();
    virtual void DoMemory();
    virtual void DoWriteBack();

    // after decoding ECALL/EBREAK instructions, pipeline may contain not-executed instruction
    // on memory/writeback stages; must finish them
    virtual void DoLastTicks();

    virtual void HazardUnitTick();
    void TickStateRegisters();

    void HaltPipeline();
    void RestartPipeline();

    // both method and variables are needed to write the correct execution trace
    // TODO: trace methods can be implemented in a derived class
#ifndef REMOVE_TRACES
    void TraceExecutedInstruction(uint32_t instr, uint32_t location);
    uint32_t trace_prev_pc_ = 0;
#endif
    bool readable_traces_ = false;

    long long ticks_counter = 0;
    int last_instructions_counter = 0;
    bool last_instructions_flag = false;

    uint32_t bp_mem = 0;
    uint32_t pc_disp = 0;
    bool pc_r = false;

    BypassOptionsEncoding hu_rs1 = BypassOptionsEncoding::REG;
    BypassOptionsEncoding hu_rs2 = BypassOptionsEncoding::REG;

    memory::MMUFixedOffset mmu;
    modules::InstrMemUnit instr_mem_unit;
    modules::DataMemUnit data_mem_unit;
    modules::RegFile reg_file;
    ControlUnit control_unit;
    modules::Register<uint32_t> program_counter;

    modules::Register<DecodeState> decode_register;
    modules::Register<ExecuteState> execute_register;
    modules::Register<MemoryState> memory_register;
    modules::Register<WriteBackState> write_back_register;
};

#undef LOG_FIELD
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_PIPELINE_H
