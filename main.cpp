#include "co_simulator/co_simulator.h"
#include "utils/system.h"
#include "utils/load_elf.h"
#include "model.h"


int main(int argc, char **argv) {
    std::string path = "../examples/nested_loops/nested_loops.out";
    if (argc == 2) {
        path = argv[1];
    } else {
        std::cerr << "taking default example (nested_loops)" << std::endl;
    }

    cosimulator::RV32ICoSimulator cosim;
    cosim.ShouldKeepTraces();
    auto ret_code = cosim.TestProgram(path.data());
    std::cout << "Co-Simulation result: " << ret_code << ' ' << cosimulator::CodeStringRepresentation(ret_code) << std::endl;

    return 0;
}
