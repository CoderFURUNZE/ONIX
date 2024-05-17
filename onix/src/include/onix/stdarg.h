#ifndef ONIX_STDARG_H
#define ONIX_STDARG_H

typedef char* va_list;

#define va_start(ap,v) (ap = (va_list)&v + sizeof(char *)) //将ap指向第一个可变参数
#define va_arg(ap,t) (*(t *)((ap+=sizeof(char*))-sizeof(char*)))//逐个提取参数
#define va_end(ap) (ap = (va_list)0) //结束参数列表处理


#endif