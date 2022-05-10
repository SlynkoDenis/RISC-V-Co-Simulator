#ifndef HW_CO_SIMULATION_ADDRESS_EXCEPTIONS_H
#define HW_CO_SIMULATION_ADDRESS_EXCEPTIONS_H

#include <stdexcept>


namespace modules {
class AlignmentException : public std::invalid_argument {
public:
    explicit AlignmentException(const std::string& mes) : std::invalid_argument(mes) {}
    explicit AlignmentException(const char *mes) : std::invalid_argument(mes) {}
};
}   // end namespace modules

#endif // HW_CO_SIMULATION_ADDRESS_EXCEPTIONS_H
