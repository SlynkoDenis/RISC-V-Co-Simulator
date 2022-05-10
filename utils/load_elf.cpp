#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include "load_elf.h"


namespace utils {
bool Elf32Loader::ValidateElfHeader(const Elf32_Ehdr &elf_header) {
    int cmp = memcmp(&elf_header.e_ident,
                     "\x7f"
                     "ELF",
                     4);
    return (cmp == 0)
           && (elf_header.e_type == ET_EXEC)
           && (elf_header.e_machine == EM_RISCV);
}


const Elf32_Ehdr &Elf32Loader::ReadElfHeader() {
    if (!header_was_init) {
        if (pread(fd_, &elf_header, sizeof(elf_header), 0) != sizeof(elf_header)) {
            throw ElfLoadException("failed to read ELF header");
        }
        if (!ValidateElfHeader()) {
            throw ElfLoadException("invalid ELF header");
        }
        header_was_init = true;
    }
    return elf_header;
}


const std::vector<Elf32_Phdr> &Elf32Loader::ReadProgramHeaderTable() {
    if (!table_was_init) {
        ph_table.resize(elf_header.e_phnum);
        long int table_bytes_size = sizeof(Elf32_Phdr) * elf_header.e_phnum;
        if (pread(fd_, ph_table.data(), table_bytes_size, elf_header.e_phoff) != table_bytes_size) {
            throw ElfLoadException("failed to load Program Header Table");
        }
        table_was_init = true;
    }
    return ph_table;
}


uint32_t Elf32Loader::LoadElf32IntoMemory(memory::MMU<uint32_t, uint8_t> &mmu) {
    ReadElfHeader();
    ReadProgramHeaderTable();
    for (const auto &p_header : ph_table) {
        if (p_header.p_type != PT_LOAD && p_header.p_memsz == 0) {   // must be loadable
            continue;
        }

        int prot = PROT_NONE;
        if (p_header.p_flags & PF_R) {
            prot |= PROT_READ;
        }
        if (p_header.p_flags & PF_W) {
            prot |= PROT_WRITE;
        }
        if (p_header.p_flags & PF_X) {
            prot |= PROT_EXEC;
        }

        auto vaddr = p_header.p_vaddr;
        auto mem_size = p_header.p_memsz;
        auto file_size = p_header.p_filesz;
        auto *paddr = mmu.AllocateMemory(vaddr, mem_size, prot);

        if (pread(fd_, paddr, file_size, p_header.p_offset) != file_size) {
            WARNING("failed to copy segment into memory");
            throw ElfLoadException("failed to copy segment into memory");
        }
        if (mem_size > file_size) {
            mmu.Memset(vaddr + file_size, 0, mem_size - file_size);
        }
    }
    return elf_header.e_entry;
}
}   // end namespace utils
