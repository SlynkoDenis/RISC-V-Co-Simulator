#include "instruction.h"


namespace functional {
std::ostream &operator<<(std::ostream &os, RV32I instr) {
    os << instructions_names[static_cast<size_t>(instr)];
    return os;
}

// constexpr inline RV32IType getInstructionType([[maybe_unused]]uint32_t instr) {
//     return RV32IType::UNKNOWN;
// }

// TODO: delete it
// static constexpr inline uint32_t GetImmediate(uint32_t instr) {
//     auto instr_t = getInstructionType(instr);
//     if (instr_t == RV32IType::UNKNOWN) {
//         return 0;
//     }
//     union imm_layout {
//         uint32_t instr;
//         struct {
//             uint32_t pad0 : 20;
//             uint32_t imm0 : 12;
//         } __attribute__((packed)) instr_i_type;
//         struct {
//             uint32_t imm0 : 12;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_i_type;
//         struct {
//             uint32_t pad0 : 7;
//             uint32_t imm0 : 5;
//             uint32_t pad1 : 13;
//             uint32_t imm1 : 7;
//         } __attribute__((packed)) instr_s_type;
//         struct {
//             uint32_t imm0 : 5;
//             uint32_t imm1 : 7;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_s_type;
//         struct {
//             uint32_t pad0 : 7;
//             uint32_t imm2 : 1;
//             uint32_t imm0 : 4;
//             uint32_t pad2 : 13;
//             uint32_t imm1 : 6;
//             uint32_t sign : 1;
//         } __attribute__((packed)) instr_b_type;
//         struct {
//             uint32_t zero : 1;
//             uint32_t imm0 : 4;
//             uint32_t imm1 : 6;
//             uint32_t imm2 : 1;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_b_type;
//         struct {
//             uint32_t pad : 12;
//             uint32_t imm : 20;
//         } __attribute__((packed)) instr_u_type;
//         struct {
//             uint32_t zeros : 12;
//             uint32_t imm : 20;
//         } __attribute__((packed)) imm_u_type;
//         struct {
//             uint32_t pad0 : 12;
//             uint32_t imm2 : 8;
//             uint32_t imm1 : 1;
//             uint32_t imm0 : 10;
//             uint32_t sign : 1;
//         } __attribute__((packed)) instr_j_type;
//         struct {
//             uint32_t zero : 1;
//             uint32_t imm0 : 10;
//             uint32_t imm1 : 1;
//             uint32_t imm2 : 8;
//             uint32_t se : 12;
//         } __attribute__((packed)) imm_j_type;
//     } in{.instr = instr}, out{.instr = 0};
//     uint8_t sign = (in.instr >> 31) & 1;

//     switch (instr_t) {
//         case RV32IType::RType:
//             break;
//         case RV32IType::IType:
//             out.imm_i_type.imm0 = in.instr_i_type.imm0;
//             if (sign) {
//                 out.imm_i_type.se--;
//             }
//             break;
//         case RV32IType::SType:
//             out.imm_s_type.imm0 = in.instr_s_type.imm0;
//             out.imm_s_type.imm1 = in.instr_s_type.imm1;
//             if (sign) {
//                 out.imm_s_type.se--;
//             }
//             break;
//         case RV32IType::BType:
//             out.imm_b_type.imm0 = in.instr_b_type.imm0;
//             out.imm_b_type.imm1 = in.instr_b_type.imm1;
//             out.imm_b_type.imm2 = in.instr_b_type.imm2;
//             if (sign) {
//                 out.imm_b_type.se--;
//             }
//             break;
//         case RV32IType::JType:
//             out.imm_j_type.imm0 = in.instr_j_type.imm0;
//             out.imm_j_type.imm1 = in.instr_j_type.imm1;
//             out.imm_j_type.imm2 = in.instr_j_type.imm2;
//             if (sign) {
//                 out.imm_j_type.se--;
//             }
//             break;
//         case RV32IType::UType:
//             out.imm_u_type.imm = in.instr_u_type.imm;
//             break;
//         default:
//             throw std::logic_error("invalid instruction type: " + std::to_string(static_cast<uint8_t>(instr_t)));
//     }

//     return out.instr;
// }

void RV32ITypeR::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", r" << GetRs1() << ", r" << GetRs2();
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}

