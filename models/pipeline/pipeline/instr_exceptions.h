#ifndef HW_CO_SIMULATION_INSTR_EXCEPTIONS_H
#define HW_CO_SIMULATION_INSTR_EXCEPTIONS_H

#include <stdexcept>


namespace pipeline {
class EcallException : public std::exception {
};

class EbreakException : public std::exception {
};

class InvalidInstructionException : public std::invalid_argument {
public:
    explicit InvalidInstructionException(const std::string& mes) : std::invalid_argument(mes) {}
    explicit InvalidInstructionException(const char *mes) : std::invalid_argument(mes) {}
};
}   // end namespace pipeline

#endif // HW_CO_SIMULATION_INSTR_EXCEPTIONS_H
