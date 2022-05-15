#ifndef HW_CO_SIMULATION_PIPELINE_H
#define HW_CO_SIMULATION_PIPELINE_H

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
#include "stages_states.h"


namespace pipeline {
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

    virtual void PrintRegisters() const {
        reg_file.PrintRegisters();
    }

#ifdef DEBUG
    virtual void Debug();
#endif

private:
    enum class LoadConflictUsedWB : uint8_t {
        NONE = 0,
        RS1 = 1,
        RS2 = 2,
    };

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

    LoadConflictUsedWB must_take_halted_wb = LoadConflictUsedWB::NONE;
    uint32_t halted_wb = 0;

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
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_PIPELINE_H
