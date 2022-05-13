#include "load_elf.h"
#include "model.h"
#include "trace_writer.h"


namespace functional {
void FunctionalModel::DumpState() const {
    std::cout << "===================== Dump Model State =====================\n";
    registers.Dump();
    std::cout << "============================================================\n";
}

runtime::ReturnCodes FunctionalModel::Run() {
    try {
        while (true) {
            ++ticks_counter;

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

            auto raw_instr = LoadFromPC();
            auto decoded_instr = Decode(raw_instr);
            cur_instr.reset(decoded_instr.instr);

            // TODO: refactor dump to make instructions appear according to the convention
            DEBUG_OBJ_DUMP(decoded_instr);


            trace::TraceWriter::GetWriter().TraceExecutedInstruction(readable_traces_,
                                                                     decoded_instr.instr->instr.raw,
                                                                     pc);

            if (Handle(*cur_instr, decoded_instr.name)) {
                pc += 4;
            }
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

bool FunctionalModel::HandleLUI(RV32IInstruction &instr) {
    auto u_instr = dynamic_cast<RV32ITypeU&>(instr);
    registers.WriteReg(u_instr.GetImm(), u_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleAUIPC(RV32IInstruction &instr) {
    auto u_instr = dynamic_cast<RV32ITypeU&>(instr);
    // TODO: can control overflow here
    registers.WriteReg(pc + u_instr.GetImm(), u_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleJAL(RV32IInstruction &instr) {
    auto j_instr = dynamic_cast<RV32ITypeJ&>(instr);
    // TODO: can control overflow here
    registers.WriteReg(pc + 4, j_instr.GetRd());
    pc += j_instr.GetImm();
    return false;
}

bool FunctionalModel::HandleJALR(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    registers.WriteReg(pc + 4, i_instr.GetRd());
    pc = (static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) + static_cast<int32_t>(i_instr.GetImm())) & (0u - 2u);
    return false;
}

bool FunctionalModel::HandleBEQ(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = registers.ReadReg(b_instr.GetRs1()) == registers.ReadReg(b_instr.GetRs2());
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleBNE(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = registers.ReadReg(b_instr.GetRs1()) != registers.ReadReg(b_instr.GetRs2());
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleBLT(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = static_cast<int32_t>(registers.ReadReg(b_instr.GetRs1())) < static_cast<int32_t>(registers.ReadReg(b_instr.GetRs2()));
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleBGE(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = static_cast<int32_t>(registers.ReadReg(b_instr.GetRs1())) >= static_cast<int32_t>(registers.ReadReg(b_instr.GetRs2()));
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleBLTU(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = registers.ReadReg(b_instr.GetRs1()) < registers.ReadReg(b_instr.GetRs2());
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleBGEU(RV32IInstruction &instr) {
    auto &b_instr = dynamic_cast<RV32ITypeB&>(instr);
    bool changed_pc = registers.ReadReg(b_instr.GetRs1()) >= registers.ReadReg(b_instr.GetRs2());
    pc = changed_pc ? pc + b_instr.GetImm() : pc;
    return !changed_pc;
}

bool FunctionalModel::HandleLB(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(static_cast<int32_t>(mmu.GetByte(addr)), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleLH(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(static_cast<int32_t>(mmu.GetHalf(addr)), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleLW(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(mmu.GetWord(addr), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleLBU(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(static_cast<uint32_t>(mmu.GetByte(addr)), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleLHU(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    auto addr = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(static_cast<uint32_t>(mmu.GetHalf(addr)), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSB(RV32IInstruction &instr) {
    auto &s_instr = dynamic_cast<RV32ITypeS&>(instr);
    auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
    mmu.SetByte(static_cast<uint8_t>(registers.ReadReg(s_instr.GetRs2())), addr);
    return true;
}

bool FunctionalModel::HandleSH(RV32IInstruction &instr) {
    auto &s_instr = dynamic_cast<RV32ITypeS&>(instr);
    auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
    mmu.SetByte(static_cast<uint16_t>(registers.ReadReg(s_instr.GetRs2())), addr);
    return true;
}

bool FunctionalModel::HandleSW(RV32IInstruction &instr) {
    auto &s_instr = dynamic_cast<RV32ITypeS&>(instr);
    auto addr = registers.ReadReg(s_instr.GetRs1()) + s_instr.GetImm();
    mmu.SetWord(registers.ReadReg(s_instr.GetRs2()), addr);
    return true;
}

bool FunctionalModel::HandleADDI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) + i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLTI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    uint32_t res = static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) < static_cast<int32_t>(i_instr.GetImm());
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLTIU(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) < i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleXORI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) ^ i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleORI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) | i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleANDI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) & i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLLI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) << i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSRLI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(i_instr.GetRs1()) >> i_instr.GetImm();
    registers.WriteReg(res, i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSRAI(RV32IInstruction &instr) {
    auto &i_instr = dynamic_cast<RV32ITypeI&>(instr);
    // TODO: raise machine exception if needed
    auto res = static_cast<int32_t>(registers.ReadReg(i_instr.GetRs1())) >> i_instr.GetImm();
    registers.WriteReg(static_cast<uint32_t>(res), i_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleADD(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) + registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSUB(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) - registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLL(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) << registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLT(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    uint32_t res = static_cast<int32_t>(registers.ReadReg(r_instr.GetRs1())) < static_cast<int32_t>(registers.ReadReg(r_instr.GetRs2()));
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSLTU(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) < registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleXOR(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) ^ registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSRL(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) >> registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleSRA(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    auto res = static_cast<int32_t>(registers.ReadReg(r_instr.GetRs1())) >> registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(static_cast<uint32_t>(res), r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleOR(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) | registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleAND(RV32IInstruction &instr) {
    auto &r_instr = dynamic_cast<RV32ITypeR&>(instr);
    // TODO: raise machine exception if needed
    uint32_t res = registers.ReadReg(r_instr.GetRs1()) & registers.ReadReg(r_instr.GetRs2());
    registers.WriteReg(res, r_instr.GetRd());
    return true;
}

bool FunctionalModel::HandleFENCE([[maybe_unused]] RV32IInstruction &instr) {
    WARNING("executed FENCE instruction");
    return true;
}

bool FunctionalModel::HandleECALL([[maybe_unused]] RV32IInstruction &instr) {
    throw ECALLException();
    return true;
}

bool FunctionalModel::HandleEBREAK([[maybe_unused]] RV32IInstruction &instr) {
    throw EBREAKException();
    return true;
}
}   // end namespace functional
