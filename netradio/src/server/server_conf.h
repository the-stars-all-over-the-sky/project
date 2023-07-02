#ifndef SERVER_CONF_H__
#define SERVER_CONF_H__

#define DEFAULT_MEDIA_DIR "../../media/"
#define DEFAULT_IF "ens33"

enum
{
    RUN_DAEMON = 1, // 后台运行模式
    RUN_FOREGROUND // 前台运行模式
};

struct server_conf_st
{
    char *rcvport;
    char *mgroup;
    char *media_dir;
    char runmode;
    char *ifname;
};

extern struct server_conf_st server_conf;
extern int serversd;
extern struct sockaddr_in sndaddr;

#endif