#ifndef HW_CO_SIMULATION_COMMON_H
#define HW_CO_SIMULATION_COMMON_H

#include <cstdint>

#ifdef DEBUG
#define DEBUG_LOG(obj) std::cout << std::dec << __LINE__ << std::hex << ": "; obj.Debug()
#else
#define DEBUG_LOG(obj)
#endif


namespace modules {
constexpr inline uint32_t signExtendFromByte(uint8_t target) {
    if (target >> 7) {
        return static_cast<uint32_t>(target) | 0xffffff00;
    }
    return static_cast<uint32_t>(target);
}

constexpr inline uint32_t signExtendFromByte2(uint16_t target) {
    if (target >> 15) {
        return static_cast<uint32_t>(target) | 0xffff0000;
    }
    return static_cast<uint32_t>(target);
}
}   // end namespace modules

#endif // HW_CO_SIMULATION_COMMON_H
