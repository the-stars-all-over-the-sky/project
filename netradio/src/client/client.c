#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <unistd.h>
#include <proto.h>
#include "client.h"

/*
    -M  --mgroup    指定多播组
    -P  --port      指定接收端口
    -p  --player    指定播放器
    -H  --help      显示帮助
*/

struct client_conf_st client_conf = {
    .rcvport = DEFAULT_RCVPORT,
    .mgroup = DEFAULT_MGROUP,
    .player_cmd = DEFAULT_PLAYER_CMD};

static void PrintfHelp(void)
{
    printf("-M  --mgroup    指定多播组\r\n");
    printf("-P  --port      指定接受端口\r\n");
    printf("-p  --player    指定播放器\r\n");
    printf("-H  --help      显示帮助\r\n");
    printf("\r\n");
}

static ssize_t writen(int fd, const char *buf, size_t len)
{
    int pos = 0;
    while (len > 0)
    {
        int ret = write(fd, buf + pos, len);
        if (ret < 0)
        {
            if (errno = EINTR) // 可能出现假错
            {
                continue;
            }
            perror("[client][writen] write fail");
            return -1;
        }
        len -= ret;
        pos += ret;
    }
    return 0;
}

int main(int argc, char **argv)
{
    int pd[2];
    pid_t pid;
    int opt;
    int sd;
    int result;
    int index = 0;
    struct sockaddr_in laddr;      // 本地地址
    struct sockaddr_in serveraddr; // server地址
    struct sockaddr_in raddr;      // 远端地址
    socklen_t serveraddr_len;
    socklen_t remoteaddr_len;
    struct ip_mreqn mreq;
    int choosenid;
    int val;
    struct option argarr[] = {
        {"port", 1, NULL, 'P'},
        {"mgroup", 1, NULL, 'M'},
        {"player", 1, NULL, 'p'},
        {"help", 0, NULL, 'H'},
        {NULL, 0, NULL, 0}};

    /*
        init(如果有级别高的配置，则忽略级别低的配置)
            级别：默认值 < 配置文件 < 环境变量 < 命令行参数
    */

    while (1)
    {
        opt = getopt_long(argc, argv, "P:M:p:H", argarr, &index); // 如果选项后面有参数，需要在选项后加上冒号
        if (opt < 0)
        {
            break;
        }
        switch (opt)
        {
        case 'P':
            client_conf.rcvport = optarg;
            break;
        case 'M':
            client_conf.mgroup = optarg;
            break;
        case 'p':
            client_conf.player_cmd = optarg;
            break;
        case 'H':
            PrintfHelp();
            exit(0);
        default:
            printf("参数错误\r\n");
            abort(); // Abort execution and generate a core-dump.
            // abort 函数是一个比较严重的函数，当调用它时，会导致程序异常终止，而不会进行一些常规的清除工作，比如释放内存等
            break;
        }
    }
    printf("\n===============[client] getopt success=================\n");

    /*
        socket
    */

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        perror("[client][main] get socket fail!!");
        exit(1);
    }
    printf("\n===============[client] get socket success=================\n");

    /*
        setsockopt
    */

    result = inet_pton(AF_INET, client_conf.mgroup, &mreq.imr_multiaddr);
    if (result < 0)
    {
        perror("[client][main]inet_pton imr_multiaddr fail");
        exit(1);
    }
    result = inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    if (result < 0)
    {
        perror("[client][main]inet_pton imr_address fail");
        exit(1);
    }
    mreq.imr_ifindex = if_nametoindex("ens33");
    result = setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)); // 加入多播组
    if (result < 0)
    {
        perror("[client][main]set sockopt IP_ADD_MEMBERSHIP fail");
        exit(1);
    }

    val = 1;
    result = setsockopt(sd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val));
    if (result < 0)
    {
        perror("[client][main]set sockopt IP_MULTICAST_LOOP fail");
        exit(1);
    }
    printf("\n===============[client] set sockopt success=================\n");
    /*
        bind
    */

    laddr.sin_family = AF_INET;
    laddr.sin_port = htons(atoi(client_conf.rcvport));
    result = inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr.s_addr);
    if (result < 0)
    {
        perror("[client][main] inet_pton laddr.sin_addr.s_addr fail");
        exit(1);
    }

    result = bind(sd, (void *)&laddr, sizeof(laddr));
    if (result < 0)
    {
        perror("[client][main] bind fail");
        exit(1);
    }
    printf("\n===============[client] bind success=================\n");

    /*
        pipe
    */

    result = pipe(pd);
    if (result < 0)
    {
        perror("[client][main] pipe fail");
        exit(1);
    }
    printf("\n===============[client] get pipe success=================\n");
    /*
        fork
    */

    pid = fork();
    if (pid < 0)
    {
        perror("[client][main] fork fail");
        exit(1);
    }
    if (pid == 0) // child
    { 
        printf("\n===============[client] child:enter child process......=================\n");
        // 子进程：调用解码器
        close(sd); // 关闭套接字
        close(pd[1]); // 关闭写端
        dup2(pd[0], 0); // 由于解码器只能获取标准输入的内容,所以这里将管道标准输入作为读端(PS:简单点说,就是把管道读端重定向到标准输入)
        if (pd[0] > 0)
        {
            close(pd[0]);
        }
        printf("\n===============[client] child:start play......=================\n");
        execl("/bin/sh", "sh", "-c", client_conf.player_cmd, NULL);
        perror("[client][main] execl fail");
        exit(1);
    }
    else
    {
        // 父进程：从网络上收包，发给子进程

        // 收节目单

        struct msg_list_st *msg_list;
        msg_list = malloc(MSG_LIST_MAX);
        if (msg_list == NULL)
        {
            perror("parent: malloc fail");
            exit(1);
        }
        int len;
        printf("\n===============[client] parent:start recevie data......=================\n");
        serveraddr_len = sizeof(serveraddr);
        while (1)
        {
            len = recvfrom(sd, msg_list, MSG_LIST_MAX, 0, (void *)&serveraddr, &serveraddr_len);
            if (len < sizeof(struct msg_list_st))
            {
                fprintf(stderr, "[client][main] parent: message is too small.\n");
                continue;
            }
            if (msg_list->chnid != LIST_CHANNEL_ID)
            {
                fprintf(stderr, "[client][main] parent: chnid is not match.\n");
                continue;
            }
            break;
        }
        // 打印节目单，选择频道
        struct msg_list_entry_st *pos;
        for (pos = msg_list->entry; (char *)pos < ((char *)msg_list) + len; pos = (struct msg_list_entry_st *)((char *)pos + ntohs(pos->len)))
        {
            printf("[client][main] print channel: %d:%s\n", pos->chnid, pos->desc);
        }
        free(msg_list);
        puts("[client][main] parent: Please enter:");
        int retval = 0;
        while (retval < 1)
        {
            retval = scanf("%d", &choosenid);
            // if (retval != 1)
            // {
            //     exit(1);
            // }
        }
        // 收频道包，发送给子进程
        fprintf(stdout, "choosenid = %d\n", choosenid);
        struct msg_channel_st *msg_channel;
        msg_channel = malloc(MSG_CHANNEL_MAX);
        if (msg_channel == NULL)
        {
            perror("[client][main] parent: malloc msg_channel_st fail");
            exit(1);
        }
        remoteaddr_len = sizeof(raddr);
        while (1)
        {
            len = recvfrom(sd, msg_channel, MSG_CHANNEL_MAX, 0, (void *)&raddr, &remoteaddr_len);
            if (raddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr || raddr.sin_port != serveraddr.sin_port)
            {
                fprintf(stderr, "[client][main] parent: remote addr is wrong,is not equal server addr from list.\n");
                continue;
            }
            if (len < sizeof(struct msg_channel_st))
            {
                fprintf(stderr, "[client][main] parent: ignore, message is too small.\n");
                continue;
            }
            if (msg_channel->chnid == choosenid)
            {
                fprintf(stdout, "[client][main] accepted msg: %d received.\n", msg_channel->chnid);
                result = writen(pd[1], (char *)msg_channel->data, len - sizeof(chnid_t));
                // int fd = open("test.mp3", O_RDWR | O_CREAT | O_APPEND, 0644);
                // result = writen(fd, (char *)msg_channel->data, len - sizeof(chnid_t));
                if (result < 0)
                {
                    exit(1);
                }
            }
        }
        free(msg_channel);
        close(sd);
    }
    exit(0);
}