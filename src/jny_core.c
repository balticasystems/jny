#include <stdint.h>

#include "../include/csr.h"
#include "../include/qemu_io.h"

// Data section addresses
extern uintptr_t _data_start;
extern uintptr_t _data_end;
// BSS section addresses
extern uintptr_t _bss_start;
extern uintptr_t _bss_end;
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

void jny_mtvec_init()
{
    CSRW(MTVEC, &trap_handler);
}

void jny_main()
{
    // Logo
    jny_logo();

    // Zeroing the BSS sections
    jny_bss_init();

    // Moving data from ROM to RAM
    jny_data_init();

    // Set the trap handler address
    jny_mtvec_init();

    // Success

    // Poweroff (test only)
    *power_switch = poweroff;
}