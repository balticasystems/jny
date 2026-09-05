#define DEBUG

#include <stdint.h>

#include "../include/csr.h"
#include "../include/qemu.h"
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

extern uintptr_t _boot_origin;
extern uintptr_t _boot_end;
extern uintptr_t _kernel_origin;
extern uintptr_t _kernel_end;

void jny_panic(const char* msg, uint64_t mepc, uint64_t mcause, uint64_t code)
{
    uart_write_string("---- PANIC ----\n");
    uart_write_string("bootloader panic: "); uart_write_string(msg); uart_write_string("\n");
    uart_write_string("mepc= "); uart_write_hex(mepc); uart_write_string("\t");
    uart_write_string("mcause= "); uart_write_hex(mcause); uart_write_string("\n");
    uart_write_string("---------------\n");

    // Exit non-zero from qemu
    SIFIVE_TEST_FAIL(code); 
}

void jny_logo()
{
    uart_write_string("\n");
    uart_write_string(" __\n");
    uart_write_string("/\\_\\    ___   __  __\n");
    uart_write_string("\\/\\ \\ /' _ `\\/\\ \\/\\ \\\n");
    uart_write_string(" \\ \\ \\/\\ \\/\\ \\ \\ \\_\\ \\\n");
    uart_write_string(" _\\ \\ \\ \\_\\ \\_\\/`____ \\\n");
    uart_write_string("/\\ \\_\\ \\/_/\\/_/`/___/> \\\n");
    uart_write_string("\\ \\____/          /\\___/\n");
    uart_write_string(" \\/___/           \\/__/\n");
    uart_write_string("                __                      __    ___                        __\n");
    uart_write_string("               /\\ \\                    /\\ \\__/\\_ \\                      /\\ \\\n");
    uart_write_string("               \\ \\ \\____    ___     ___\\ \\ ,_\\//\\ \\     ___      __     \\_\\ \\     __   _ __\n");
    uart_write_string("                \\ \\ '__`\\  / __`\\  / __`\\ \\ \\/ \\ \\ \\   / __`\\  /'__`\\   /'_` \\  /'__`\\/\\`'__\\\n");
    uart_write_string("                 \\ \\ \\L\\ \\/\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_ \\_\\ \\_/\\ \\L\\ \\/\\ \\L\\.\\_/\\ \\L\\ \\/\\  __/\\ \\ \\/\n");
    uart_write_string("                  \\ \\_,__/\\ \\____/\\ \\____/\\ \\__\\/\\____\\ \\____/\\ \\__/.\\_\\ \\___,_\\ \\____\\\\ \\_\\\n");
    uart_write_string("                   \\/___/  \\/___/  \\/___/  \\/__/\\/____/\\/___/  \\/__/\\/_/\\/__,_ /\\/____/ \\/_/\n");
    uart_write_string("\n");
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

#ifdef DEBUG
    uint64_t check; CSRR(check, MSCRATCH);
    uart_write_string("[jny_mscratch_init] mscratch="); uart_write_hex(check); uart_write_string("\n");
#endif

}

void jny_mtvec_init()
{
    CSRW(MTVEC, &trap_handler);

#ifdef DEBUG
    uint64_t check; CSRR(check, MTVEC);
    uart_write_string("[jny_mtvec_init] mtvec="); uart_write_hex(check); uart_write_string("\n");
#endif

}

void jny_pmp_setup()
{
    uint64_t cfg = 0;

    // Lock the bootloader/firmware
    // TODO: Unlock once development of kernel is fine
    // CSRW(PMPADDR0, (uint64_t)&_boot_origin);
    // CSRW(PMPADDR1, (uint64_t)&_boot_end);
    // cfg |= ((1 << 7) | (0b01) << 3 | 0b111) << 8;

    // Setup the s-mode memory for the kernel
    CSRW(PMPADDR2, (uint64_t)&_kernel_origin >> 2);
    CSRW(PMPADDR3, (uint64_t)&_kernel_end >> 2);
    cfg |= ((0b01) << 3 | 0b111) << 24;
    
    // Write config
    CSRW(PMPCFG0, cfg);
}

void jny_handover()
{
    // Set mstatus to s-mode
    uint64_t cfg;
    CSRR(cfg, MSTATUS);
    cfg &= ~(0b11) << 11;   // clear MPP field
    cfg |= (0b01) << 11;    // set MPP field
    CSRW(MSTATUS, cfg);

    // Set mepc to restart exec at kernel origin
    CSRW(MEPC, (uint64_t)&_kernel_origin);

    asm volatile ("mret");
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

#define CAUSE_BIT   (1ULL << 63)
#define CAUSE_MASK  ((1ULL << 63)-1)

void jny_dispatcher(frame_t* frame)
{
    uint64_t code = frame->mcause & CAUSE_MASK;

    switch (frame->mcause & CAUSE_BIT)
    {
    case 0:                 // Exception path
        switch (code)
        {
        case 0: jny_panic("instruction address misaligned", frame->mepc, frame->mcause, code); break;
        case 1: jny_panic("instruction access fault", frame->mepc, frame->mcause, code); break;
        case 2: jny_panic("illegal instruction", frame->mepc, frame->mcause, code); break;
        case 3: jny_panic("breakpoint (unexpected)", frame->mepc, frame->mcause, code); break;
        case 4: jny_panic("load address misaligned", frame->mepc, frame->mcause, code); break;
        case 5: jny_panic("load access fault", frame->mepc, frame->mcause, code); break;
        case 6: jny_panic("store/amo address misaligned", frame->mepc, frame->mcause, code); break;
        case 7: jny_panic("store/amo access fault", frame->mepc, frame->mcause, code); break;
        case 8: jny_panic("ecall from u-mode (unexpected - no priv drop yet)", frame->mepc, frame->mcause, code); break;
        case 9: jny_panic("ecall from s-mode (unexpected - no priv drop yet)", frame->mepc, frame->mcause, code); break;
        case 11:            // ecall from M-mode - your deliberate test trap
            uart_write_string("[jny_dispatcher] ecall from m-mode, mepc="); uart_write_hex(frame->mepc); uart_write_string("\n");
            CSRW(MEPC, frame->mepc + 4); // must advance, or you re-trap on the same ecall forever   
            break;
        case 12: jny_panic("instruction page fault (unexpected - no paging yet)", frame->mepc, frame->mcause, code); break;
        case 13: jny_panic("load page fault (unexpected - no paging yet)", frame->mepc, frame->mcause, code); break;
        case 15: jny_panic("store/amo page fault (unexpected - no paging yet)", frame->mepc, frame->mcause, code); break;
        default: jny_panic("unhandled exception code", frame->mepc, frame->mcause, 255); break;
        }
        break;
    case CAUSE_BIT:         // Interrupt path
        switch (code)
        {
        case 3:  jny_panic("machine software interrupt (unexpected - MIE off)", frame->mepc, frame->mcause, code); break;
        case 7:  jny_panic("machine timer interrupt (unexpected - MIE off)", frame->mepc, frame->mcause, code); break;
        case 11: jny_panic("machine external interrupt (unexpected - MIE off)", frame->mepc, frame->mcause, code); break;
        default: jny_panic("unhandled interrupt code", frame->mepc, frame->mcause, 255); break;
        }
        break;
    default: jny_panic("unreachable mcause category", frame->mepc, frame->mcause, 255); break;
    }
}

#define DISPATCHER_TEST

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

#ifdef DISPATCHER_TEST
    asm volatile("ecall");
    uart_write_string("[jny_main] dispatcher test passed\n");
#endif

    // Success

    // TEST PASSED (EXIT)
    SIFIVE_TEST_PASS();
}