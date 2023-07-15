#include "im_server.h"

/* 表示所有用户的结构体数组，数组索引和id相对应
1001 --- all_users[1]
 */
IM_USER all_users[MAX_USER_NUM];
int user_id = CONF_LOW_ID;
pthread_mutex_t uid_lock = PTHREAD_MUTEX_INITIALIZER;

int reg_user_num = 0;
pthread_mutex_t reg_user_lock = PTHREAD_MUTEX_INITIALIZER;

void reg_user_num_inc()
{
    pthread_mutex_lock(&reg_user_lock);
    reg_user_num++;
    pthread_mutex_unlock(&reg_user_lock);
}

int get_next_uid()
{
    pthread_mutex_lock(&uid_lock);
    user_id++;
    pthread_mutex_unlock(&uid_lock);
    return user_id;
}

int init_all_usr_struct(int argc, char **argv)
{
    IM_USER *user = all_users;
    memset(all_users, 0, sizeof(IM_USER) * MAX_USER_NUM);

    for (int i = 0; i < MAX_USER_NUM; i++)
    {
        user->u_id = CONF_LOW_ID + 1;
        user->u_socketfd = -1;
        INIT_LIST_HEAD(&user->u_frndhd);
        user++;
    }
    return 0;
}

void usr_fill_frndst(FRND_STAT *fs, int count, IM_USER *user)
{
    USR_FRND *pos;
    IM_USER *p;

    uflist_for_each_entry(pos, &user->u_frndhd, f_node)
    {
        p = &all_users[pos->f_id - CONF_LOW_ID];
        memset(fs, 0, sizeof(*fs));
        strncpy(fs->f_name, p->username, MAX_NAME_LEN - 1);
        fs->f_id = p->u_id;
        fs->f_stat = p->u_stat;
        fs++;
        count--;
        if (!count)
        {
            break;
        }
    }
}
void get_online_users(FRND_STAT *fs, int *count)
{
    IM_USER *p = all_users;
    int n = *count;

    memset(fs, 0, sizeof(*fs) * (*count));

    for (int i = 0; n > 0 && i < MAX_USER_NUM; i++, p++)
    {
        if (p->u_stat & USER_LOGIN)
        {
            fs->f_id = p->u_id;
            strncpy(fs->f_name, p->username, MAX_NAME_LEN - 1);
            fs->f_stat = p->u_stat;

            fs++;
            n--;
        }
    }
    *count = *count - n;
}
USR_FRND *user_friend_find(IM_USER *user, int uid)
{
    USR_FRND *pos;
    uflist_for_each_entry(pos, &user->u_frndhd, f_node)
    {
        if (pos->f_id == uid)
        {
            printf("server: user find! id = %d\n", pos->f_id);
            return pos;
        }
    }
    printf("server: user NOT found!\n");
    return NULL;
}

int user_friend_add(IM_USER *user, int uid)
{
    USR_FRND *uf;
    printf("server: entry usr_friend_add, uid = %d\n", uid);
    uf = user_friend_find(user, uid);
    printf("server: usr_friend_add\n");
    if (uf == NULL)
    {
        uf = (USR_FRND *)malloc(sizeof(*uf));
        if (uf == NULL)
        {
            return -1;
        }
        uf->f_id = uid;
        list_add(&uf->f_node, &user->u_frndhd);
        user->u_frndcount++;
        return 0;
    }
    return -1;
}

int user_friend_del(IM_USER *user, int uid)
{
    USR_FRND *uf;
    uf = user_friend_find(user, uid);
    if (uf == NULL)
    {
        list_del(&uf->f_node);
        user->u_frndcount--;
        free(uf);
        printf("server: user:%d del OK, memory free.\n", uid);
        return 0;
    }
    return -1;
}

void user_logout_exit(int uid)
{
    IM_USER *p;
    p = &all_users[uid - CONF_LOW_ID];
    p->u_stat &= ~USER_LOGIN;
    printf("server: logout uid=%d, stat=%x\n", uid, p->u_stat);

    close(p->u_socketfd);
}