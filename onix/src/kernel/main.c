#include <onix/onix.h>
#include <onix/io.h>
#include <onix/types.h>
#include<onix/console.h>
#include<onix/stdarg.h>
#include<onix/printk.h>
#include<onix/assert.h>
#include<onix/stdio.h>
#include<onix/debug.h>
#include<onix/global.h>
void kernel_init()
{
   console_init();
   // BMB;
   
   // DEBUGK("debug onix!!!\n");
   gdt_init();
   return;   
}