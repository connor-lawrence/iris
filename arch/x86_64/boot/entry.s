.global _start
.extern kernel_main

.section .text
_start:

    # Disable interrupts and clear direction flag
    cli
    cld

    # Set up stack and align it
    leaq stack_top(%rip), %rsp
    andq $-16, %rsp

    # Mark bottom of initial stack frame
    xorq %rbp, %rbp

    call kernel_main

.hang:
    hlt
    jmp .hang

# Reserve 16 KiB for the stack
.section .bss
.align 16

.equ STACK_SIZE, 16384

stack:
    .skip STACK_SIZE

stack_top:

# Tells linker that an executable stack is not needed
.section .note.GNU-stack,"",@progbits
