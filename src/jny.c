#include <stdint.h>

#include "../include/csr.h"

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

void jny_main()
{
    // Zeroing the BSS sections
    uintptr_t* curr = &_bss_start;
    while (curr < &_bss_end)
    {
        *curr = 0;
        curr++;
    }

    // Moving data from ROM to RAM
    // Not needed rn

    // Set the trap handler address
    CSRW(MTVEC, &trap_handler);

    // Success

    // Poweroff (test only)
    *power_switch = poweroff;
}