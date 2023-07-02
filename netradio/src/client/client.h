#ifndef CLIENT_H__
#define CLIENT_H__

#define DEFAULT_PLAYER_CMD "/usr/bin/mpg123 -" // mpg123 的option如果是"-",则对标准输入进行解码

struct client_conf_st
{
    char *rcvport;    // 接收数据的端口（跨网络的数据传输，不允许出现指针；这里的指针，不会直接上传到网络）
    char *mgroup;     // 多播地址
    char *player_cmd; // 播放命令
};

// struct ip_mreqn // 如果报错次结构体重复定义，删掉即可；这里定义是因为库头文件中没有定义
// {
//     struct in_addr imr_multiaddr; /* IP multicast address of group */
//     struct in_addr imr_address;   /* local IP address of interface */
//     int imr_ifindex;              /* Interface index */
// };

extern struct client_conf_st client_conf;

#endif