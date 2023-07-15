#ifndef CLIENT_USER_H__
#define CLIENT_USER_H__

#include "msg_type.h"
#include "list.h"

typedef struct client_firend
{
    char c_name[MAX_NAME_LEN];
    int c_id;
    struct list_head c_node;
    int c_stat;
} CLIENT_FRIEND;

#define ufrnd_for_each_entry(pos, head, member)                                  \
    for (pos = (CLIENT_FRIEND *)list_entry((head)->next, CLIENT_FRIEND, member); \
         &pos->member != (head);                                                 \
         pos = (CLIENT_FRIEND *)list_entry(pos->member.next, CLIENT_FRIEND, member))

#define INIT_CSTAT 0 // 客户端初始化状态
#define LOGIN_CSTAT 1 // 客户登陆状态
#define MAX_MSG_SIZE 512
#define MSG_SNDING 0 // 消息发送状态
#define MSG_RCVING 1 // 消息接收状态

typedef struct whoami
{
    /* user id */
    int w_id;
    /* user name */
    char w_name[MAX_NAME_LEN];
    /* user password */
    char w_password[MAX_PASSWD_LEN];
    /* user socket fd*/
    int w_socketfd;
    /* user stat:init, login */
    int w_cstat;
    /* msg count */
    int w_msgcount;
    /* msg head */
    struct list_head w_msghead;
    /* all friends users list head */
    struct list_head w_flisthd;
    /* friend count */
    int w_fndcount;
    /* 消息发送缓冲区 */
    char w_buf[MAX_MSG_SIZE];
    /* message state: sending, recving */
    char w_msgstat;
    /* expected msg type */
    char w_msgtype;
} MYSELF;

extern MYSELF myself;

int init_user_struct(MYSELF *m);
int user_list_add(FRND_STAT *fs, short cnt);
int user_list_del(FRND_STAT *fs, short cnt);
int user_list_update(FRND_STAT *fs, short cnt);
int user_list_output(FRND_STAT *fs, short cnt);

#endif