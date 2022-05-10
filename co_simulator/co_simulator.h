#ifndef HW_CO_SIMULATION_CO_SIMULATOR_H
#define HW_CO_SIMULATION_CO_SIMULATOR_H

#include <array>
#include "macros.h"
#include "model.h"
#include "pipeline.h"
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

    virtual int TestProgram(const char *path) = 0;
};


class RV32ICoSimulator : public CoSimulator {
public:
    RV32ICoSimulator() = default;
    DEFAULT_COPY_SEMANTIC(RV32ICoSimulator);
    DEFAULT_MOVE_SEMANTIC(RV32ICoSimulator);
    ~RV32ICoSimulator() noexcept override {};

    bool SetupBeforeTest() override;
    void TeardownAfterTest() override;

    int TestProgram(const char *path) override;

    bool CompareTraces() const;
    bool CompareRegFileTraces() const;

    static constexpr const char *TracesDir = "traces";
    static constexpr std::array<const char*, 3> FunctionalTraces = {"traces/mmu.funct.tr",
                                                                    "traces/regfile.funct.tr",
                                                                    "traces/decoder.funct.tr"};
    static constexpr std::array<const char*, 3> PipelineTraces = {"traces/mmu.pipe.tr",
                                                                  "traces/regfile.pipe.tr",
                                                                  "traces/decoder.pipe.tr"};

private:
    template <typename Iterable>
    static bool SetupFiles(Iterable files) {
        for (const auto &name : files) {
            if (!utils::CreateFileIfNotExists(name)) {
                WARNING("failed to create " + std::string(name));
                return false;
            }
        }
        return true;
    }

    static bool SetupDirectory(const char *path) {
        if (!utils::CreateDirectoryIfNotExists(path)) {
            WARNING("failed to mkdir " + std::string(path));
            return false;
        }
        return true;
    }

    functional::FunctionalModel functional_model_;
    pipeline::PipelineModel pipeline_model_;
};
}   // end namespace cosimulator

#endif // HW_CO_SIMULATION_CO_SIMULATOR_H
