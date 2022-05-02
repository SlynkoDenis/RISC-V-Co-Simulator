#include "utils/load_elf.h"


int main() {
    const char *path = "../examples/nested_loops/nested_loops.out";
    // auto instructions = utils::GetInstructions(path);

    utils::execute(path);

    return 0;
}
