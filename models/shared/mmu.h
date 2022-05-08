#ifndef HW_CO_SIMULATION_MMU_H
#define HW_CO_SIMULATION_MMU_H

#include "macros.h"


namespace memory {
class MMUInitException : std::runtime_error {
public:
    explicit MMUInitException(const std::string& mes) : std::runtime_error(mes) {};
    explicit MMUInitException(const char *mes) : std::runtime_error(mes) {};
};

class OutOfMemoryException : std::runtime_error {
public:
    explicit OutOfMemoryException(const std::string& mes) : std::runtime_error(mes) {};
    explicit OutOfMemoryException(const char *mes) : std::runtime_error(mes) {};
};

class AccessViolationException : std::runtime_error {
public:
    explicit AccessViolationException(const std::string& mes) : std::runtime_error(mes) {};
    explicit AccessViolationException(const char *mes) : std::runtime_error(mes) {};
};


template <typename VAType, typename PAType, typename PIDType>
class MMU {
public:
    MMU() = default;
    NO_COPY_SEMANTIC(MMU);
    DEFAULT_MOVE_SEMANTIC(MMU);
    virtual ~MMU() noexcept = default;

    virtual PAType *AllocateMemory(VAType vaddr, size_t length, int prot) = 0;
    virtual VAType AlignUp(VAType vaddr) const = 0;
    virtual VAType AlignDown(VAType vaddr) const = 0;
    virtual void *Memset(VAType vaddr, int fill_byte, size_t n_bytes) = 0;

protected:
    virtual PAType *Translate(PIDType pid, VAType vaddr) const = 0;
};
}   // end namespace memory

#endif // HW_CO_SIMULATION_MMU_H
