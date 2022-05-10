#include "co_simulator/co_simulator.h"
#include "utils/system.h"
#include "utils/load_elf.h"
#include "model.h"


int main(int argc, char **argv) {
    std::string path = "../examples/nested_loops/nested_loops.out";
    if (argc == 2) {
        path = argv[1];
    } else {
        WARNING("taking default example (nested_loops.out)");
    }

    pipeline::PipelineModel model;

    if (!utils::CreateDirectoryIfNotExists("traces")) {
        WARNING("failed to create trace directory");
        return -1;
    }
    std::array<const char*, 3> trace_files = {"traces/mmu.ptr", "traces/regfile.ptr", "traces/decoder.ptr"};
    for (const auto &name : trace_files) {
        if (!utils::CreateFileIfNotExists(name)) {
            WARNING("failed to create " + std::string(name));
            return -1;
        }
    }
    if (!model.EnableTraces(trace_files)) {
        WARNING("failed to create trace files");
        return -1;
    }

    auto ret_code = model.RunProgram(path.data());
    std::cout << "Return code: " << static_cast<int>(ret_code) << ret_code << std::endl;



    // cosimulator::RV32ICoSimulator cosim;
    // std::cout << cosim.TestProgram(path.data());



    // auto model = functional::FunctionalModel();

    // if (!utils::CreateDirectoryIfNotExists("traces")) {
    //     WARNING("failed to create trace directory");
    //     return -1;
    // }
    // std::array<const char*, 3> trace_files = {"traces/mmu.trace", "traces/regfile.trace", "traces/decoder.trace"};
    // for (const auto &name : trace_files) {
    //     if (!utils::CreateFileIfNotExists(name)) {
    //         WARNING("failed to create " + std::string(name));
    //         return -1;
    //     }
    // }

    // if (!model.EnableTraces(trace_files)) {
    //     WARNING("failed to create trace files");
    //     return -1;
    // }
    // auto ret_code = model.RunProgram(path.data());
    // std::cout << "Return code: " << static_cast<int>(ret_code) << ret_code << std::endl;

    return 0;
}
