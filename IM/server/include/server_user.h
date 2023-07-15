#ifndef SERVER_USER_H__
#define SERVER_USER_H__

#include "im_server.h"
#define USER_REG (0x1 << 0)
#define USER_LOGIN (0x1 << 1)

typedef struct im_user
{
    char username[MAX_NAME_LEN];
    char password[MAX_PASSWD_LEN];
    int u_id;
    int u_stat;
    int u_socketfd;
    struct list_head u_frndhd;
    int u_frndcount;
} IM_USER;

typedef struct usr_frnd
{
    int f_id;
    struct list_head f_node;
} USR_FRND;

extern IM_USER all_users[MAX_USER_NUM];
extern int reg_user_num;

#define uflist_for_each_entry(pos, head, member) \
    for(pos=(USR_FRND *)list_entry((head)->next, USR_FRND, member); \
        &pos->member != (head); \
        pos = (USR_FRND *)list_entry(pos->member.next, USR_FRND, member))

int get_next_uid();
void reg_user_num_inc();
int init_all_usr_struct(int argc, char **argv);
void usr_fill_frndst(FRND_STAT *fs, int count, IM_USER *user);
void get_online_users(FRND_STAT *fs, int *count);
USR_FRND *user_friend_find(IM_USER *user, int uid);
int user_friend_add(IM_USER *user, int uid);
int user_friend_del(IM_USER *user, int uid);
void user_logout_exit(int uid);


#endif