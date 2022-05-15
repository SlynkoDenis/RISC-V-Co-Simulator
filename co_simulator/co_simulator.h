#ifndef HW_CO_SIMULATION_CO_SIMULATOR_H
#define HW_CO_SIMULATION_CO_SIMULATOR_H

#include <array>
#include "macros.h"
#include "model.h"
#include "pipeline.h"
#include <sstream>
#include "system.h"


namespace cosimulator {
class CoSimulator {
public:
    CoSimulator() = default;
    DEFAULT_COPY_SEMANTIC(CoSimulator);
    DEFAULT_MOVE_SEMANTIC(CoSimulator);
    virtual ~CoSimulator() noexcept = default;

    virtual bool SetupBeforeTest() = 0;
    virtual void TeardownAfterTest() = 0;
    virtual int RunTest(const char *path) = 0;
    virtual int TestProgram(const char *path) {
        if (!SetupBeforeTest()) {
            return -1;
        }
        int ret_code = RunTest(path);
        TeardownAfterTest();
        return ret_code;
    }
};


enum class RV32ICoSimResult : int {
    SETUP_FAIL = -1,
    ERROR_NONE,
    FUNCTIONAL_FAILED,
    PIPELINE_FAILED,
    REGFILE_MISMATCH = 0b100,
    MMU_MISMATCH = 0b1000,
    EXEC_PATH_MISMATCH = 0b10000,
};

std::string CodeStringRepresentation(int code);

class RV32ICoSimulator : public CoSimulator {
public:
    RV32ICoSimulator(): should_delete_traces_(true), readable_traces_(false) {}
    DEFAULT_COPY_SEMANTIC(RV32ICoSimulator);
    DEFAULT_MOVE_SEMANTIC(RV32ICoSimulator);
    ~RV32ICoSimulator() noexcept override {}

    bool SetupBeforeTest() override;
    void TeardownAfterTest() override;
    int RunTest(const char *path) override;

    int CompareTraces() const;
    int CompareRegFileTraces() const;
    int CompareMMUTraces() const;
    int CompareExecutionPathTraces() const;

    void ShouldDeleteTraces(bool should_delete) { should_delete_traces_ = should_delete; }

    void MakeTracesReadable(bool readable) { readable_traces_ = readable; }

    static constexpr const char *TracesDir = "traces";
    static constexpr std::array<const char*, 3> FunctionalTraces = {"traces/mmu.funct.tr",
                                                                    "traces/regfile.funct.tr",
                                                                    "traces/decoder.funct.tr"};
    static constexpr std::array<const char*, 3> PipelineTraces = {"traces/mmu.pipe.tr",
                                                                  "traces/regfile.pipe.tr",
                                                                  "traces/decoder.pipe.tr"};
    static constexpr std::array<std::ios_base::openmode, trace::NumberOfTraceLevels> TraceFilesModes = \
        {std::ofstream::app | std::ofstream::binary};

private:
    template <typename Iterable>
    static bool SetupFiles(Iterable files) {
        for (const auto &name : files) {
            if (!utils::CreateNewFile(name)) {
                WARNING("failed to create " + std::string(name));
                return false;
            }
        }
        return true;
    }
    static bool SetupDirectory(const char *path);
    static bool RemoveDirectory(const char *path);

    static bool IsReturnCodeValid(runtime::ReturnCodes code);

    functional::FunctionalModel functional_model_;
    pipeline::PipelineModel pipeline_model_;
    bool should_delete_traces_;
    bool readable_traces_;
};
}   // end namespace cosimulator

#endif // HW_CO_SIMULATION_CO_SIMULATOR_H
