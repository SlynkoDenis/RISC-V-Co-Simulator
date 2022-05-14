#include "pipeline.h"


int main(int argc, char **argv) {
    std::string path = "../examples/nested_loops/nested_loops.out";
    if (argc == 2) {
        path = argv[1];
    } else {
        std::cerr << "taking default example (nested_loops)" << std::endl;
    }

    pipeline::PipelineModel model;
    auto ret_code = model.RunProgram(path.data());
    std::cout << "Return Code: " << static_cast<int>(ret_code) << ' ' << ret_code << std::endl;
    std::cout << "Took cycles: " << model.GetTicksCounter() << std::endl;

    return 0;
}
