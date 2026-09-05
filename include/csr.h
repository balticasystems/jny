#pragma once

#define MHARTID  "mhartid"   // hart's unique ID, read-only, set by hardware
#define MTVEC    "mtvec"     // trap handler entry address + mode (direct/vectored)
#define MSTATUS  "mstatus"   // global interrupt enable + privilege mode tracking
#define MEPC     "mepc"      // PC to resume at after a trap is serviced
#define MCAUSE   "mcause"    // reason the last trap occurred (exception/interrupt code)
#define MTVAL    "mtval"     // extra trap info, e.g. faulting address
#define MIE      "mie"       // which interrupt sources are enabled
#define MIP      "mip"       // which interrupts are currently pending
#define MSCRATCH "mscratch"  // scratch storage for the trap handler, no fixed hw meaning

// PMP

// PMPADDR
#define PMPADDR0  "pmpaddr0"
#define PMPADDR1  "pmpaddr1"
#define PMPADDR2  "pmpaddr2"
#define PMPADDR3  "pmpaddr3"
#define PMPADDR4  "pmpaddr4"
#define PMPADDR5  "pmpaddr5"
#define PMPADDR6  "pmpaddr6"
#define PMPADDR7  "pmpaddr7"
#define PMPADDR8  "pmpaddr8"
#define PMPADDR9  "pmpaddr9"
#define PMPADDR10 "pmpaddr10"
#define PMPADDR11 "pmpaddr11"
#define PMPADDR12 "pmpaddr12"
#define PMPADDR13 "pmpaddr13"
#define PMPADDR14 "pmpaddr14"
#define PMPADDR15 "pmpaddr15"
#define PMPADDR16 "pmpaddr16"
#define PMPADDR17 "pmpaddr17"
#define PMPADDR18 "pmpaddr18"
#define PMPADDR19 "pmpaddr19"
#define PMPADDR20 "pmpaddr20"
#define PMPADDR21 "pmpaddr21"
#define PMPADDR22 "pmpaddr22"
#define PMPADDR23 "pmpaddr23"
#define PMPADDR24 "pmpaddr24"
#define PMPADDR25 "pmpaddr25"
#define PMPADDR26 "pmpaddr26"
#define PMPADDR27 "pmpaddr27"
#define PMPADDR28 "pmpaddr28"
#define PMPADDR29 "pmpaddr29"
#define PMPADDR30 "pmpaddr30"
#define PMPADDR31 "pmpaddr31"
#define PMPADDR32 "pmpaddr32"
#define PMPADDR33 "pmpaddr33"
#define PMPADDR34 "pmpaddr34"
#define PMPADDR35 "pmpaddr35"
#define PMPADDR36 "pmpaddr36"
#define PMPADDR37 "pmpaddr37"
#define PMPADDR38 "pmpaddr38"
#define PMPADDR39 "pmpaddr39"
#define PMPADDR40 "pmpaddr40"
#define PMPADDR41 "pmpaddr41"
#define PMPADDR42 "pmpaddr42"
#define PMPADDR43 "pmpaddr43"
#define PMPADDR44 "pmpaddr44"
#define PMPADDR45 "pmpaddr45"
#define PMPADDR46 "pmpaddr46"
#define PMPADDR47 "pmpaddr47"
#define PMPADDR48 "pmpaddr48"
#define PMPADDR49 "pmpaddr49"
#define PMPADDR50 "pmpaddr50"
#define PMPADDR51 "pmpaddr51"
#define PMPADDR52 "pmpaddr52"
#define PMPADDR53 "pmpaddr53"
#define PMPADDR54 "pmpaddr54"
#define PMPADDR55 "pmpaddr55"
#define PMPADDR56 "pmpaddr56"
#define PMPADDR57 "pmpaddr57"
#define PMPADDR58 "pmpaddr58"
#define PMPADDR59 "pmpaddr59"
#define PMPADDR60 "pmpaddr60"
#define PMPADDR61 "pmpaddr61"
#define PMPADDR62 "pmpaddr62"
#define PMPADDR63 "pmpaddr63"

// PMPCFG 
#define PMPCFG0 "pmpcfg0"
#define PMPCFG1 "pmpcfg1"
#define PMPCFG2 "pmpcfg2"
#define PMPCFG3 "pmpcfg3"
#define PMPCFG4 "pmpcfg4"
#define PMPCFG5 "pmpcfg5"
#define PMPCFG6 "pmpcfg6"
#define PMPCFG7 "pmpcfg7"

#define CSRR(REG, CSR_REG) \
    asm volatile ("csrr %0, " CSR_REG : "=r" (REG))       // read CSR into REG
#define CSRW(CSR_REG, REG) \
    asm volatile ("csrw " CSR_REG ", %0" : : "r" (REG))   // overwrite entire CSR with REG
#define CSRRW(REG, CSR_REG) \
    asm volatile ("csrrw %0, " CSR_REG ", %0" : "+r" (REG) : : "memory") // atomically swap REG and CSR
#define CSRS(CSR_REG, REG) \
    asm volatile ("csrs " CSR_REG ", %0" : : "r" (REG))   // atomically set bits in CSR per REG mask
#define CSRC(CSR_REG, REG) \
    asm volatile ("csrc " CSR_REG ", %0" : : "r" (REG))   // atomically clear bits in CSR per REG mask

