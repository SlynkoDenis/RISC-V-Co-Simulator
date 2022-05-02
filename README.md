# RISC-V Co-Simulation

TBD

### How to compile with riscv-gnu-toolchain

To compile a `C` source code for compatibility with `Co-Simulator` execute

```
riscv32-unknown-linux-gnu-gcc -ffreestanding -march=rv32im -mabi=ilp32 -O2 -fpic -fpie -nostartfiles -nolibc -static $SOURCE_FILES
```

To disassemble a binary file execute
```
riscv32-unknown-linux-gnu-objdump -d $BINARY_FILE
```
