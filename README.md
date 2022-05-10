# RISC-V Co-Simulation

TBD

### How to compile with riscv-gnu-toolchain

To compile a `C` source code (for compatibility with `Co-Simulator`) execute

```
riscv32-unknown-elf-gcc -march=rv32i -ffreestanding -fno-asynchronous-unwind-tables -O2 -fpic -fpie -static $SOURCE_FILES -o $BINARY_FILE
```

We implement only RV32I instructions set, which is reflected in `-march=rv32i` compilation option.

To disassemble a binary file execute
```
riscv32-unknown-elf-objdump -D $BINARY_FILE > $DISASM_FILE
```
