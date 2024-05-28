extern void memory_map_init();
extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void hang();

extern void memory_test();

void kernel_init()
{
    memory_map_init();
    interrupt_init();
    clock_init();
    // time_init();
    // rtc_init();

    memory_test();

    // // task_init();
    asm volatile("sti");
    hang();
}