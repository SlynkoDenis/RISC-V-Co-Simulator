#include "decoder.h"
#include "load_elf.h"
#include "model.h"
#include "trace_writer.h"


namespace functional {
void FunctionalModel::DumpState() const {
    std::cout << "===================== Dump Model State =====================\n";
    registers.Dump();
    std::cout << "============================================================\n";
}

void FunctionalModel::TraceExecutedInstruction(uint32_t instr, uint32_t location) {
    if (readable_traces_) {
        trace::TraceWriter::GetWriter().TraceIfEnabled(trace::TraceLevel::DECODER,
                                                       instr, '(',
                                                       location, ")\n");
    } else {
        trace::TraceWriter::GetWriter().TraceIfEnabled(trace::TraceLevel::DECODER,
                                                       instr,
                                                       location);
    }
}

runtime::ReturnCodes FunctionalModel::RunProgram(const char *path) {
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

// TODO: refactor dump to make instructions appear according to the convention
#define DECODE_AND_GOTO(typed_instr)                        \
    DumpNamedInstruction(instr_name, typed_instr);          \
    ++ticks_counter;                                        \
    untyped_instr.instr.raw = LoadFromPC();                 \
    goto DECODER

runtime::ReturnCodes FunctionalModel::Run() {
    static std::array<const void*, instructions_names.size()> dispatch_table {
        &&HANDLE_LUI,
        &&HANDLE_AUIPC,
        &&HANDLE_JAL,
        &&HANDLE_JALR,
        &&HANDLE_BEQ,
        &&HANDLE_BNE,
        &&HANDLE_BLT,
        &&HANDLE_BGE,
        &&HANDLE_BLTU,
        &&HANDLE_BGEU,
        &&HANDLE_LB,
        &&HANDLE_LH,
        &&HANDLE_LW,
        &&HANDLE_LBU,
        &&HANDLE_LHU,
        &&HANDLE_SB,
        &&HANDLE_SH,
        &&HANDLE_SW,
        &&HANDLE_ADDI,
        &&HANDLE_SLTI,
        &&HANDLE_SLTIU,
        &&HANDLE_XORI,
        &&HANDLE_ORI,
        &&HANDLE_ANDI,
        &&HANDLE_SLLI,
        &&HANDLE_SRLI,
        &&HANDLE_SRAI,
        &&HANDLE_ADD,
        &&HANDLE_SUB,
        &&HANDLE_SLL,
        &&HANDLE_SLT,
        &&HANDLE_SLTU,
        &&HANDLE_XOR,
        &&HANDLE_SRL,
        &&HANDLE_SRA,
        &&HANDLE_OR,
        &&HANDLE_AND,
        &&HANDLE_FENCE,
        &&HANDLE_ECALL,
        &&HANDLE_EBREAK,
        &&HANDLE_UNKNOWN,
    };

    auto untyped_instr = RV32IInstruction{LoadFromPC()};
    RV32I instr_name = RV32I::UNKNOWN;
    RV32ITypeR r_instr{0};
    RV32ITypeI i_instr{0};
    RV32ITypeS s_instr{0};
    RV32ITypeB b_instr{0};
    RV32ITypeU u_instr{0};
    RV32ITypeJ j_instr{0};

    const void *label = 0;

    ++ticks_counter;

    try {
        // TODO: make debugger mode
        // std::cout << "Executing instruction #" << std::dec << ticks_counter << " (pc " << std::hex << pc << ")\n";
        // char c = 0;
        // while (c != 'c') {
        //     std::cin >> c;
        //     if (c == 'p') {
        //         DEBUG_OBJ_DUMP(registers);
        //     }
        //     if (c == 'k') {
        //         std::exit(0);
        //     }
        // }

DECODER:
        TraceExecutedInstruction(untyped_instr.instr.raw, pc);
        SWITCH_DECODER(instr_name, untyped_instr, r_instr, i_instr, s_instr, b_instr, u_instr, j_instr);
        label = dispatch_table[static_cast<uint8_t>(instr_name)];
        goto *label;

HANDLE_LUI: {
        registers.WriteReg(u_instr.GetImm(), u_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(u_instr);
}

HANDLE_AUIPC: {
        // TODO: can control overflow here
        registers.WriteReg(pc + u_instr.GetImm(), u_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(u_instr);
}

HANDLE_JAL: {
        // TODO: can control overflow here
        registers.WriteReg(pc + 4, j_instr.GetRd());
        pc += j_instr.GetImm();
        DECODE_AND_GOTO(j_instr);
}

HANDLE_JALR: {
        // TODO: raise machine exception if needed
        registers.WriteReg(pc + 4, i_instr.GetRd());
        pc = (static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) + static_cast<int32_t>(i_instr.GetImm())) & (0u - 2u);
        DECODE_AND_GOTO(i_instr);
}

HANDLE_BEQ: {
        pc = registers.ReadReg(b_instr.GetRs1()) == registers.ReadReg(b_instr.GetRs2()) ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_BNE: {
        pc = registers.ReadReg(b_instr.GetRs1()) != registers.ReadReg(b_instr.GetRs2()) ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_BLT: {
        bool changed_pc = static_cast<int32_t>(registers.ReadReg(b_instr.GetRs1())) < static_cast<int32_t>(registers.ReadReg(b_instr.GetRs2()));
        pc = changed_pc ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_BGE: {
        bool changed_pc = static_cast<int32_t>(registers.ReadReg(b_instr.GetRs1())) >= static_cast<int32_t>(registers.ReadReg(b_instr.GetRs2()));
        pc = changed_pc ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_BLTU: {
        bool changed_pc = registers.ReadReg(b_instr.GetRs1()) < registers.ReadReg(b_instr.GetRs2());
        pc = changed_pc ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_BGEU: {
        bool changed_pc = registers.ReadReg(b_instr.GetRs1()) >= registers.ReadReg(b_instr.GetRs2());
        pc = changed_pc ? pc + b_instr.GetImm() : pc + 4;
        DECODE_AND_GOTO(b_instr);
}

HANDLE_LB: {
        auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(static_cast<int32_t>(mmu.GetByte(addr)), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_LH: {
        auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(static_cast<int32_t>(mmu.GetHalf(addr)), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_LW: {
        auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(mmu.GetWord(addr), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_LBU: {
        auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(static_cast<uint32_t>(mmu.GetByte(addr)), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_LHU: {
        auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(static_cast<uint32_t>(mmu.GetHalf(addr)), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SB: {
        auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
        mmu.SetByte(static_cast<uint8_t>(registers.ReadReg(s_instr.GetRs2())), addr);
        pc += 4;
        DECODE_AND_GOTO(s_instr);
}

HANDLE_SH: {
        auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
        mmu.SetByte(static_cast<uint16_t>(registers.ReadReg(s_instr.GetRs2())), addr);
        pc += 4;
        DECODE_AND_GOTO(s_instr);
}

HANDLE_SW: {
        auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
        mmu.SetWord(registers.ReadReg(s_instr.GetRs2()), addr);
        pc += 4;
        DECODE_AND_GOTO(s_instr);
}

HANDLE_ADDI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SLTI: {
        uint32_t res = static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) < static_cast<int32_t>(i_instr.GetImm());
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SLTIU: {
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) < i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_XORI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) ^ i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_ORI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) | i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_ANDI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) & i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SLLI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) << i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SRLI: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(i_instr.GetRs1()) >> i_instr.GetImm();
        registers.WriteReg(res, i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_SRAI: {
        // TODO: raise machine exception if needed
        auto res = static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) >> i_instr.GetImm();
        registers.WriteReg(static_cast<uint32_t>(res), i_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_ADD: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) + registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SUB: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) - registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SLL: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) << registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SLT: {
        uint32_t res = static_cast<int32_t>(registers.ReadReg(r_instr.GetRs1())) < static_cast<int32_t>(registers.ReadReg(r_instr.GetRs2()));
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SLTU: {
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) < registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_XOR: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) ^ registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SRL: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) >> registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_SRA: {
        // TODO: raise machine exception if needed
        auto res = static_cast<int32_t>(registers.ReadReg(r_instr.GetRs1())) >> registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(static_cast<uint32_t>(res), r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_OR: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) | registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_AND: {
        // TODO: raise machine exception if needed
        uint32_t res = registers.ReadReg(r_instr.GetRs1()) & registers.ReadReg(r_instr.GetRs2());
        registers.WriteReg(res, r_instr.GetRd());
        pc += 4;
        DECODE_AND_GOTO(r_instr);
}

HANDLE_FENCE: {
        WARNING("executed FENCE instruction as NOP");
        pc += 4;
        DECODE_AND_GOTO(i_instr);
}

HANDLE_ECALL: {
        DumpNamedInstruction(instr_name, i_instr);
        throw ECALLException();
}

HANDLE_EBREAK: {
        DumpNamedInstruction(instr_name, i_instr);
        throw EBREAKException();
}

HANDLE_UNKNOWN: {
        UNREACHABLE("unknown instruction: " + std::to_string(untyped_instr.instr.raw));
}

    } catch (memory::AccessViolationException const&) {
        return runtime::SEGFAULT_ERROR;
    } catch (ECALLException const&) {
        return runtime::ECALL;
    } catch (EBREAKException const&) {
        return runtime::EBREAK;
    }
    //  catch (...) {
    //     DumpState();
    //     throw;
    // }
    // TODO: uncomment, print dump (and stacktrace?) and terminate
    return runtime::ERROR_NONE;
}

#undef DECODE_AND_GOTO
}   // end namespace functional
