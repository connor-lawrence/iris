__attribute__((ms_abi)) void kernel_main(void) {
    // We inline the direct machine instructions so GCC cannot insert tracking symbols.
    // This writes "\nYES\n" straight into the QEMU debug port hardware registers.
    __asm__ volatile (
        "mov $0xe9, %%dx\n\t"  // Load QEMU Debug Port into DX register
        
        "mov $10, %%al\n\t"    // Newline '\n'
        "out %%al, %%dx\n\t"
        
        "mov $89, %%al\n\t"    // 'Y'
        "out %%al, %%dx\n\t"
        
        "mov $69, %%al\n\t"    // 'E'
        "out %%al, %%dx\n\t"
        
        "mov $83, %%al\n\t"    // 'S'
        "out %%al, %%dx\n\t"
        
        "mov $10, %%al\n\t"    // Newline '\n'
        "out %%al, %%dx\n\t"
        :
        :
        : "rax", "rdx"         // Tell compiler we are modifying these registers
    );

    // Freeze safely
    while (1) {
        __asm__ volatile ("hlt");
    }
}
