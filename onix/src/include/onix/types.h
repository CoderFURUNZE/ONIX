#ifndef ONIX_TYPES_H
#define ONIX_TYPES_H

#include <onix/onix.h>

#define EOF -1 // END OF FILE

#define NULL ((void *)0) // 空指针

#define EOS '\0' // 字符串结尾

#ifndef __cplusplus
#define bool _Bool
#define true 1
#define false 0
#endif

// 用于定义特殊的结构体
#define _packed __attribute__((packed)) 

//用于省略函数的栈帧
#define _ofp __attribute__((optimize("omit-frame-pointer")))

typedef unsigned int size_t;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 time_t;

#endif