#include <stdint.h>

#include "../include/csr.h"
#include "../include/qemu_io.h"
#include "../include/extensions.h"

// Data section addresses
extern uintptr_t _data_start;
extern uintptr_t _data_end;
// BSS section addresses
extern uintptr_t _bss_start;
extern uintptr_t _bss_end;
// Trap stack top address
extern uintptr_t _trap_stack_top;
// Trap handler address
extern uintptr_t trap_handler;

// Poweroff (test only)
volatile uint32_t* power_switch = (volatile uint32_t*)0x100000;
static uint32_t poweroff = 0x5555;

void jny_logo()
{
    UART0(" __");
    UART0("/\\_\\    ___   __  __");
    UART0("\\/\\ \\ /' _ `\\/\\ \\/\\ \\");
    UART0(" \\ \\ \\/\\ \\/\\ \\ \\ \\_\\ \\");
    UART0(" _\\ \\ \\ \\_\\ \\_\\/`____ \\");
    UART0("/\\ \\_\\ \\/_/\\/_/`/___/> \\");
    UART0("\\ \\____/          /\\___/");
    UART0(" \\/___/           \\/__/");
    UART0("                __                      __    ___                        __");
    UART0("               /\\ \\                    /\\ \\__/\\_ \\                      /\\ \\");
    UART0("               \\ \\ \\____    ___     ___\\ \\ ,_\\//\\ \\     ___      __     \\_\\ \\     __   _ __");
    UART0("                \\ \\ '__`\\  / __`\\  / __`\\ \\ \\/ \\ \\ \\   / __`\\  /'__`\\   /'_` \\  /'__`\\/\\`'__\\");
    UART0("                 \\ \\ \\L\\ \\/\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_ \\_\\ \\_/\\ \\L\\ \\/\\ \\L\\.\\_/\\ \\L\\ \\/\\  __/\\ \\ \\/");
    UART0("                  \\ \\_,__/\\ \\____/\\ \\____/\\ \\__\\/\\____\\ \\____/\\ \\__/.\\_\\ \\___,_\\ \\____\\\\ \\_\\");
    UART0("                   \\/___/  \\/___/  \\/___/  \\/__/\\/____/\\/___/  \\/__/\\/_/\\/__,_ /\\/____/ \\/_/");
}

void jny_bss_init()
{
    uintptr_t* curr = &_bss_start;
    while (curr < &_bss_end)
    {
        *curr = 0;
        curr++;
    }
}

void jny_data_init()
{
    /* Skip; Not needed yet */
}

void jny_mscratch_init()
{
    CSRW(MSCRATCH, &_trap_stack_top);
}

void jny_mtvec_init()
{
    CSRW(MTVEC, &trap_handler);
}

typedef struct {
    uint64_t x1;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t x8;
    uint64_t x9;
    uint64_t x10;
    uint64_t x11;
    uint64_t x12;
    uint64_t x13;
    uint64_t x14;
    uint64_t x15;
    uint64_t x16;
    uint64_t x17;
    uint64_t x18;
    uint64_t x19;
    uint64_t x20;
    uint64_t x21;
    uint64_t x22;
    uint64_t x23;
    uint64_t x24;
    uint64_t x25;
    uint64_t x26;
    uint64_t x27;
    uint64_t x28;
    uint64_t x29;
    uint64_t x30;
    uint64_t x31;
    uint64_t mcause;
    uint64_t mepc;
} frame_t;

#define CAUSE_BIT (1ULL << 63)

void jny_dispatcher(frame_t* frame)
{
    switch (frame->mcause & CAUSE_BIT)
    {
    case 0:                 // Exception path
        break;
    case 1:                 // Interrupt path
        break;
    default:
        unreachable();
        break;
    }
}

void jny_main()
{
    // Logo
    jny_logo();

    // Zeroing the BSS sections
    jny_bss_init();

    // Moving data from ROM to RAM
    jny_data_init();

    // Set the trap stack top
    jny_mscratch_init();

    // Set the trap handler address
    jny_mtvec_init();

    // Success

    // Poweroff (test only)
    *power_switch = poweroff;
}