// constexpr inline uint32_t RV32ITypeI::GetImm() const {
//     union imm_layout {
//         uint32_t instr;
//         struct {
//             uint32_t pad0 : 20;
//             uint32_t imm0 : 12;
//         } __attribute__((packed)) instr_i_type;
//         struct {
//             uint32_t imm0 : 12;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_i_type;
//     } in{.instr = raw}, out{.instr = 0};
//     uint8_t sign = (in.instr >> 31) & 1;

//     out.imm_i_type.imm0 = in.instr_i_type.imm0;
//     if (sign) {
//         out.imm_i_type.se--;
//     }
//     return out.instr;
// }

void RV32ITypeI::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", r" << GetRs1() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}

// constexpr inline uint32_t RV32ITypeS::GetImm() const {
//     union imm_layout {
//         uint32_t instr;
//         struct {
//             uint32_t pad0 : 7;
//             uint32_t imm0 : 5;
//             uint32_t pad1 : 13;
//             uint32_t imm1 : 7;
//         } __attribute__((packed)) instr_s_type;
//         struct {
//             uint32_t imm0 : 5;
//             uint32_t imm1 : 7;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_s_type;
//     } in{.instr = raw}, out{.instr = 0};
//     uint8_t sign = (in.instr >> 31) & 1;

//     out.imm_s_type.imm0 = in.instr_s_type.imm0;
//     out.imm_s_type.imm1 = in.instr_s_type.imm1;
//     if (sign) {
//         out.imm_s_type.se--;
//     }
//     return out.instr;
// }

void RV32ITypeS::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRs1() << ", r" << GetRs2() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}

// constexpr inline uint32_t RV32ITypeB::GetImm() const {
//     union imm_layout {
//         uint32_t instr;
//         struct {
//             uint32_t pad0 : 7;
//             uint32_t imm2 : 1;
//             uint32_t imm0 : 4;
//             uint32_t pad2 : 13;
//             uint32_t imm1 : 6;
//             uint32_t sign : 1;
//         } __attribute__((packed)) instr_b_type;
//         struct {
//             uint32_t zero : 1;
//             uint32_t imm0 : 4;
//             uint32_t imm1 : 6;
//             uint32_t imm2 : 1;
//             uint32_t se : 20;
//         } __attribute__((packed)) imm_b_type;
//     } in{.instr = raw}, out{.instr = 0};
//     uint8_t sign = (in.instr >> 31) & 1;

//     out.imm_b_type.imm0 = in.instr_b_type.imm0;
//     out.imm_b_type.imm1 = in.instr_b_type.imm1;
//     out.imm_b_type.imm2 = in.instr_b_type.imm2;
//     if (sign) {
//         out.imm_b_type.se--;
//     }
//     return out.instr;
// }

void RV32ITypeB::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRs1() << ", r" << GetRs2() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}

void RV32ITypeU::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", " << GetImm();
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}

// constexpr inline uint32_t RV32ITypeJ::GetImm() const {
//     union imm_layout {
//         uint32_t instr;
//         struct {
//             uint32_t pad0 : 12;
//             uint32_t imm2 : 8;
//             uint32_t imm1 : 1;
//             uint32_t imm0 : 10;
//             uint32_t sign : 1;
//         } __attribute__((packed)) instr_j_type;
//         struct {
//             uint32_t zero : 1;
//             uint32_t imm0 : 10;
//             uint32_t imm1 : 1;
//             uint32_t imm2 : 8;
//             uint32_t se : 12;
//         } __attribute__((packed)) imm_j_type;
//     } in{.instr = raw}, out{.instr = 0};
//     uint8_t sign = (in.instr >> 31) & 1;

//     out.imm_j_type.imm0 = in.instr_j_type.imm0;
//     out.imm_j_type.imm1 = in.instr_j_type.imm1;
//     out.imm_j_type.imm2 = in.instr_j_type.imm2;
//     if (sign) {
//         out.imm_j_type.se--;
//     }
//     return out.instr;
// }

void RV32ITypeJ::dump() const {
    std::cout << std::dec;
    std::cout << 'r' << GetRd() << ", " << static_cast<int32_t>(GetImm());
    // TODO: print leading zeros
    std::cout << '\t' << std::hex << raw << std::endl;
}
}   // end namespace functional
