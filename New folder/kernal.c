void kernel_main(void) {
    // Kernel initialized successfully!
    // For Raspberry Pi 5 bare metal, hardware interaction (UART/GPIO)
    // is configured by sending commands to the Broadcom registers.
    
    while (1) {
        // Sleep processor to save power until an interrupt occurs
        __asm__ volatile("wfe");
    }
}