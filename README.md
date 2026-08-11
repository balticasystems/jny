# jny

> The project's bare-metal RV64 bootloader. First code to run under QEMU's `virt` machine (no BIOS, no OpenSBI), it brings the hart up from reset, zeroes `.bss`, installs a trap vector, and hands off into C.

This is where the project stops being separate infrastructure repos (`env.sh`, `linker.ld`, `Makefile`, headers) and becomes an actual running system. `jny` doesn't vendor any of that infrastructure itself, it declares it as a dependency (see **Dependencies** below) and pulls it in, keeping this repo scoped to just the bootloader's own source.

## Boot sequence

Everything starts in `src/jny_init.s`, the only file placed in `.text.init` (see `linker.ld`), so it's guaranteed to be the first code QEMU jumps to:

```asm
_start:
    csrr t0, mhartid    # get the mhartid
    bnez t0, park_hart  # if it is not 0 disable
    la sp, _stack_top   # initialize the stack stack
    call jny_main       # jump into C part of the bootloader
park_hart:
    wfi
    j park_hart
trap_handler:
    wfi
    j trap_handler
```

1. **Read `mhartid`.** QEMU's `virt` machine can boot with multiple harts; this reads the current one's hardware ID via the `mhartid` CSR.
2. **Park every hart except 0.** Any hart that isn't hart 0 branches straight to `park_hart` and sits in a `wfi` (wait-for-interrupt) loop forever. Bring-up is single-hart only for now, this is the gate that enforces it.
3. **Set up the stack.** `sp` is loaded with `_stack_top`, a symbol `linker.ld` defines as the top of RAM. Nothing before this point may use the stack, there isn't one yet.
4. **Jump into C.** `call jny_main` hands off to `src/jny_core.c`, the first C code the bootloader runs.
5. **`trap_handler`** exists as a symbol (its address gets installed into `mtvec`, see below) but does nothing yet beyond parking in `wfi`, it's a placeholder so a trap doesn't fall through to undefined behavior.

From there, `jny_main()` in `src/jny_core.c` runs, in order:

- **`jny_logo()`** — prints an ASCII banner over UART. This is the first thing that proves the whole pipeline, build, linker layout, boot, and UART MMIO access, actually works end to end.
- **`jny_bss_init()`** — zeroes every byte between `_bss_start` and `_bss_end`, the symbols `linker.ld` exports for exactly this. The ELF format doesn't store bytes for `.bss`, only its size, so nothing else does this zeroing; skip it and every uninitialized global starts out as garbage.
- **`jny_data_init()`** — currently a no-op. QEMU's `-kernel` loads the ELF straight into RAM, so there's no ROM-to-RAM copy needed yet. The stub is there for later, once boot moves off `-kernel` and onto a real flash/ROM image, `.data` will need to be copied at this point.
- **`jny_mtvec_init()`** — writes `trap_handler`'s address into the `mtvec` CSR via the `CSRW` macro (`include/csr.h`), so any exception or interrupt from here on traps into our handler instead of an undefined one.
- **Poweroff (test only)** — writes `0x5555` to `0x100000`, QEMU `virt`'s SiFive test/poweroff MMIO device. This cleanly exits QEMU from inside the guest, it's a deliberate end-of-bring-up marker confirming the boot path completed, not a permanent behavior. The next milestone replaces this with continuing into a real kernel instead of shutting down.

## Dependencies

`jny` doesn't contain its own build tooling or linker script, it declares them in `puller.toml` and fetches them via [`pff`](https://github.com/Emilia-Systems/pff):

```toml
[dependencies]
repos = [
    ["Emilia-Systems", "csr.h"],
    ["Emilia-Systems", "extensions.h"],
    ["Emilia-Systems", "qemu_io.h"],
    ["Emilia-Systems", "Makefile"],
    ["Emilia-Systems", "linker.ld"],
]
```

| Dependency | Provides |
| --- | --- |
| [`csr.h`](https://github.com/Emilia-Systems/csr.h) | `CSRR`/`CSRW`/`CSRS`/`CSRC` macros and CSR name constants (`MTVEC`, `MHARTID`, ...) used to touch RISC-V control/status registers |
| [`extensions.h`](https://github.com/Emilia-Systems/extensions.h) | GCC attribute wrappers (`force_inline`, `section`, `packed`, ...) used across freestanding code |
| [`qemu_io.h`](https://github.com/Emilia-Systems/qemu_io.h) | Minimal polled UART driver (`put`/`puts`/`UART0`) against `virt`'s memory-mapped UART |
| [`Makefile`](https://github.com/Emilia-Systems/Makefile) | The canonical compile/link/run recipe |
| [`linker.ld`](https://github.com/Emilia-Systems/linker.ld) | Memory layout and the `_bss_start`/`_bss_end`/`_stack_top`/`trap_handler` symbols this code relies on |

Everything under `include/` and the root `Makefile`/`linker.ld` in this repo are fetched copies, not authored here, only `src/` is `jny`'s own code.

## Requirements

- The toolchain `env.sh` installs: `riscv64-elf-gcc`, `riscv64-elf-binutils`, `qemu-system-riscv`
- [`pff`](https://github.com/Emilia-Systems/pff), to pull the dependencies declared in `puller.toml`

## Usage

```
# pull declared dependencies (Makefile, linker.ld, headers) via pff
make compile   # build bin/main.elf
make run       # boot it in QEMU
```

Expected output is the `jny` ASCII banner over the serial console, followed by QEMU exiting (the deliberate poweroff-on-success step described above).

## Project structure

```
jny/
├── include/          # pulled headers (csr.h, extensions.h, qemu_io.h)
├── src/
│   ├── jny_init.s     # _start, hart parking, stack setup, trap_handler stub
│   └── jny_core.c     # jny_main: logo, bss init, data init, mtvec init, poweroff
├── linker.ld           # pulled
├── Makefile             # pulled
└── puller.toml         # dependency declaration for pff
```

## Known limitations (intentional, for this stage)

- **Single-hart only.** Every hart but 0 parks forever in `_start`, there's no SMP bring-up yet.
- **`trap_handler` is a placeholder.** It's installed into `mtvec` so traps don't go somewhere undefined, but it just spins in `wfi`, there's no real exception or interrupt dispatch yet.
- **`jny_data_init()` is a no-op.** Fine while QEMU's `-kernel` loads the ELF directly into RAM; will need a real ROM→RAM copy once boot moves off that.
- **Powers off on success.** The current "done" state is a clean QEMU exit, not a handoff into a kernel. That handoff is the next milestone.
- **No console input**, UART is transmit-only right now.

## Status

Early stage. Boots, prints its banner, zeroes `.bss`, installs a placeholder trap vector, then exits. Next up: real trap handling and the transition from bootloader into kernel space.