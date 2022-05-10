#ifndef HW_CO_SIMULATION_HAZARD_UNIT_H
#define HW_CO_SIMULATION_HAZARD_UNIT_H

#include "instruction.h"
#include "common.h"


namespace pipeline {
enum class BypassOptionsEncoding : uint8_t {
    REG,
    MEM,
    WB
};

inline std::ostream& operator<<(std::ostream& os, const BypassOptionsEncoding& enc) {
    if (enc == BypassOptionsEncoding::REG) {
        os << "REG";
    } else if (enc == BypassOptionsEncoding::MEM) {
        os << "MEM";
    } else {
        os << "WB";
    }
    return os;
}
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_HAZARD_UNIT_H
