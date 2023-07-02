#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <getopt.h>
#include <arpa/inet.h>

#include "proto.h"
#include "meidalib.h"
#include "server_conf.h"
#include "thr_list.h"
#include "thr_channel.h"

/*
    -M      指定多播组
    -P      指定接收端口
    -F      前台运行
    -D      指定媒体库位置
    -I      指定网络设备
    -H      显示帮助
*/

struct server_conf_st server_conf = {
    .rcvport = DEFAULT_RCVPORT,
    .mgroup = DEFAULT_MGROUP,
    .media_dir = DEFAULT_MEDIA_DIR,
    .runmode = RUN_DAEMON,
    .ifname = DEFAULT_IF};

int serversd;
struct sockaddr_in sndaddr;
struct mlib_listentry_st *list;

static void printfhelp()
{
    printf("-M      指定多播组\n");
    printf("-P      指定接收端口\n");
    printf("-F      前台运行\n");
    printf("-D      指定媒体库位置\n");
    printf("-I      指定网络设备\n");
    printf("-H      显示帮助\n");
    printf("\n");
}

void daemon_exit(int s)
{
    thr_list_destroy();
    thr_channel_destroyall();
    mlib_freechnlist(list);
    syslog(LOG_WARNING, "[server][daemon_exit]:signal-%d caught, exit now!", s);
    closelog();
    exit(0);
}

static int daemonize(void)
{
    pid_t pid;
    int fd;
    pid = fork();
    if (pid < 0)
    {
        // perror("fork() fail");
        syslog(LOG_ERR, "[ERROR][daemonize] fork fail: %s", strerror(errno));
        return -1;
    }
    if (pid > 0)
    { // parent
        exit(0);
    }
    /* 守护进程拒绝stdin、stdout、stderr */
    fd = open("/dev/null", O_RDWR);
    if (fd < 0)
    {
        // perror("open() fail");
        syslog(LOG_WARNING, "[ERROR][daemonize] open /dev/null fail: %s", strerror(errno));
        return -2;
    }
    else
    {
        dup2(fd, 0);
        dup2(fd, 1);
        dup2(fd, 2);
        if (fd > 2)
        {
            close(fd);
        }
    }
    setsid();
    chdir("/"); // 指定守护进程运行路径，指定的路径必须存在
    umask(0);
    return 0;
}

static int socket_init(void)
{
    struct ip_mreqn mreq;
    serversd = socket(AF_INET, SOCK_DGRAM, 0);
    if (serversd < 0)
    {
        syslog(LOG_ERR, "[server][socket_init] fail to get server socket id :%s", strerror(errno));
        exit(1);
    }

    inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
    inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
    mreq.imr_ifindex = if_nametoindex(server_conf.ifname);

    if (setsockopt(serversd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
    {
        syslog(LOG_ERR, "[server][socket_init] fail to set sockopt:%s", strerror(errno));
        exit(1);
    }
    // bind()
    sndaddr.sin_family = AF_INET;
    sndaddr.sin_port = htons(atoi(server_conf.rcvport));
    inet_pton(AF_INET, server_conf.mgroup, &sndaddr.sin_addr);
    syslog(LOG_INFO, "[server][socket_init] socket init ok!\n");
    return 0;
}

int main(int argc, char *argv[])
{
    int result;

    struct sigaction sa;
    sa.sa_handler = daemon_exit;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGQUIT);
    sigaddset(&sa.sa_mask, SIGTERM);
    sigaction(SIGTERM, &sa, NULL); // 不使用signal是因为它会产生重入现象
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    openlog("netradio", LOG_PID | LOG_PERROR, LOG_DAEMON);
    /* 命令行分析 */
    while (1)
    {
        result = getopt(argc, argv, "M:P:FD:I:H");
        if (result < 0)
        {
            break;
        }
        switch (result)
        {
        case 'M':
            server_conf.mgroup = optarg;
            break;
        case 'P':
            server_conf.rcvport = optarg;
            break;
        case 'F':
            server_conf.runmode = RUN_FOREGROUND;
            break;
        case 'D':
            server_conf.media_dir = optarg;
            break;
        case 'I':
            server_conf.ifname = optarg;
            break;
        case 'H':
            printfhelp();
            exit(0);
        default:
            abort();
        }
    }
    syslog(LOG_INFO, "\n===============[server][main] getopt success=================\n");
    /* 守护进程的实现 */
    if (server_conf.runmode == RUN_DAEMON)
    {
        if (daemonize() != 0)
        {
            exit(1);
        }
        syslog(LOG_INFO, "\n[INFO]===============daemon process create success=================\n");
    }
    else if (server_conf.runmode == RUN_FOREGROUND)
    {
        /* do nothing */
    }
    else
    {
        // fprintf(stderr, "EINVAL\n");
        syslog(LOG_ERR, "[ERROR] EINVAL, server_conf.runmode is wrong.");
        exit(1);
    }
    /* SOCKET初始化 */
    socket_init();
    syslog(LOG_INFO, "\n===============[server][main] socket init success=================\n");
    /* 获取频道信息 */
    int list_size;
    int err;
    err = mlib_getchnlist(&list, &list_size);
    if (err)
    {
        syslog(LOG_ERR, "[server][main] fail to get channel list:%s.", strerror(err));
        exit(1);
    }
    syslog(LOG_INFO, "\n===============[server][main] get channel list success=================\n");
    /* 创建节目单线程 */
    err = thr_list_create(list, list_size);
    if (err)
    {
        exit(1);
    }
    syslog(LOG_INFO, "\n[server][main]create list thread success, list_size:%d\n", list_size);
    /* 创建频道线程 */
    int i;
    for (i = 0; i < list_size; i++)
    {
        err = thr_channel_create(list + i);
        if (err)
        {
            fprintf(stderr, "[server][main] create channel thread fail: %s\n", strerror(err));
            exit(1);
        }
    }
    syslog(LOG_INFO, "\n===============[server][main] create channel thread success=================\n");

    while (1)
        pause();

    exit(0);
}
