# RISC-V Co-Simulation

RISCV-V RV32I instruction set Co-Simulator. Project produces co-simulation using Interpreter and Cycle Models; co-simulation part compares executed instructions, writes into Register File and Memory.

Co-Simulation is conducted by comparing traces, which are produced by Models when running simulation; by default traces are stored in a newly created `traces` directory. By default traces are written in binary form and deleted after comparison; this can be changed using `MakeTracesReadable` and `ShouldDeleteTraces` methods respectively.

## Models

### Cycle Model

This model is conceptually based on pipeline from the previous [project](https://github.com/SlynkoDenis/RISC-V-Simulator). For compatability several changes were made:
* Model was changed to inherit from `Runtime` abstract class
* Model uses `MMUFixedOffset` module to enable directly loading from elf
* Model implements tracing in `RegFile` and `Execute` pipeline stage

When testing, several bugs (including critical bug with consequent `load; load; branch;` instructions pattern) were found and fixed.

### Functional Model

Dispatch table is used to implement the Interpreter. Current implementation is approximately<sup>(1)</sup> 230 MIPS. This speed is reached after building with `-DREMOVE_TRACES=True`, which removes conditional calls to `TraceWriter`.

## How to build
This project supports only Linux. After cloning repository, do:
* Run `git submodule update --init --recursive`
* In build directory run `cmake ../ -GNinja -DCMAKE_BUILD_TYPE=$BUILD_TYPE`, where `BUILD_TYPE` is either `Debug` (by default) or `Release`
* Then in build directory run `ninja` to build binaries
* Tests are built when build in `Debug` or when `-DWITH_TESTS=True` option was used
* For faster Models runtime build with `-DREMOVE_TRACES=True`, which removes code related to traces from Models
* You can also use `-DWILL_TRACE` with `True` or `False` value to give a hint about whether you are intended to use traces

Project generates `cycle`, `interpreter` and `cosim` binaries for Cycle Model, Functional Model and Co-Simulator respectively. All binaries are generated in `$BUILD_DIR/bin` directory.

## How to compile with riscv-gnu-toolchain

To compile a `C` source code (for compatibility with `Co-Simulator`) execute

```
riscv32-unknown-elf-gcc -march=rv32i -ffreestanding -fno-asynchronous-unwind-tables -O2 -fpic -fpie -static $SOURCE_FILES -o $BINARY_FILE
```

We implement only RV32I instructions set, which is reflected in `-march=rv32i` compilation option.

To disassemble a binary file execute
```
riscv32-unknown-elf-objdump -D $BINARY_FILE > $DISASM_FILE
```


<sub>(1) Results were obtained on device with: Intel(R) Core(TM) i5-8265U CPU @ 1.60GHz</sub>
