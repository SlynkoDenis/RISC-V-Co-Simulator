#ifndef HW_CO_SIMULATION_INTERPRETER_H
#define HW_CO_SIMULATION_INTERPRETER_H

#include "macros.h"
#include "runtime.h"


namespace functional {
class RV32IInterpreter : public runtime::Runtime {
public:
    RV32IInterpreter() = default;
    DEFAULT_COPY_SEMANTIC(RV32IInterpreter);
    DEFAULT_MOVE_SEMANTIC(RV32IInterpreter);
    virtual ~RV32IInterpreter() noexcept = default;

    virtual void DumpState() const = 0;

    virtual runtime::ReturnCodes RunProgram(const char *path) = 0;

protected:
    virtual runtime::ReturnCodes Run() = 0;
};
}   // end namespace functional

#endif // HW_CO_SIMULATION_INTERPRETER_H
