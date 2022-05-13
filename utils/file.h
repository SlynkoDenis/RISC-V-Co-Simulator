#ifndef HW_CO_SIMULATION_FILE_H
#define HW_CO_SIMULATION_FILE_H

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
}   // end namespace utils

#endif // HW_CO_SIMULATION_FILE_H
