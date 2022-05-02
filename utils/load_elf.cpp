#include <cstring>
#include <sys/mman.h>
#include "load_elf.h"


namespace utils {
Elf::Elf(const char *path) {
    auto file = File{open(path, O_RDONLY)};
    if (!file.IsValid()) {
        throw FileOpenException("failed to open file " + std::string(path));
    }

	sz = file.GetFileSize();
    mapped_elf = mmap(NULL, sz, PROT_READ, MAP_PRIVATE, file.GetFd(), 0);
	if (mapped_elf == MAP_FAILED) {
        throw ElfLoadException("failed mmap");
    }
	if (file.Close()) {
		throw FileOpenException("failed to close file " + std::string(path));
	}
}

Elf::~Elf() noexcept {
	int r = munmap(mapped_elf, sz);
	if (r != 0) {
		WARNING("failed nunmap");
	}
}

const Elf32_Ehdr *Elf::GetElfHeader() const {
	return reinterpret_cast<Elf32_Ehdr *>(mapped_elf);
}

bool Elf::IsElfMagicCorrect(const unsigned char (*e_ident)[16]) {
	int cmp = memcmp(e_ident,
					 "\x7f"
	   	   			 "ELF",
	   	   			 4);
	return (cmp == 0);
}

bool Elf::IsMagicCorrect() const {
	const auto *elf_header = GetElfHeader();
	return IsElfMagicCorrect(&elf_header->e_ident);
}

bool Elf::IsLoadableSegment(Elf32_Word p_type) {
	return (p_type == PT_LOAD);
}

size_t Elf::GetProgramHeaderEntriesCount() const {
	ASSERT(IsMagicCorrect());
	const auto *elf_header = GetElfHeader();
	return static_cast<size_t>(elf_header->e_phnum);
}

Elf32_Phdr *Elf::GetProgramHeaderEntry(size_t idx) const {
	ASSERT(IsMagicCorrect());

	const auto *elf_header = GetElfHeader();
	auto ph_count = static_cast<size_t>(elf_header->e_phnum);
	auto ph_off = elf_header->e_phoff;
	auto ph_ent_size = elf_header->e_phentsize;

	ASSERT(idx < ph_count);
	return reinterpret_cast<Elf32_Phdr*>(reinterpret_cast<uint8_t*>(mapped_elf) + ph_off + ph_ent_size * idx);
}

ElfEntry Elf::GetElfEntry() const {
	if (!IsMagicCorrect()) {
		throw ElfLoadException("invalid elf magic");
	}

	const auto *elf_header = GetElfHeader();

	auto entry_va = elf_header->e_entry;
	auto ph_off = elf_header->e_phoff;
	auto ph_num = elf_header->e_phnum;
	auto ph_ent_size = elf_header->e_phentsize;

	Elf32_Phdr *to_load = nullptr;
	for (int i = 0; i < ph_num; ++i) {
		auto *ph = reinterpret_cast<Elf32_Phdr*>(reinterpret_cast<uint8_t*>(mapped_elf) + ph_off + ph_ent_size * i);
		if (ph->p_type != PT_LOAD) {
			continue;
        }
		if (ph->p_vaddr <= entry_va && ph->p_vaddr + ph->p_memsz > entry_va) {
			to_load = ph;
			break;
		}
	}
	if (!to_load) {
        throw ElfLoadException("failed to load");
    }

	ElfEntry elf_entry{reinterpret_cast<uint8_t*>(mapped_elf) + to_load->p_offset,
					   to_load->p_filesz,
					   entry_va - to_load->p_vaddr};
	return elf_entry;	
}

std::vector<uint32_t> GetInstructions(const ElfEntry &elf_entry) {
	ASSERT(elf_entry.ptr != nullptr);

	int sz_in_bytes = elf_entry.sz - elf_entry.entry;
	// check alignment
	ASSERT(sz_in_bytes > 0);
	ASSERT(sz_in_bytes % sizeof(uint32_t) == 0);
	unsigned instrs_count = sz_in_bytes / sizeof(uint32_t);

	std::vector<uint32_t> instructions(instrs_count);
	auto *entry_va = reinterpret_cast<uint32_t*>(elf_entry.ptr) + elf_entry.entry / 4;
	for (size_t i = 0; i < instrs_count; ++i) {
		instructions[i] = *(entry_va + i);
	}

	return instructions;
}

// TODO: remove this function, now it exists only for tests
void execute(const char *path) {
	std::cout << "**************** execute elf\n";

	auto elf = Elf(path);
	auto elf_entry = elf.GetElfEntry();

	auto instructions = GetInstructions(elf_entry);
	std::cout << "Got " << instructions.size() << " instructions" << std::endl;
	for (const auto &inst : instructions) {
		std::cout << std::hex << inst << std::endl;
	}

	std::cout << "****************\n";
}
}   // end namespace utils
