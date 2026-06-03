__attribute__((ms_abi)) void kernel_main(void) {
    __asm__ volatile (
        "mov $0xe9, %%dx\n\t"
        "mov $89, %%al\n\t"
        "out %%al, %%dx\n\t"
        "mov $69, %%al\n\t"
        "out %%al, %%dx\n\t"
        "mov $83, %%al\n\t"
        "out %%al, %%dx\n\t"
        "mov $10, %%al\n\t"
        "out %%al, %%dx\n\t"
        :
        :
        : "rax", "rdx"
    );
    while (1) {
        __asm__ volatile ("hlt");
    }
}