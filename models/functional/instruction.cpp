#include "instruction.h"


namespace functional {
std::ostream &operator<<(std::ostream &os, RV32I instr) {
    os << instructions_names[static_cast<size_t>(instr)];
    return os;
}

void RV32ITypeR::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", r" << GetRs1() << ", r" << GetRs2();
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}

void RV32ITypeI::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", r" << GetRs1() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}

void RV32ITypeS::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRs1() << ", r" << GetRs2() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}

void RV32ITypeB::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRs1() << ", r" << GetRs2() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}

void RV32ITypeU::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", " << GetImm();
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}

void RV32ITypeJ::Dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << instr.raw << std::endl;
}
}   // end namespace functional
