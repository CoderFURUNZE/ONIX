#ifndef ONIX_IO_H
#define ONIX_IO_H
    #include<onix/types.h>
    extern u8 inb(u16 port);
    extern u16 inw(u16 port);

    extern void outb(u16 port,u8 value);
    extern void outw(u16 port,u16 value);
#endif