#ifndef HW_CO_SIMULATION_TRACE_WRITER_H
#define HW_CO_SIMULATION_TRACE_WRITER_H

#include <array>
#include <fstream>
#include <ios>
#include "macros.h"


namespace trace {
#ifdef WILL_TRACE
#define TRACE(expr) (__builtin_expect((expr) != 0, true))
#else
#define TRACE(expr) (__builtin_expect((expr) != 0, false))
#endif

enum class TraceLevel : uint8_t {
    MMU = 0,
    REG_FILE,
    DECODER,
};

static constexpr size_t NumberOfTraceLevels = static_cast<size_t>(TraceLevel::DECODER) + 1;


class TraceWriter final {
public:
    NO_COPY_SEMANTIC(TraceWriter);
    NO_MOVE_SEMANTIC(TraceWriter);
    ~TraceWriter() noexcept {
        CloseAll();
    }

    static TraceWriter &GetWriter() {
        static TraceWriter writer;
        return writer;
    }

    void TraceEnable() { must_trace_ = true; };
    void TraceDisable() { must_trace_ = false; };

    void CloseIfOpen(TraceLevel level) noexcept {
        auto &trace_os = trace_streams_[static_cast<size_t>(level)];
        if (trace_os.is_open()) {
            trace_os.close();
        }
    }

    void CloseAll() noexcept {
        for (auto &trace_os : trace_streams_) {
            if (trace_os.is_open()) {
                trace_os.close();
            }
        }
    }

    bool OpenTraceFile(TraceLevel level, const char *path, std::ios_base::openmode mode) {
        CloseIfOpen(level);
        auto &trace_os = trace_streams_[static_cast<size_t>(level)];
        trace_os.open(path, mode);
        return !trace_os.fail();
    }

    template<typename... T>
    void TraceIfEnabled(TraceLevel level, const T&... args) {
        if (TRACE(must_trace_)) {
            auto &trace_os = trace_streams_[static_cast<size_t>(level)];
            [[maybe_unused]] volatile int dummy[sizeof...(T)] = {(trace_os << args, 0)...};
            trace_os.flush();
        }
    }

    template <typename T>
    void TraceObjIfEnabled(TraceLevel level, const T &obj) {
        if (TRACE(must_trace_)) {
            auto &trace_os = trace_streams_[static_cast<size_t>(level)];
            obj.DumpImpl(trace_os);
            trace_os.flush();
        }
    }

private:
    TraceWriter(): must_trace_(false) {}

    std::array<std::ofstream, NumberOfTraceLevels> trace_streams_;
    bool must_trace_;
};
}   // end namespace trace

#endif // HW_CO_SIMULATION_TRACE_WRITER_H
