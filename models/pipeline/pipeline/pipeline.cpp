#include "instruction.h"
#include "gates.h"
#include "load_elf.h"
#include "pipeline.h"


#ifdef REMOVE_TRACES
#define TraceExecutedInstruction(instr, location)   static_cast<void>(0)
#endif

namespace pipeline {
void PipelineModel::Tick() {
    DoWriteBack();
    DoMemory();
    DoExecute();
    DoDecode();
    DoFetch();

    if (!program_counter.enable_flag) {
        // Case of pipeline stall due to LW instruction conflict
        memory_register.clear();
    }
    TickStateRegisters();
    program_counter.Tick();
    HazardUnitTick();

    ++ticks_counter;
    // uncomment for verbose information per-cycle
    // DEBUG_LOG((*this));
}


void PipelineModel::DoFetch() {
    instr_mem_unit.address = program_counter.GetCurrent();

    auto summand1 = modules::Multiplexer2<uint32_t>{}(pc_r,
                                                      program_counter.GetCurrent(),
                                                      execute_register.GetCurrent().pc_de);
    auto summand2 = modules::Multiplexer2<uint32_t>{}(pc_r, 4, pc_disp);
    if (execute_register.GetCurrent().is_jalr) {
        // in case of JALR the address was already computed
        // in circuit this branch equals to logical AND or multiplexer
        ASSERT(pc_r);
        summand1 = 0;
    }

    program_counter.next = modules::Add<uint32_t>{}(summand1, summand2);
    if (program_counter.next % sizeof(uint32_t)) {
        throw modules::AlignmentException("pc is not aligned: pc_next == " +\
            std::to_string(program_counter.next));
    }

    // update next register
    decode_register.next.instr = instr_mem_unit.GetData();
    decode_register.next.pc_de = program_counter.GetCurrent();

#ifdef DEBUG
    std::cout << ">>> Fetched instruction " << std::hex << decode_register.next.instr;
    std::cout << " (" << program_counter.GetCurrent() << ")\n";
#endif
}


void PipelineModel::DoDecode() {
    auto decode_stage_instr = decode_register.GetCurrent();
    reg_file.SetNewSignals(instruction::getRs1(decode_stage_instr.instr),
                           instruction::getRs2(decode_stage_instr.instr),
                           write_back_register.GetCurrent().wb_a,
                           write_back_register.GetCurrent().wb_we,
                           write_back_register.GetCurrent().wb_d);
    reg_file.Tick();

    control_unit.opcode = instruction::getOpcode(decode_stage_instr.instr);
    control_unit.funct3 = instruction::getFunct3(decode_stage_instr.instr);
    control_unit.funct7 = instruction::getFunct7(decode_stage_instr.instr);
    control_unit.rs2 = instruction::getRs2(decode_stage_instr.instr);

    try {
        control_unit.Tick();
    } catch (EcallException &ec) {
        TraceExecutedInstruction(decode_stage_instr.instr, decode_stage_instr.pc_de);
        DoLastTicks();
        throw ec;
    } catch (EbreakException &eb) {
        TraceExecutedInstruction(decode_stage_instr.instr, decode_stage_instr.pc_de);
        DoLastTicks();
        throw eb;
    } catch (...) {
        DoLastTicks();
        throw;
    }

    // update next register
    auto control_signals = control_unit.GetControlSignals();
    execute_register.next.alu_op = control_signals.alu_op;
    execute_register.next.alu_src2 = control_signals.alu_src2;
    execute_register.next.is_jalr = control_signals.is_jalr;
    execute_register.next.wb_we = control_signals.wb_we;
    execute_register.next.mem_we = control_signals.mem_we;
    execute_register.next.mem_to_reg = control_signals.mem_to_reg;
    execute_register.next.brn_cond = control_signals.brn_cond;
    execute_register.next.jmp_cond = control_signals.jmp_cond;
    execute_register.next.cmp_control = control_signals.cmp_control;
    execute_register.next.funct3 = control_unit.funct3;
    // check if the instruction is AUIPC
    execute_register.next.data1 = modules::Multiplexer2<uint32_t>{}((control_unit.opcode == 0b0010111),
                                                                    reg_file.GetReadData1(),
                                                                    decode_stage_instr.pc_de);
    // check if the instruction is LUI
    execute_register.next.data1 = modules::Multiplexer2<uint32_t>{}((control_unit.opcode == 0b0110111),
                                                                    execute_register.next.data1,
                                                                    0);
    execute_register.next.data2 = reg_file.GetReadData2();
    execute_register.next.pc_de = decode_stage_instr.pc_de;
    execute_register.next.instr = decode_stage_instr.instr;
    execute_register.next.v_de = modules::Or<bool>{}(pc_r, decode_stage_instr.v_de);
}


void PipelineModel::DoExecute() {
    auto exec_reg_cur = execute_register.GetCurrent();
    auto src_a = modules::Multiplexer3<uint32_t>{}(static_cast<uint8_t>(hu_rs1),
                                                exec_reg_cur.data1,
                                                bp_mem,
                                                write_back_register.GetCurrent().wb_d);
    auto rs2v = modules::Multiplexer3<uint32_t>{}(static_cast<uint8_t>(hu_rs2),
                                               exec_reg_cur.data2,
                                               bp_mem,
                                               write_back_register.GetCurrent().wb_d);
    pc_disp = instruction::ImmediateExtensionBlock{}(exec_reg_cur.instr);
    auto src_b = modules::Multiplexer2<uint32_t>{}(exec_reg_cur.alu_src2,
                                                rs2v,
                                                pc_disp);
    bool cmp_res = modules::Cmp<uint32_t>{}(exec_reg_cur.cmp_control,
                                         src_a,
                                         rs2v);
    pc_r = modules::Or<bool>{}(exec_reg_cur.jmp_cond,
                               modules::And<bool>{}(cmp_res, exec_reg_cur.brn_cond));
    // update next registers
    // store_mode will be used by DMEM
    memory_register.next.store_mode = exec_reg_cur.funct3;
    memory_register.next.mem_we = exec_reg_cur.mem_we && (!exec_reg_cur.v_de);
    memory_register.next.mem_to_reg = exec_reg_cur.mem_to_reg;
    memory_register.next.wb_we = exec_reg_cur.wb_we && (!exec_reg_cur.v_de);
    memory_register.next.jmp_cond = exec_reg_cur.jmp_cond;
    memory_register.next.write_data = rs2v;
    memory_register.next.alu_res = modules::ALU{}(exec_reg_cur.alu_op, src_a, src_b);

    // writeback_address will be taken into account only if instruction writes in registers
    // (in out case, only if it's not store or branch)
    memory_register.next.wb_a = instruction::getRd(exec_reg_cur.instr);
    if (exec_reg_cur.is_jalr) {
        // if instruction is JALR, then pc_disp equals the result of ALU
        pc_disp = memory_register.next.alu_res;
        pc_disp &= 0xfffffffe;
    }
    if (exec_reg_cur.jmp_cond) {
        // write into rd the value pc + 4 for instructions JAL and JALR
        memory_register.next.alu_res = modules::Add<uint32_t>{}(exec_reg_cur.pc_de, 4);
    }
    decode_register.next.v_de = pc_r;

    TraceExecutedInstruction(exec_reg_cur.instr, exec_reg_cur.pc_de);
}


void PipelineModel::DoMemory() {
    auto mem_reg_cur = memory_register.GetCurrent();
    if (mem_reg_cur.mem_to_reg || mem_reg_cur.mem_we) { // if load or store
        data_mem_unit.store_mode = mem_reg_cur.store_mode;
        data_mem_unit.write_enable = mem_reg_cur.mem_we;
        data_mem_unit.address = mem_reg_cur.alu_res;
        data_mem_unit.write_data = mem_reg_cur.write_data;
        data_mem_unit.Tick();
    }
    // update next register
    write_back_register.next.wb_we = mem_reg_cur.wb_we;
    write_back_register.next.wb_d = modules::Multiplexer2<uint32_t>{}(mem_reg_cur.mem_to_reg,
                                                                      mem_reg_cur.alu_res,
                                                                      data_mem_unit.GetData());
    write_back_register.next.wb_a = mem_reg_cur.wb_a;
    bp_mem = mem_reg_cur.alu_res;
}


void PipelineModel::DoWriteBack() {}


void PipelineModel::HaltPipeline() {
    program_counter.enable_flag = false;
    decode_register.enable_flag = false;
    execute_register.enable_flag = false;
}

void PipelineModel::RestartPipeline() {
    program_counter.enable_flag = true;
    decode_register.enable_flag = true;
    execute_register.enable_flag = true;
}


void PipelineModel::HazardUnitTick() {
    if (pc_r) {
        // jump or branch. Must clear invalid stages
        decode_register.clear();
        execute_register.clear();
        return;
    }

    auto execute_stage_instr = execute_register.GetCurrent().instr;
    auto instr_type = control_unit.GetInstructionType();
    auto rs1 = instruction::getRs1(execute_stage_instr);
    auto rs2 = instruction::getRs2(execute_stage_instr);
    auto mem_reg_cur = memory_register.GetCurrent();
    auto wb_reg_cur = write_back_register.GetCurrent();

    bool was_halted = false;
    if (instruction::hasRs1(instr_type) && rs1 != 0) {
        if (rs1 == mem_reg_cur.wb_a && mem_reg_cur.wb_we) {
            hu_rs1 = BypassOptionsEncoding::MEM;
            if (mem_reg_cur.mem_to_reg) {
                // load conflict case
                HaltPipeline();
                was_halted = true;
            }
        } else if (rs1 == wb_reg_cur.wb_a && wb_reg_cur.wb_we) {
            hu_rs1 = BypassOptionsEncoding::WB;
        } else {
            hu_rs1 = BypassOptionsEncoding::REG;
        }
    } else {
        hu_rs1 = BypassOptionsEncoding::REG;
    }

    if (instruction::hasRs2(instr_type) && rs2 != 0) {
        if (rs2 == mem_reg_cur.wb_a && mem_reg_cur.wb_we) {
            hu_rs2 = BypassOptionsEncoding::MEM;
            if (mem_reg_cur.mem_to_reg) {
                // load conflict case
                HaltPipeline();
                was_halted = true;
            }
        } else if (rs2 == wb_reg_cur.wb_a && wb_reg_cur.wb_we) {
            hu_rs2 = BypassOptionsEncoding::WB;
        } else {
            hu_rs2 = BypassOptionsEncoding::REG;
        }
    } else {
        hu_rs2 = BypassOptionsEncoding::REG;
    }
    if (!was_halted) {
        // Changes the registers' state only if the pipeline was stalled before;
        // otherwise does nothing
        RestartPipeline();
    }
}

void PipelineModel::TickStateRegisters() {
    decode_register.Tick();
    execute_register.Tick();
    memory_register.Tick();
    write_back_register.Tick();
}

void PipelineModel::DoLastTicks() {
    memory_register.Tick();
    write_back_register.Tick();
    DoWriteBack();
    DoMemory();
    reg_file.SetNewSignals(0,
                           0,
                           write_back_register.GetCurrent().wb_a,
                           write_back_register.GetCurrent().wb_we,
                           write_back_register.GetCurrent().wb_d);
    reg_file.Tick();


    write_back_register.Tick();
    reg_file.SetNewSignals(0,
                           0,
                           write_back_register.GetCurrent().wb_a,
                           write_back_register.GetCurrent().wb_we,
                           write_back_register.GetCurrent().wb_d);
    reg_file.Tick();
}

runtime::ReturnCodes PipelineModel::Run() {
#ifndef REMOVE_TRACES
    trace_prev_pc_ = 0;
#endif
    try {
        while (true) {
            // this workaround prevents loading from invalid memory region on first cycles
            if (UNLIKELY(ticks_counter < 2)) {
                data_mem_unit.address = program_counter.GetCurrent();
            }

            Tick();
        }
    } catch (EcallException const&) {
        return runtime::ECALL;
    } catch (EbreakException const&) {
        return runtime::EBREAK;
    } catch (std::logic_error& e) {
        std::cerr << e.what() << std::endl;
        throw e;
    }
    return runtime::ERROR_NONE;
}

runtime::ReturnCodes PipelineModel::RunProgram(const char *path) {
    ticks_counter = 0;
    uint32_t entrypoint = 0;
    try {
        utils::Elf32Loader program_loader(path);
        entrypoint = program_loader.LoadElf32IntoMemory(mmu);
    } catch (utils::FileOpenException const &) {
        return runtime::FILE_OPEN_ERROR;
    } catch (utils::ElfLoadException const &) {
        return runtime::ELF_LOAD_ERROR;
    } catch (memory::AllocationException const &) {
        return runtime::MMU_ALLOC_ERROR;
    }
    SetPC(entrypoint);
    InitSP();

    auto ret_code = Run();
    mmu.FreeMemory();
    return ret_code;
}


#ifndef REMOVE_TRACES
void PipelineModel::TraceExecutedInstruction(uint32_t instr, uint32_t location) {
    if (instr != 0 && location != trace_prev_pc_) {
        trace::TraceWriter::GetWriter().TraceExecutedInstruction(readable_traces_,
                                                                 instr,
                                                                 location);
    }
    trace_prev_pc_ = location;
}
#endif

#ifdef DEBUG
void PipelineModel::Debug() {
    DEBUG_LOG(program_counter);
    DEBUG_LOG(decode_register);
    DEBUG_LOG(execute_register);
    DEBUG_LOG(memory_register);
    DEBUG_LOG(write_back_register);

    std::cout << "pc_redirect == " << pc_r << std::endl;
    std::cout << "hu_rs1 == " << hu_rs1 << std::endl;
    std::cout << "hu_rs2 == " << hu_rs2 << std::endl;

    DEBUG_LOG(instr_mem_unit);
    DEBUG_LOG(data_mem_unit);
    DEBUG_LOG(reg_file);
    DEBUG_LOG(control_unit);

    PrintRegisters();

    std::cout << "\n\n";
}
#endif
}   // end namespace pipeline
