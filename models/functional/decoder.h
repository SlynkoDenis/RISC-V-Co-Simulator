#ifndef HW_CO_SIMULATION_DECODER_H
#define HW_CO_SIMULATION_DECODER_H

#include "instruction.h"
#include "macros.h"


namespace functional {
struct RV32IDecodeResult {
    RV32IInstruction *instr;
    RV32I name;

    void Dump() const {
        std::cout << name << ' ';
        DEBUG_POINTER_DUMP(instr);
    }
};

RV32IDecodeResult Decode(uint32_t raw_instr);
}   // end namespace functional

#endif // HW_CO_SIMULATION_DECODER_H
