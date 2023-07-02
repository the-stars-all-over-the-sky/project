#ifndef SITETYPE_H__
#define SITETYPE_H__
#include <stdint.h> // 不加这个头文件回报错：“未定义标识符 "uint8_t" ”

/*
备注：
    在C99标准 (C语言的ISO标准) 里面，uint8_t这种带_t后缀的数据类型是被设计成跨平台兼容的数据类型。
    这里"t"代表"type"。也就是说，无论在什么平台上编译，int8_t都是8个bit。
*/
typedef uint8_t chnid_t; // channel_id数据类型：0-255

#endif