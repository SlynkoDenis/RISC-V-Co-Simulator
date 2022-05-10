#ifndef HW_CO_SIMULATION_RUNTIME_H
#define HW_CO_SIMULATION_RUNTIME_H

#include "macros.h"
#include "trace_writer.h"


namespace runtime {
static constexpr std::array<const char*, 8> return_codes_names{
    "ERROR_NONE",
    "FILE_OPEN_ERROR",
    "ELF_LOAD_ERROR",
    "MMU_INIT_ERROR",
    "MMU_ALLOC_ERROR",
    "SEGFAULT_ERROR",
    "ECALL",
    "EBREAK",
};

enum ReturnCodes : int {
    ERROR_NONE = 0,
    FILE_OPEN_ERROR,
    ELF_LOAD_ERROR,
    MMU_INIT_ERROR,
    MMU_ALLOC_ERROR,
    SEGFAULT_ERROR,
    ECALL,
    EBREAK,
};

inline std::ostream &operator<<(std::ostream &os, ReturnCodes code) {
    os << '(' << return_codes_names[static_cast<size_t>(code)] << ')';
    return os;
}


class Runtime {
public:
    Runtime() = default;
    DEFAULT_COPY_SEMANTIC(Runtime);
    DEFAULT_MOVE_SEMANTIC(Runtime);
    virtual ~Runtime() noexcept = default;

    virtual ReturnCodes RunProgram(const char *path) = 0;

    virtual bool EnableTraces(const std::array<const char*, trace::NumberOfTraceLevels> &paths) const {
        auto &writer = trace::TraceWriter::GetWriter();
        writer.TraceEnable();
        bool success = true;
        for (size_t i = 0; i < trace::NumberOfTraceLevels; ++i) {
            success &= writer.OpenTraceFile(static_cast<trace::TraceLevel>(i), paths[i]);
        }
        writer.TraceIfEnabled(trace::TraceLevel::REG_FILE, std::hex);
        writer.TraceIfEnabled(trace::TraceLevel::MMU, std::hex);
        return success;
    }
    virtual void DisableTraces() const {
        auto &writer = trace::TraceWriter::GetWriter();
        writer.TraceDisable();
        writer.CloseAll();
    }
};
}   // end namespace runtime

#endif // HW_CO_SIMULATION_RUNTIME_H
