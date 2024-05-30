#include<onix/bitmap.h>
#include<onix/string.h>
#include<onix/onix.h>
#include<onix/assert.h>

//构造位图
void bitmap_make(bitmap_t* map, char* bits, u32 length, u32 offset) {
    map->bits = bits;
    map->length = length;
    map->offset = offset;
}

//位图初始化,全部置为0
void bitmap_init(bitmap_t* map, char* bits, u32 length, u32 start) {
    memset(bits, 0, length);
    bitmap_make(map, bits, length, start);
}

/// @brief 测试某一位是否为1
/// @param map 位图结构的指针
/// @param index 检查位的索引
/// @return 测试某一位是否为1
bool bitmap_test(bitmap_t* map, idx_t index) {
    assert(index >= map->offset);

    //得到位图的索引
    idx_t idx = index - map->offset;

    //位图数组中的字节
    u32 bytes = idx / 8;

    //该字节中的那一位
    u8 bits = idx % 8;
    assert(bytes < map->length);

    //返回那一位是否等于1
    return (map->bits[bytes] & (1 << bits));
}

/// @brief 设置位图某位的值
/// @param map 位图指针
/// @param index 被设置的索引值
/// @param value 0/1
void bitmap_set(bitmap_t* map, idx_t index, bool value) {
    //value 必须是二值的
    assert(value == 0 || value == 1);

    assert(index >= map->offset);

    //得到位图的索引
    idx_t idx = index - map->offset;

    //位图数组中的字节
    u32 bytes = idx / 8;
    u8 bits = idx % 8;
    if (value) {
        //置为1
        map->bits[bytes] |= (1 << bits);
    }
    else {
        //设置为0
        map->bits[bytes] &= ~(1 << bits);
    }
}

//从位图中得到连续的count位
int bitmap_scan(bitmap_t* map, u32 count) {
    int start = EOF;    //标记目标开始的位置
    u32 bits_left = map->length * 8;  //剩余的位数
    u32 next_bit = 0;   //下一个位
    u32 counter = 0;    //计数器

    while (bits_left-- > 0) {
        if (!bitmap_test(map, map->offset + next_bit)) {
            //如果下一位没有被占用,则计数器加一
            counter++;
        }
        else {
            //否则计数器设置为0,继续寻找
            counter = 0;
        }

        //下一位,位置加一
        next_bit++;
        if (counter == count) {
            start = next_bit - count;
            break;
        }
    }
    //如果没有找到,则返回EOF(End of File)
    if (start == EOF) {
        return EOF;
    }

    //否则将找到的位,全部置为1
    bits_left = count;
    next_bit = start;
    while (bits_left--) {
        bitmap_set(map, map->offset + next_bit, true);
        next_bit++;
    }
    return start + map->offset;
}