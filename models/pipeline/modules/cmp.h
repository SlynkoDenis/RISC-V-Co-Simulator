#ifndef HW_CO_SIMULATION_CMP_H
#define HW_CO_SIMULATION_CMP_H

#include "common.h"


namespace modules {
enum class CmpControl : uint8_t {
    NOP,
    EQ,
    NE,
    LT,
    GE,
    LTU,
    GEU
};

inline std::ostream& operator<<(std::ostream& os, const CmpControl& cmp_control) {
    switch (cmp_control) {
        case CmpControl::NOP:
            os << "NOP";
            break;
        case CmpControl::EQ:
            os << "EQ";
            break;
        case CmpControl::NE:
            os << "NE";
            break;
        case CmpControl::LT:
            os << "LT";
            break;
        case CmpControl::GE:
            os << "GE";
            break;
        case CmpControl::LTU:
            os << "LTU";
            break;
        case CmpControl::GEU:
            os << "GEU";
            break;
    }
    return os;
}

// for signed/unsigned comparison
template <typename U, typename S>
constexpr inline bool Cmp(CmpControl control, U lhs, U rhs) {
    switch (control) {
        case CmpControl::NOP:
            return false;
        case CmpControl::EQ:
            return lhs == rhs;
        case CmpControl::NE:
            return lhs != rhs;
        case CmpControl::LT:
            return static_cast<S>(lhs) < static_cast<S>(rhs);
        case CmpControl::LTU:
            return lhs < rhs;
        case CmpControl::GE:
            return static_cast<S>(lhs) >= static_cast<S>(rhs);
        case CmpControl::GEU:
            return lhs >= rhs;
        default:
            throw std::logic_error("unknown compare operation");
    }
}
}   // end namespace modules

#endif // HW_CO_SIMULATION_CMP_H
