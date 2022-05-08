#ifndef HW_CO_SIMULATION_MMU_FIXED_OFFSET_H
#define HW_CO_SIMULATION_MMU_FIXED_OFFSET_H

#include <sys/mman.h>
#include "mmu.h"
#include <string.h>
#include <utility>


namespace memory {
class AllocationException : std::runtime_error {
public:
    explicit AllocationException(const std::string& mes) : std::runtime_error(mes) {};
    explicit AllocationException(const char *mes) : std::runtime_error(mes) {};
};


class MMUFixedOffset : public MMU<uint32_t, uint8_t, uint8_t> {
public:
    MMUFixedOffset(): MMU(), va_allocated_range{NullSize, Size} {
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
        if (munmap(memory_, Size)) {
            WARNING("failed to unmap memory area");
        }
    }

    // TODO: memory address computations done by the hardware ignore overflow and instead wrap around
    uint32_t GetWord(uint32_t vaddr) const {
        return *reinterpret_cast<uint32_t*>(Translate(0, vaddr));
    }
    uint32_t &GetWord(uint32_t vaddr) {
        return *reinterpret_cast<uint32_t*>(Translate(0, vaddr));
    }
    uint16_t GetHalf(uint32_t vaddr) const {
        return *reinterpret_cast<uint16_t*>(Translate(0, vaddr));
    }
    uint16_t &GetHalf(uint32_t vaddr) {
        return *reinterpret_cast<uint16_t*>(Translate(0, vaddr));
    }
    uint8_t GetByte(uint32_t vaddr) const {
        return *(Translate(0, vaddr));
    }
    uint8_t &GetByte(uint32_t vaddr) {
        return *(Translate(0, vaddr));
    }

    void SetWord(uint32_t value, uint32_t vaddr) {
        *reinterpret_cast<uint32_t*>(Translate(0, vaddr)) = value;
    }
    void SetHalf(uint16_t value, uint32_t vaddr) {
        *reinterpret_cast<uint16_t*>(Translate(0, vaddr)) = value;
    }
    void SetByte(uint8_t value, uint32_t vaddr) {
        *(Translate(0, vaddr)) = value;
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
        void *paddr = mmap(Translate(0, aligned_vaddr), length, prot | PROT_WRITE,
                           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (paddr == MAP_FAILED) {
            throw AllocationException("failed to allocate memory");
        }
        return reinterpret_cast<uint8_t*>(paddr) + (vaddr - aligned_vaddr);
    }
    uint32_t AllocateStack() {
        // TODO: refactor it
        const uint32_t stack_size = PageSize * 100;
        const uint32_t stack_begin_vaddr = AlignDown(Size - stack_size);
        CheckPageAlignment(stack_begin_vaddr);
        CheckPageAlignment((uintptr_t)(memory_ + stack_begin_vaddr));
        void *paddr = mmap(Translate(0, stack_begin_vaddr), stack_size, PROT_READ | PROT_WRITE,
                           MAP_FIXED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (paddr == MAP_FAILED) {
            throw AllocationException("failed to allocate memory for stack");
        }
        return Size - 2 * PageSize;
    }
    uint32_t AlignUp(uint32_t vaddr) const override {
        return ((vaddr + (PageSize - 1)) / PageSize) * PageSize;
    }
    uint32_t AlignDown(uint32_t vaddr) const override {
        return vaddr - vaddr % PageSize;
    }
    void *Memset(uint32_t vaddr, int fill_byte, size_t n_bytes) override {
        return memset(Translate(0, vaddr), fill_byte, n_bytes);
    }

    static constexpr uint32_t Size = (0u - 1u);
    static constexpr uint32_t NullSize = 4096;
    static constexpr uint32_t PageSize = 4096;

protected:
    // implementation for one process at any time only
    inline uint8_t *Translate(uint8_t, uint32_t vaddr) const override {
        return memory_ + vaddr;
    }

    inline void CheckPageAlignment([[maybe_unused]] uint32_t val) {
        ASSERT(val % PageSize == 0);
    }

    // void ExceptIfAddressIllegal(uint32_t vaddr) const {
    //     if (vaddr < va_allocated_range.first || vaddr >= va_allocated_range.second) {
    //         throw AccessViolationException("segmentation fault");
    //     }
    // }

private:
    uint8_t *memory_;
    std::pair<uint32_t, uint32_t> va_allocated_range;
};
}   // end namespace memory

#endif // HW_CO_SIMULATION_MMU_FIXED_OFFSET_H

/*
questions: 4-7
сети Петри
*/
