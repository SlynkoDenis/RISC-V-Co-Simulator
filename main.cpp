#include "utils/load_elf.h"
#include "model.h"


int main(int argc, char **argv) {
    std::string path = "../examples/nested_loops/nested_loops.out";
    if (argc == 2) {
        path = argv[1];
    } else {
        WARNING("taking default example (nested_loops.out)");
    }

    auto model = functional::FunctionalModel();
    auto ret_code = model.RunProgram(path.data());
    std::cout << "Return code: " << static_cast<int>(ret_code) << ret_code << std::endl;

    return 0;
}
