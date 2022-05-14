#ifndef HW_CO_SIMULATION_MMU_FIXED_OFFSET_H
#define HW_CO_SIMULATION_MMU_FIXED_OFFSET_H

#include <cstdarg>
#include <sys/mman.h>
#include "mmu.h"
#include <string.h>
#include "trace_writer.h"
#include <utility>
#include <vector>


namespace memory {
class AllocationException : std::runtime_error {
public:
    explicit AllocationException(const std::string& mes) : std::runtime_error(mes) {};
    explicit AllocationException(const char *mes) : std::runtime_error(mes) {};
};


class MMUFixedOffset : public MMU<uint32_t, uint8_t> {
public:
    MMUFixedOffset(): MMU(), readable_traces_(false) {
        memory_ = reinterpret_cast<uint8_t*>(mmap(NULL, Size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
        if (memory_ == MAP_FAILED) {
            WARNING("memory_ == MAP_FAILED");
            throw MMUInitException("failed to create memory area");
        }
        if (munmap(memory_ + NullSize, Size - NullSize)) {
            WARNING("munmap(memory_ + NullSize, Size - NullSize)");
            throw MMUInitException("failed to create memory area");
        }
    }
    NO_COPY_SEMANTIC(MMUFixedOffset);
    DEFAULT_MOVE_SEMANTIC(MMUFixedOffset);
    ~MMUFixedOffset() noexcept override {
        FreeMemory();
        if (munmap(memory_, NullSize)) {
            WARNING("failed to unmap memory area");
        }
    }

    void MakeTracesReadable(bool readable) { readable_traces_ = readable; }

    uint32_t GetWord(uint32_t vaddr) const {
        return *reinterpret_cast<uint32_t*>(Translate(vaddr));
    }
    uint16_t GetHalf(uint32_t vaddr) const {
        return *reinterpret_cast<uint16_t*>(Translate(vaddr));
    }
    uint8_t GetByte(uint32_t vaddr) const {
        return *(Translate(vaddr));
    }

    void SetWord(uint32_t value, uint32_t vaddr) {
#ifndef REMOVE_TRACES
        trace::TraceWriter::GetWriter().TraceSetMemory(readable_traces_, "setw ",
                                                       2, vaddr, value);
#endif
        *reinterpret_cast<uint32_t*>(Translate(vaddr)) = value;
    }
    void SetHalf(uint16_t value, uint32_t vaddr) {
#ifndef REMOVE_TRACES
        trace::TraceWriter::GetWriter().TraceSetMemory(readable_traces_, "seth ",
                                                       1, vaddr, value);
#endif
        *reinterpret_cast<uint16_t*>(Translate(vaddr)) = value;
    }
    void SetByte(uint8_t value, uint32_t vaddr) {
#ifndef REMOVE_TRACES
        trace::TraceWriter::GetWriter().TraceSetMemory(readable_traces_, "setb ",
                                                       0, vaddr, value);
#endif
        *(Translate(vaddr)) = value;
    }

    uint8_t *AllocateMemory(uint32_t vaddr, size_t length, int prot) override {
        auto aligned_vaddr = AlignDown(vaddr);
        length = AlignUp(length + vaddr - aligned_vaddr);
        if (Size - length < vaddr) {
            throw OutOfMemoryException("failed to allocate " +
                                       std::to_string(length) +
                                       " bytes starting at " +
                                       std::to_string(vaddr));
        }

        void *paddr = mmap(Translate(aligned_vaddr), length, prot | PROT_WRITE,
                           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (paddr == MAP_FAILED) {
            throw AllocationException("failed to allocate memory");
        }
        allocated_chunks_.emplace_back(paddr, length);

        return reinterpret_cast<uint8_t*>(paddr) + (vaddr - aligned_vaddr);
    }
    uint32_t AllocateStack() {
        // TODO: refactor it
        const uint32_t stack_size = PageSize * 100;
        const uint32_t stack_begin_vaddr = AlignDown(Size - stack_size);

        void *paddr = mmap(Translate(stack_begin_vaddr), stack_size, PROT_READ | PROT_WRITE,
                           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (paddr == MAP_FAILED) {
            throw AllocationException("failed to allocate memory for stack");
        }
        allocated_chunks_.emplace_back(paddr, stack_size);

        return Size - 2 * PageSize;
    }

    uint32_t AlignUp(uint32_t vaddr) const override {
        return ((vaddr + (PageSize - 1)) / PageSize) * PageSize;
    }

    uint32_t AlignDown(uint32_t vaddr) const override {
        return vaddr - vaddr % PageSize;
    }

    void *Memset(uint32_t vaddr, int fill_byte, size_t n_bytes) override {
        return memset(Translate(vaddr), fill_byte, n_bytes);
    }

    void FreeMemory() override {
        for (const auto &it : allocated_chunks_) {
            if (munmap(it.first, it.second)) {
                WARNING("failed to unmap memory area");
            }
        }
        allocated_chunks_.clear();
    }

    static constexpr uint32_t Size = (0u - 1u);
    static constexpr uint32_t NullSize = 4096;
    static constexpr uint32_t PageSize = 4096;

protected:
    // implementation for one process at any time only
    inline uint8_t *Translate(uint32_t vaddr) const override {
        return memory_ + vaddr;
    }

private:
    uint8_t *memory_;
    std::vector<std::pair<void*, size_t>> allocated_chunks_;
    bool readable_traces_;
};
}   // end namespace memory

#endif // HW_CO_SIMULATION_MMU_FIXED_OFFSET_H
