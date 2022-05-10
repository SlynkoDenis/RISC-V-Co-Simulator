#include "co_simulator.h"


namespace cosimulator {
bool RV32ICoSimulator::SetupBeforeTest() {
    return SetupDirectory(TracesDir)
           && SetupFiles(FunctionalTraces)
           && SetupFiles(PipelineTraces)
           && functional_model_.EnableTraces(FunctionalTraces)
           && pipeline_model_.EnableTraces(PipelineTraces);
}

void RV32ICoSimulator::TeardownAfterTest() {
    // TODO: remove traces dir after run (use filesystem std library)
    functional_model_.DisableTraces();
    pipeline_model_.DisableTraces();
}

int RV32ICoSimulator::TestProgram(const char *path) {
    functional_model_.RunProgram(path);
    pipeline_model_.RunProgram(path);

    return 0;
}

bool RV32ICoSimulator::CompareTraces() const {
    // TODO: add other checks
    bool success = CompareRegFileTraces();
    return success;
}

bool RV32ICoSimulator::CompareRegFileTraces() const {
    std::ifstream funct_regfile(FunctionalTraces[1]);
    if (!funct_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(FunctionalTraces[1]));
        return false;
    }
    std::ifstream pipe_regfile(PipelineTraces[1]);
    if (!funct_regfile.is_open()) {
        WARNING("failed to open trace file " + std::string(PipelineTraces[1]));
        return false;
    }

    std::array<char, 100> funct_line{'\0'};
    std::array<char, 100> pipe_line{'\0'};
    int i = 0;
    while (funct_regfile.getline(funct_line.data(), 100) && pipe_regfile.getline(pipe_line.data(), 100)) {
        // TODO: implement comparison
        ++i;
    }
    return true;
}
}   // end namespace cosimulator
