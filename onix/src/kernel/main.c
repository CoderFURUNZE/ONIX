#include<onix/types.h>
extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void time_init();
extern void rtc_init();
extern void hang();
extern void set_alarm(u32 secs);
void kernel_init()
{
    console_init();
    gdt_init();
    interrupt_init();
    clock_init();
    time_init();
    rtc_init();
    set_alarm(2);
    // task_init();
    asm volatile("sti");
    hang();
}