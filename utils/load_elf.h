#ifndef HW_CO_SIMULATION_LOAD_ELF_H
#define HW_CO_SIMULATION_LOAD_ELF_H

#include <elf.h>
#include <fcntl.h>
#include "macros.h"
#include "mmu.h"
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>


namespace utils {
class FileOpenException : public std::runtime_error {
public:
    explicit FileOpenException(const std::string& mes) : std::runtime_error(mes) {};
    explicit FileOpenException(const char *mes) : std::runtime_error(mes) {};
};


class ElfLoadException : public std::runtime_error {
public:
    explicit ElfLoadException(const std::string& mes) : std::runtime_error(mes) {};
    explicit ElfLoadException(const char *mes) : std::runtime_error(mes) {};
};


class File {
public:
    explicit File(int fd): fd_(fd) {}
    DEFAULT_COPY_SEMANTIC(File);
    DEFAULT_MOVE_SEMANTIC(File);
    virtual ~File() noexcept = default;

    int Close() const {
        return close(fd_);
    }

    size_t GetFileSize() const {
        struct stat st{};
        int r = fstat(fd_, &st);
        if (r != 0) {
            throw FileOpenException("failed fstat");
        }
        return static_cast<size_t>(st.st_size);
    }

    bool IsValid() const {
        return fd_ != -1;
    }

    int GetFd() const {
        return fd_;
    }

protected:
    int fd_;
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
    uint32_t LoadElf32IntoMemory(memory::MMU<uint32_t, uint8_t, uint8_t> &mmu);

private:
    Elf32_Ehdr elf_header;
    bool header_was_init;
    std::vector<Elf32_Phdr> ph_table;
    bool table_was_init;
};
}   // end namespace utils

#endif // HW_CO_SIMULATION_LOAD_ELF_H
