#include <onix/onix.h>
#include <onix/types.h>
#include <onix/io.h>
#include <onix/string.h>
#include <onix/console.h>
#include <onix/printk.h>
#include <onix/assert.h>
#include <onix/debug.h>
#include <onix/global.h>
#include <onix/task.h>
#include <onix/interrupt.h>
#include <onix/stdlib.h>
extern void console_init();
extern void gdt_init();
extern void interrupt_init();
extern void clock_init();
extern void hang();
void kernel_init()
{
    console_init();
    gdt_init();
    interrupt_init();
    // task_init();
    clock_init();

    asm volatile("sti");
    hang();

    return;
}
