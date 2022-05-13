#ifndef HW_CO_SIMULATION_LOAD_ELF_H
#define HW_CO_SIMULATION_LOAD_ELF_H

#include <elf.h>
#include <fcntl.h>
#include "file.h"
#include "macros.h"
#include "mmu.h"
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>


namespace utils {

class ElfLoadException : public std::runtime_error {
public:
    explicit ElfLoadException(const std::string& mes) : std::runtime_error(mes) {};
    explicit ElfLoadException(const char *mes) : std::runtime_error(mes) {};
};


class Elf32Loader : protected File {
public:
    explicit Elf32Loader(const File &file): File(file), elf_header{},
                                            header_was_init(false),
                                            table_was_init(false) {
        if (!IsValid()) {
            throw FileOpenException("invalid file");
        }
    }
    explicit Elf32Loader(const char *path): File(open(path, O_RDONLY)), elf_header{},
                                            header_was_init(false),
                                            table_was_init(false) {
        if (!IsValid()) {
            throw FileOpenException("failed to open");
        }
    }
    NO_COPY_SEMANTIC(Elf32Loader);
    DEFAULT_MOVE_SEMANTIC(Elf32Loader);
    ~Elf32Loader() noexcept override {
        Close();
    };

    static bool ValidateElfHeader(const Elf32_Ehdr &elf_header);
    bool ValidateElfHeader() const {
        return ValidateElfHeader(elf_header);
    }
    const Elf32_Ehdr &ReadElfHeader();
    const std::vector<Elf32_Phdr> &ReadProgramHeaderTable();
    uint32_t LoadElf32IntoMemory(memory::MMU<uint32_t, uint8_t> &mmu);

private:
    Elf32_Ehdr elf_header;
    bool header_was_init;
    std::vector<Elf32_Phdr> ph_table;
    bool table_was_init;
};
}   // end namespace utils

#endif // HW_CO_SIMULATION_LOAD_ELF_H
