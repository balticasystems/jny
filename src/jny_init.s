.section .text.init
.global _start
.global trap_handler
_start:
    csrr t0, mhartid    # get the mhartid
    bnez t0, park_hart  # if it is not 0 disable
    la sp, _stack_top   # initialize the stack stack
    call jny_main       # jump into C part of the bootloader
park_hart:
    wfi                 # wait until the next interrupt
    j park_hart         # stay parked
trap_handler:
    wfi                 # wait until the next interrupt
    j trap_handler      # stay parked (this time for the trap handler)
    