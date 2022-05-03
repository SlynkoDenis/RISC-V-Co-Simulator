#ifndef HW_CO_SIMULATION_LOAD_ELF_H
#define HW_CO_SIMULATION_LOAD_ELF_H

#include <elf.h>
#include <fcntl.h>
#include <stdexcept>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "macros.h"


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


class File final {
public:
    explicit File(int fd): fd_(fd) {}
    DEFAULT_COPY_SEMANTIC(File);
    DEFAULT_MOVE_SEMANTIC(File);
    DEFAULT_DTOR(File);

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

private:
    int fd_;
};


struct ElfEntry {
    ElfEntry(): ptr(nullptr), sz(0), entry(0) {}
    ElfEntry(void *ptr_n, size_t sz_n, size_t entry_n): ptr(ptr_n), sz(sz_n), entry(entry_n) {}
    DEFAULT_COPY_SEMANTIC(ElfEntry);
    DEFAULT_MOVE_SEMANTIC(ElfEntry);
    DEFAULT_DTOR(ElfEntry);

	void *ptr;
	size_t sz;      // size in bytes
	size_t entry;   // entry point offset in bytes
};

std::vector<uint32_t> GetInstructions(const ElfEntry &elf_entry);


class Elf final {
public:
    explicit Elf(const char *path);
    NO_COPY_SEMANTIC(Elf);
    DEFAULT_MOVE_SEMANTIC(Elf);
    ~Elf() noexcept;

    const Elf32_Ehdr *GetElfHeader() const;

    static bool IsElfMagicCorrect(const unsigned char (*e_ident)[16]);

    bool IsMagicCorrect() const;

    ElfEntry GetElfEntry() const;

private:
    void *mapped_elf;
    size_t sz;
};


std::vector<uint32_t> execute(const char *path);
}   // end namespace utils

#endif // HW_CO_SIMULATION_LOAD_ELF_H
