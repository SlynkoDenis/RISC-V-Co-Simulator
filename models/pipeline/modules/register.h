#ifndef HW_CO_SIMULATION_REGISTER_H
#define HW_CO_SIMULATION_REGISTER_H

#include "common.h"


namespace modules {
template <typename T>
class Register {
public:
    virtual ~Register() noexcept = default;

    virtual void Tick() {
        if (enable_flag) {
            current = next;
        }
    }

    [[nodiscard]] T GetCurrent() const {
        return current;
    }

    virtual void clear() {
        current = 0;
        next = 0;
    }

    virtual void Debug() const {
        std::cout << "Register: enable_flag=" << enable_flag << "; next=" << next;
        std::cout << "; current=" << current << std::endl;
    }

    bool enable_flag = true;
    T next = 0;

private:
    T current = 0;
};
}   // end namespace modules

#endif // HW_CO_SIMULATION_REGISTER_H
