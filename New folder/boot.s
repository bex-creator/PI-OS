.section ".text.boot"

.global _start
_start:
    // Read core ID from mpidr_el1 register
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    
    // If not core 0, put it to sleep
    cbnz    x0, halt

    // Set stack pointer right below our code entry point
    ldr     x0, =_start
    mov     sp, x0

    // Jump into our C kernel main function
    bl      kernel_main

halt:
    // Infinite sleep loop for extra cores
    wfe
    b       halt