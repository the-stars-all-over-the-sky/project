#ifndef PROTO_H__ // 主要是用来防止重复包含头文件
#define PROTO_H__
#include "site_type.h" // 用<> 括起来的头文件会先在一个或多个标准位置搜索。用"" 括起来的头文件会首先在“当前目录” 中搜索, 然后(如果没有找到) 再在标准位置搜索

#define DEFAULT_MGROUP "224.2.2.2" // 默认的多播地址（多播地址的范围是224.0.0.0到239.255.255.255，共228个地址）
#define DEFAULT_RCVPORT "1989" // 接收数据的默认端口

#define CHANNEL_NUM 100 // 频道数量
#define LIST_CHANNEL_ID 0 // 0号频道用来发送节目单
#define MIN_CHANNEL_ID 1 // 最小频道ID为1
#define MAX_CHANNEL_ID (MIN_CHANNEL_ID + CHANNEL_NUM - 1)

#define MSG_CHANNEL_MAX (65536 - 20 - 8) // 65536 byte = 8KB；msg推荐长度512byte-64K（其中，IP报头长度在20-60byte之间浮动,UDP包的报头是8个字节）
#define MAX_DATA (MSG_CHANNEL_MAX - sizeof(chnid_t))

#define MSG_LIST_MAX (65536 - 20 - 8)
#define MAX_ENTRY (MSG_LIST_MAX - sizeof(chnid_t))

struct msg_channel_st // 频道id && 发送/接收的数据
{
    chnid_t chnid; // must between [MIN_CHANNEL_ID, MAX_CHANNEL_ID]
    uint8_t data[1]; // 使结构体变长；其实可以写data[0]，但是新的C99标准才支持长度为0的数组，考虑兼容性，这里定义长度为1
}__attribute__((packed)); // __attribnute__((packed)) 告诉gcc，次结构体不需要进行优化对齐（原因：UDP数据包在不同平台的对齐方式是不一样的）

struct msg_list_entry_st // 节目信息：频道id && 结构体长度长度 && 节目描述
{
    chnid_t chnid;
    uint16_t len;
    uint8_t desc[1];
}__attribute__((packed));

struct msg_list_st // 节目单：频道id && 节目信息
{
    chnid_t chnid; // must be LIST_CHANNEL_ID
    struct msg_list_entry_st entry[1];
}__attribute__((packed));

#endif