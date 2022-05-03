#include "utils/load_elf.h"
#include "model.h"


int main(int argc, char **argv) {
    std::string path = "../examples/nested_loops/nested_loops.out";
    if (argc == 2) {
        WARNING("taking default example (nested_loops.out)");
        path = argv[1];
    }
    // auto instructions = utils::GetInstructions(path);

    auto instructions = utils::execute(path.data());
    std::cout << std::endl;

    auto model = functional::FunctionalModel();
    model.LoadIntoMemory(instructions, 0);
    model.SetPC(0);
    model.Run();

    return 0;
}
