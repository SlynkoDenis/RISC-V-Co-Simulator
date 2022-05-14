#ifndef HW_CO_SIMULATION_PIPELINE_REGFILE_H
#define HW_CO_SIMULATION_PIPELINE_REGFILE_H

#include <array>
#include <exception>
#include "trace_writer.h"


namespace modules {
class RegFile {
public:
    virtual ~RegFile() noexcept = default;

    virtual void Tick() {
        if (write_enable3 && address3 != 0) {
#ifndef REMOVE_TRACES
            trace::TraceWriter::GetWriter().TraceSetRegFile(readable_traces_, "set ", 1,
                                                            static_cast<uint32_t>(address3),
                                                            write_data3);
#endif
            regs.at(address3) = write_data3;
        }
        read_data1 = regs.at(address1);
        read_data2 = regs.at(address2);
    }

    [[nodiscard]] uint32_t GetReadData1() const {
        return read_data1;
    }

    [[nodiscard]] uint32_t GetReadData2() const {
        return read_data2;
    }

    void SetNewSignals(uint8_t addr1, uint8_t addr2, uint8_t addr3, bool we3, uint32_t wd3) {
        address1 = addr1;
        address2 = addr2;
        address3 = addr3;
        write_enable3 = we3;
        write_data3 = wd3;
    }

    void InitSP(uint32_t value) {
#ifndef REMOVE_TRACES
        trace::TraceWriter::GetWriter().TraceSetRegFile(readable_traces_, "set ", 1,
                                                        static_cast<uint32_t>(SPReg),
                                                        value);
#endif
        regs.at(SPReg) = value;
    }

    void MakeTracesReadable(bool readable) { readable_traces_ = readable; }

    virtual void PrintRegisters() const {
        std::cout << "RegFile:\n";
        for (size_t i = 0; i < number_of_regs; ++i) {
            std::cout << 'x' << i << ":\t" << std::hex << regs[i] << " (" << std::dec << static_cast<int32_t>(regs[i]) << ")\n";
        }
    }

    uint32_t GetRegDirectly(uint32_t addr) const {
        return regs.at(addr);
    }

#ifdef DEBUG
    virtual void Debug() const {
        std::cout << "RegFile: read_data1=" << read_data1 << "; read_data2=" << read_data2;
        std::cout << "; write_enable3=" << write_enable3 << "; write_data3=" << write_data3;
        std::cout << "; address1=" << static_cast<uint32_t>(address1) << "; address2=";
        std::cout << static_cast<uint32_t>(address2) << "; address3=" << static_cast<uint32_t>(address3) << std::endl;
    }
#endif

    uint8_t address1 = 0;
    uint8_t address2 = 0;
    uint8_t address3 = 0;
    bool write_enable3 = false;
    uint32_t write_data3 = 0;

    static constexpr size_t number_of_regs = 32;
    static constexpr uint8_t SPReg = 2;

private:
    std::array<uint32_t, number_of_regs> regs = {0};
    uint32_t read_data1 = 0;
    uint32_t read_data2 = 0;
    bool readable_traces_ = false;
};
}   // end namespace modules

#endif // HW_CO_SIMULATION_PIPELINE_REGFILE_H
