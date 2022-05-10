#ifndef HW_CO_SIMULATION_DATA_MEM_UNIT_H
#define HW_CO_SIMULATION_DATA_MEM_UNIT_H

#include "address_exceptions.h"
#include "common.h"
#include <iostream>
#include "macros.h"
#include "mmu_fixed_offset.h"


namespace modules {
enum class StoreMode : uint8_t {
    Byte = 0,
    Halfword = 1,
    Word = 2
};

inline std::ostream& operator<<(std::ostream& os, const StoreMode& store_mode) {
    switch (store_mode) {
        case StoreMode::Byte:
            os << "Byte";
            break;
        case StoreMode::Halfword:
            os << "Halfword";
            break;
        case StoreMode::Word:
            os << "Word";
            break;
    }
    return os;
}

enum class LoadExtensionMode : uint8_t {
    Byte = 0,
    Halfword = 1,
    Word = 2,
    ByteU = 4,
    HalfwordU = 5
};

inline std::ostream& operator<<(std::ostream& os, const LoadExtensionMode& load_mode) {
    switch (load_mode) {
        case LoadExtensionMode::Byte:
            os << "Byte";
            break;
        case LoadExtensionMode::Halfword:
            os << "Halfword";
            break;
        case LoadExtensionMode::Word:
            os << "Word";
            break;
        case LoadExtensionMode::ByteU:
            os << "ByteU";
            break;
        case LoadExtensionMode::HalfwordU:
            os << "HalfwordU";
            break;
    }
    return os;
}


class DataMemUnit {
public:
    DataMemUnit(): store_mode(0), write_enable(false), address(0), write_data(0), mmu_(nullptr), read_data(0) {}
    explicit DataMemUnit(memory::MMUFixedOffset *mmu): store_mode(0), write_enable(false),
                                                       address(0), write_data(0), mmu_(mmu), read_data(0) {}
    NO_COPY_SEMANTIC(DataMemUnit);
    DEFAULT_MOVE_SEMANTIC(DataMemUnit);
    virtual ~DataMemUnit() noexcept = default;

    virtual void Tick() {
        if (write_enable) {
            StoreValue();
            // read_data modification is useless, as we don't read after write
        } else {
            read_data = GetData();
        }
    }

    uint32_t GetData() {
        auto load_mode = static_cast<LoadExtensionMode>(store_mode);
        switch (load_mode) {
        default:
            throw std::logic_error("invalid load mode: " + std::to_string(store_mode));
        case LoadExtensionMode::Byte:
            return signExtendFromByte(mmu_->GetByte(address));
        case LoadExtensionMode::Halfword:
            return signExtendFromByte2(mmu_->GetHalf(address));
        case LoadExtensionMode::Word:
            return mmu_->GetWord(address);
        case LoadExtensionMode::ByteU:
            return static_cast<uint32_t>(mmu_->GetByte(address));
        case LoadExtensionMode::HalfwordU:
            return static_cast<uint32_t>(mmu_->GetHalf(address));
        }
    }

#ifdef DEBUG
    virtual void Debug() const {
        std::cout << "DataMemUnit: read_data=" << read_data << "; write_enable=";
        std::cout << write_enable << "; address=" << address << "; store_mode=";
        std::cout << static_cast<StoreMode>(store_mode) << "; load_mode=";
        std::cout << static_cast<LoadExtensionMode>(store_mode) << std::endl;
    }
#endif

    uint8_t store_mode;
    bool write_enable;
    uint32_t address;
    uint32_t write_data;

private:
    void StoreValue() {
        auto store_mode_enum = static_cast<StoreMode>(store_mode);
        switch (store_mode_enum) {
            default:
                throw std::logic_error("invalid store mode: " + std::to_string(store_mode));
            case StoreMode::Byte:
                mmu_->SetByte(static_cast<uint8_t>(write_data), address);
                break;
            case StoreMode::Halfword:
                mmu_->SetHalf(static_cast<uint16_t>(write_data), address);
                break;
            case StoreMode::Word:
                mmu_->SetWord(write_data, address);
                break;
        }
    }

    memory::MMUFixedOffset *mmu_;
    uint32_t read_data;
};
}   // end namespace modules

#endif //HW_CO_SIMULATION_DATA_MEM_UNIT_H
