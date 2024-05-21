#ifndef ONIX_GLOBAL_H
#define ONIX_GLOBAL_H

#define GDT_SIZE 128

#include<onix/types.h>

typedef struct descriptor_t//共八个字节
{
    unsigned short limit_low;//段界限 0~15位
    unsigned int base_low;//基地址0~23位 16M
    unsigned char type : 4;//段类型
    unsigned char segment : 1;//1 表示代码段或数据段,0 表示系统段
    unsigned char DPL : 2;//Descriptor Privilege Level 描述符特权等级0~3
    unsigned char present : 1;//存在位 1在内存中,0在磁盘中
    unsigned char limit_high : 4;//段界限 16~19
    unsigned char available : 1;//送给操作系统的部分
    unsigned char long_mode : 1;//64位扩展标志
    unsigned char big : 1;//32位还是16位
    unsigned char granularity : 1;//粒度是4kb还是1byte
    unsigned char base_high;//基地址24-31位
}_packed descriptor_t;

//段选择子
typedef struct selector_t
{
    u8 RPL : 2;//特权级
    u8 TI : 1;//gdt或者ldt
    u16 index : 13;//段选择子索引
} selector_t;

//全局描述符指针
typedef struct pointer_t {
    u16 limit;//段大小
    u32 base;//段的起始位置
}_packed pointer_t;

void gdt_init();

#endif