#include <onix/onix.h>
#include <onix/io.h>
#include <onix/types.h>
#include<onix/console.h>
#include<onix/stdarg.h>

#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5

#define CRT_CURSOR_H 0xe
#define CRT_CURSOR_L 0xf

char message[] = "hello onix!!!\n";

void test_args(int cnt,...){
   va_list args;//初始化指针
   va_start(args,cnt);//将指针指向第一个可变参数位置

   int arg;
   while(cnt--)
   {
      arg = va_arg(args,int);//逐个提取参数
   }
   va_end(args);
}

void kernel_init()
{
   console_init();
   // while(true)
   //    console_write(message,sizeof(message)-1);
   test_args(5,1,2,3,4,6);
}