#ifndef HW_CO_SIMULATION_GATES_H
#define HW_CO_SIMULATION_GATES_H

#include <exception>
#include "common.h"


namespace modules {
template <typename T>
struct Add {
    constexpr inline T operator()(T lhs, T rhs) const {
        return lhs + rhs;
    }
};

template <typename T>
struct And {
    constexpr inline T operator()(T lhs, T rhs) const {
        return lhs & rhs;
    }
};

template <typename T>
struct Or {
    constexpr inline T operator()(T lhs, T rhs) const {
        return lhs | rhs;
    }
};

template <typename T>
struct Multiplexer2 {
    constexpr inline uint32_t operator()(bool control, T src1, T src2) const {
        return control ? src2 : src1;
    }
};

template <typename T>
struct Multiplexer3 {
    constexpr inline uint32_t operator()(uint8_t control, T src1, T src2, T src3) const {
        switch (control) {
        default:
            throw std::logic_error("invalid control signal: " + std::to_string(control));
        case 0:
            return src1;
        case 1:
            return src2;
        case 2:
            return src3;
        }
    }
};
}   // end namespace modules

#endif // HW_CO_SIMULATION_GATES_H
