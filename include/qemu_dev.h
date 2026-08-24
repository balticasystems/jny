#pragma once

#include <stdint.h>

#include "extensions.h"

// IO

#define UART_BASE   (0x10000000UL)
#define UART_THR    (*(volatile unsigned char *)(UART_BASE + 0x00))
#define UART_LSR    (*(volatile unsigned char *)(UART_BASE + 0x05))
#define LSR_THRE    (1 << 5)

force_inline void uart_write_char(char c)
{
    while (!(UART_LSR & LSR_THRE)) { /* spin */ }
    UART_THR = c;
}

force_inline void uart_write_string(const char* s)
{
    for (int i = 0; s[i] != '\0'; i++)
    {
        uart_write_char(s[i]);
    }
}

force_inline void uart_write_hex(uint64_t val)
{
    static const char digits[] = "0123456789ABCDEF";

    uart_write_char('0');
    uart_write_char('x');

    // 16 nibbles for 64 bits
    // nibble = half byte
    for (int i = 15; i >= 0; i--) 
    {   
        uint8_t nibble = (val >> (i * 4)) & 0xF;
        uart_write_char(digits[nibble]);
    }
}

// CTL

// SiFive
#define SIFIVE_TEST             (*(volatile uint32_t*)0x100000)

#define SIFIVE_TEST_PASS_FLAG   (0x5555)
#define SIFIVE_TEST_FAIL_FLAG   (0x3333)
#define SIFIVE_TEST_RESET_FLAG  (0x7777)

#define SIFIVE_TEST_PASS()      (SIFIVE_TEST = SIFIVE_TEST_PASS_FLAG)
#define SIFIVE_TEST_FAIL(code)  (SIFIVE_TEST = (((code) << 16) | SIFIVE_TEST_FAIL_FLAG))
#define SIFIVE_TEST_RESET()     (SIFIVE_TEST = SIFIVE_TEST_RESET_FLAG)
