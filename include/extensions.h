/*
 * compiler.h
 *
 * GCC-specific macros wrapping compiler attributes and builtins used
 * throughout the kernel. Centralizes GNU C extensions relied on for
 * freestanding RV64I development (struct layout control, linker
 * placement, trap handler generation, and optimizer hints) so call
 * sites read as plain, self-documenting keywords instead of raw
 * __attribute__/__builtin_ syntax.
 *
 * Target: GCC only (no Clang/MSVC compatibility attempted).
 * Architecture: RISC-V rv64i.
 * Environment: freestanding (-ffreestanding, no hosted libc).
 *
 * Notes:
 *   - `interrupt(mode)` expects one of int_machine / int_supervisor /
 *     int_rnmi as its argument, matching RISC-V's three privilege
 *     modes for trap handling.
 *   - `force_inline` mixes `always_inline` with the `inline` keyword;
 *     use on internal, non-recursive helpers only — GCC will hard
 *     error if the function genuinely can't be inlined.
 *   - `noclone` should be paired with `noinline` on any function using
 *     __builtin_return_address/__builtin_frame_address, or on naked
 *     functions with hand-written prologues, to keep the compiler
 *     from generating specialized clones that break those assumptions.
 */

#pragma once

// Enables -Wformat checking on printf-style functions; fmt_idx/va_idx are 1-based (va_idx 0 for va_list variants)
#define format(fmt_idx, va_idx) __attribute__((format(printf, fmt_idx, va_idx)))

// Removes the padding from the struct
#define packed          __attribute__((packed))

// Aligns the value/struct to n-byte boundaries
#define align(n)        __attribute__((aligned(n)))

// Places the symbol in a specific linker section
#define section(name)   __attribute__((section(name)))

// Explicits that the function never returns
#define noreturn        __attribute__((noreturn))

// Prevents the linker from discarding the symbol even if unreferenced
#define used            __attribute__((used))

// Allows the symbol to be overridden by another definition elsewhere
#define weak            __attribute__((weak))

// Function body is raw assembly; compiler emits no prologue/epilogue
#define naked           __attribute__((naked))

// Hints this function is frequently executed (optimize for speed)
#define hot             __attribute__((hot))

// Hints this function is rarely executed (optimize for size, out of hot path)
#define cold            __attribute__((cold))

// Prevents the compiler from inlining this function
#define noinline        __attribute__((noinline))

// Forces inlining regardless of optimization level (does not touch the `inline` keyword)
#define force_inline    __attribute__((always_inline)) inline

// Generates trap-correct prologue/epilogue for the given privilege mode
// (machine, supervisor, or rnmi); returns via mret/sret as appropriate
#define int_machine     "machine"
#define int_supervisor  "supervisor"
#define int_rnmi        "rnmi"
#define interrupt(mode) __attribute__((interrupt(mode)))

// Prevents IPA cloning — needed alongside noinline for functions using
// __builtin_return_address / __builtin_frame_address, or naked functions
#define noclone         __attribute__((noclone))

// Branch prediction hints
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

// Marks an intentional switch-case fallthrough; silences -Wimplicit-fallthrough (C23)
#define fallthrough     [[fallthrough]]

// Marks a code path as unreachable, letting the compiler optimize accordingly
#define unreachable()   __builtin_unreachable()