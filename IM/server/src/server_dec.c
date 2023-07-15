#include "im_server.h"

int dec_reg_msg(char *buf, int n, REG_MSG **r)
{
    if (n != sizeof(REG_MSG))
    {
        printf("server: reg msg len check failed!\n");
        return -1;
    }

    printf("server: recv reg msg, len=%d\n", n);
    *r = (REG_MSG *)(buf + sizeof(MSG_HDR) - 1);

    return 0;
}
int dec_login_msg(char *buf, int n, LOGIN_MSG **l)
{
    if (n != sizeof(LOGIN_MSG))
    {
        printf("server: login msg len check failed!\n");
        return -1;
    }

    *l = (LOGIN_MSG *)(buf + sizeof(MSG_HDR) - 1);
    (*l)->lg_id = ntohl((*l)->lg_id);
    printf("server: rev login msg, len=%d, id=%d, passwd = %s\n", n, (*l)->lg_id, (*l)->lg_pwd);
    return 0;
}
int dec_chat_msg(char *buf, int n, CHAT_MSG **c, int *rid)
{
    if (n <= sizeof(CHAT_MSG) - 1 || n > MAX_CHAT_MSG)
    {
        printf("server: chat msg len check failed!\n");
        return -1;
    }
    printf("server: recv chat msg, len=%d\n", n);
    *c = (CHAT_MSG *)(buf + sizeof(MSG_HDR) - 1);
    *rid = ntohl((*c)->ch_rid);

    return 0;
}
int dec_logout_msg(char *buf, int n, LOGOUT_MSG **l)
{
    if (n != sizeof(LOGOUT_MSG))
    {
        printf("server: logout msg len check failed!\n");
        return -1;
    }
    printf("server: rev logout msg, len=%d\n", n);
    *l = (LOGOUT_MSG *)(buf + sizeof(MSG_HDR) - 1);
    (*l)->lg_id = ntohl((*l)->lg_id);
    return 0;
}
int dec_fmgt_msg(char *buf, int n, FRND_OP **f, int **fid, int *cnt)
{
    int *p;
    if (n != sizeof(FRND_OP) &&
        ((n - sizeof(FRND_OP)) % sizeof(int) != 0))
    {
        printf("serv: fmgt msg len check failed!\n");
        return -1;
    }
    printf("serv: rev fmgt msg, len=%d\n", n);
    *f = (FRND_OP *)(buf + sizeof(MSG_HDR) - 1);
    (*f)->f_id = ntohl((*f)->f_id);

    *cnt = (n - sizeof(FRND_OP) + sizeof((*f)->f_fids)) / sizeof(int);
    p = (*f)->f_fids;
    *fid = p;
    for (int i = 0; i < *cnt; i++)
    {
        *p = ntohl(*p);
        p++;
    }

    return 0;
}
// int dec_client_msgs(char *buf, int n);