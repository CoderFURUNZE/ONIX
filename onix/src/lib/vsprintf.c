#include <onix/stdarg.h>
#include <onix/string.h>

#define ZEROPAD 0x01 // 填充零
#define SIGN 0x02    // unsigned/signed long
#define PLUS 0x04    // 显示加
#define SPACE 0x08   // 如是加，则置空格
#define LEFT 0x10    // 左调整
#define SPECIAL 0x20 // 0x
#define SMALL 0x40   // 使用小写字母
#define DOUBLE 0x80  // 浮点数

#define is_digit(c) ((c) >= '0' && (c) <= '9')

// 将字符数字串转换成整数，并将指针前移
static int skip_atoi(const char **s)
{
    int i = 0;
    while (is_digit(**s))
        i = i * 10 + *((*s)++) - '0';
    return i;
}

// 将整数转换为指定进制的字符串
// str - 输出字符串指针
// num - 整数
// base - 进制基数
// size - 字符串长度
// precision - 数字长度(精度)
// flags - 选项
static char* number(char* str, unsigned long num, int base, int size, int precision, int flags) {
    char c, sign, tmp[36];
    const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int i;
    int index;
    char* ptr = str;

    //如果flags 指定用小写字母 则定义小写字符集
    if (flags & SMALL)
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";

    //如果flags指出要左对齐,则屏蔽类型中的填零标志
    if (flags & LEFT)
        flags &= ~ZEROPAD;

    //如果进制基数小于2或者大于36,则退出处理
    if (base < 2 || base>36)
        return 0;

    //如果flags 指出要填零,则置字符变量c = '0' 否则c等于空格字符
    c = (flags & ZEROPAD) ? '0' : ' ';

    //如果flags指出是带符号数并且数值num小于0,则置符号变量sign = 负号,并使num取绝对值
    if (flags & SIGN && num < 0) {
        sign = '-';
        num = -num;
    }
    else {
        //否则flags 指出是加号,则置sign = 加号 否则若类型带空格标志则sign = 空格,否则置0
        sign = (flags & PLUS) ? '+' : ((flags & SPACE) ? ' ' : 0);
    }

    //若带符号,则宽度值减1
    if (sign)
        size--;

    //若flags指出是特殊转换,则堆十六位进制宽度再减少2位(用于0x)
    if (flags & SPECIAL) {
        if (base == 16) {
            size -= 2;
        }
        //对于八进制宽度减1(用于八进制转换结果前放一个零)
        else if (base == 8) {
            size--;
        }
    }

    i = 0;

    //如果数值num为0 则临时字符串 = '0' 否则根据给定的基数将数值num转换为字符形式
    if (num == 0) {
        tmp[i++] = '0';
    }
    else {
        while (num != 0)
        {
            index = num % base;
            num /= base;
            tmp[i++] = digits[index];
        }
    }

    //若数值字符个数大于精度值,则精度值扩展为数字个数值
    if (i > precision) {
        precision = i;
    }

    //宽度值size减去用于存放数值字符的个数
    size -= precision;

    //从这里真正开始形成所需要的转换结果,并且暂时存放在字符串str中

    //若flags 中没有填0(ZEROPAD) 和 左对齐(左调整)标志
    //则在str中首先填放剩余宽度值指出的空格数
    if (!flags & (ZEROPAD + LEFT)) {
        while (size-- > 0)
        {
            *str++ = ' ';
        }
    }

    //若需带符号位,则存入符号
    if (sign) {
        *str++ = sign;
    }

    if (flags & SPECIAL) {
        //则对于八进制转换结果头一位放置一个'0'
        if (base == 8) {
            *str++ = '0';
        }
        else if (base == 16) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    //若flags中没有左调整(左对齐)标志 则在剩余宽度中存放c字符('0'或空格)
    if (!(flags & LEFT)) {
        while (size-- > 0) {
            *str++ = c;
        }
    }

    //此时 i 存放数值num的数字个数
    //若数字个数小于精度值 则str 中放入(精度值 - i)个'0'
    while (i < precision--) {
        *str++ = '0';
    }

    //将转换至换好的数字字符填入到str中 共i个
    while (i-- > 0) {
        *str++ = tmp[i];
    }

    //若宽度值仍大于零
    //则表示flags标志中有左对齐标志标志
    //则在剩余宽度中放入空格
    while (size-- > 0) {
        *str++ = ' ';
    }
    return str;
}

int vsprintf(char* buf, const char* fmt, va_list args) {
    int len;
    int i;

    //用于存放转换过程的字符串
    char* str;
    char* s;
    int* ip;

    //number()函数使用的标志
    int flags;
    int field_width;//输出字段宽度
    int precision;//min整数数字个数,max字符串中字符个数
    int qualifier;//'h','l'或'L' 用于整数字段

    //首先将字符串指向buf
    //然后扫描格式字符串
    //对各个格式转换知识进行相应的处理

    for (str = buf;*fmt;++fmt) {
        //格式转换指示字符串均以'%'开始
        //这里从fmt格式字符串中扫描% 寻找格式转换字符串的开始
        //不是格式指示的一般字符均被依次存入str
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        //下面取得格式指示字符串中的标志域,并将标志常量放入flags变量中
        flags = 0;

    repeat:
        //略过第一个%
        ++fmt;
        switch (*fmt)
        {
            //左对齐调整
        case '-':
            flags |= LEFT;
            goto repeat;
            //放加号
        case '+':
            flags |= PLUS;
            goto repeat;
        case ' ':
            flags |= SPACE;
            goto repeat;
        case '#':
            flags |= SPECIAL;
            goto repeat;
        case '0':
            flags |= ZEROPAD;
            goto repeat;
        }
        //取当前参数字段宽度域值 放入field_width变量中
        field_width = -1;

        //如果宽度域中是数值则直接取其为宽度值
        if (is_digit(*fmt)) {
            field_width = skip_atoi(&fmt);
        }

        //如果宽度域中是字符 '*' 表示下一个参数指定宽度
        else if (*fmt == '*') {
            ++fmt;
            //因此调用va_arg 取宽度值
            field_width = va_arg(args, int);

            //若此时宽度值小于0 则该负数表示其带有标志'-'标志(左对齐)
            if (field_width < 0) {
                //因此还需要再标志变量中添加该标志,并将字段宽度值取为其绝对值
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        //取格式转换串的精度域 并放入precision变量中
        precision = -1;

        //精度域开始的标志是'.' 其处理过程与上面宽度域的类似
        if (*fmt == '.') {
            ++fmt;
            //如果精度域中是数值则直接取其为精度值
            if (is_digit(*fmt)) {
                precision = skip_atoi(&fmt);
            }
            else if (*fmt == '*') {
                //因此调用va_arg 取精度值
                precision = va_arg(args, int);
            }

            //若此时宽度值小于0 则将其字段精度值取为绝对值
            if (precision < 0) {
                precision = 0;
            }
        }

        //下面这段代码分析长度修饰符,并将其存入qualifer变量
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L') {
            qualifier = *fmt;
            ++fmt;
        }

        //下面分析转换指示符
        switch (*fmt) {
            //如果转换指示符是'c' 则表示对齐参数应该是字符
        case 'c':
            if (!(flags & LEFT)) {
                while (--field_width > 0)
                    *str++ = ' ';
            }
            *str++ = (unsigned char)va_arg(args, int);
            //如多宽度域还大于0 则表示左对齐
            //则在参数字符添加(宽度值-1)个空格字符
            while (--field_width > 0)
            {
                *str++ = ' ';
            }
            break;

            //如果转换指示符是's' 则表示对应参数是字符串
        case 's':
            s = va_arg(args, char*);
            //首先取参数字符串的长度
            len = strlen(s);
            if (precision < 0) {
                precision = len;
            }
            else if (len > precision)
            {
                len = precision;
            }

            //此时如果标志域表明不是左对齐
            if (!(flags & LEFT)) {
                while (len < field_width--)
                {
                    *str++ = ' ';
                }
            }
            //然后再放入参数字符串
            for (i = 0;i < len;++i) {
                *str++ = *s++;
            }
            //如果宽度域大于0,则表示左对齐
            //则在参数字符串后面 天剑(宽度值-字符串长度)个空格字符
            while (len < field_width--) {
                *str++ = ' ';
            }
            break;
            //如果格式转换符是'o' 表示需将对应的参数转换成八进制的字符串        
        case 'o':
            str = number(str, va_arg(args, unsigned long), 8, field_width, precision, flags);
            break;
            //如果格式转换符是'p' 表示对应参数的一个指针类型
        case 'p':
            if (field_width == -1) {
                field_width == 8;
                flags |= ZEROPAD;
            }
            str = number(str, (unsigned long)va_arg(args, void*), 16, field_width, precision, flags);
            break;

            //若格式转换指示是'x'或'X'
            //则表示将对应参数需要打印成十六进制输出
        case 'x':
            //用'x'表示小写字母
            flags |= SMALL;
        case 'X':
            str = number(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
            break;
            //若格式转换符是'n'
            //表示要把目前为止转换输出的字符数保存到对应参数指针指定的位置中
        case 'n':
            ip = va_arg(args, int*);
            *ip = (str - buf);
            break;
        default:
            //若格式字符串不是'%' 则表示格式字符串有错
            if (*fmt != '%') {
                *str++ = '%';
            }
            //如果格式转换符的位置处还有字符,则也直接将该字符写入输出串中
            //然后继续循环处理格式字符串
            if (*fmt) {
                *str++ = *fmt;
            }
            else
                //否则表示已经处理到格式字符串的结尾处,则退出循环
                --fmt;
            break;
        }
    }
    *str = '\0';
    return str - buf;
}

//结果按格式输出字符串到buf
int sprintf(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(buf, fmt);
    int i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}