#include "im_server.h"

/*
服务器处理某个用户的子线程调用recv函数接收消息
调用handle_client_msgs对消息进行初步解析，判断消息长度是否合法，从消息头中获取消息长度及类型
根据不同的消息类型，调用对应的函数解析
若消息解析成功，调用对应的消息处理函数
处理完成后，再调用recv函数接收下一条消息
*/

int serv_snd_msg(int *usr_id, int fd, char *buf, int n)
{
    if (send(fd, buf, n, 0) != n)
    {
        printf("server: send msg error, user exit......\n");
        if (*usr_id != 0)
        {
            user_logout_exit(*usr_id);
        }
        return -2;
    }
    printf("server: serv_snd_msg, n= %d\n ", n);
    return n;
}

int handle_reg_msg(int *usr_id, int fd, REG_MSG *r)
{
    char buf[MAX_MSG_SIZE];
    char err[MAX_ERR_LEN];
    int uid = get_next_uid(); // 1001
    int n;
    int ret;

    IM_USER *user = &all_users[uid - CONF_LOW_ID];
    memset(buf, 0, MAX_MSG_SIZE);
    memset(err, 0, MAX_ERR_LEN);
    if (reg_user_num >= MAX_USER_NUM)
    {
        printf("server: reg failed, max num user reached!\n");
        strncpy(err, "max num user reached", MAX_ERR_LEN - 1);
        enc_reg_resp(buf, &n, -1, err);
        return serv_snd_msg(usr_id, fd, buf, n);
    }

    enc_reg_resp(buf, &n, uid, NULL);
    ret = serv_snd_msg(usr_id, fd, buf, n);
    if (ret > 0)
    {
        memset(user->username, 0, MAX_NAME_LEN);
        strncpy(user->username, r->r_name, MAX_NAME_LEN - 1);
        memset(user->password, 0, MAX_PASSWD_LEN);
        strncpy(user->password, r->r_password, MAX_PASSWD_LEN - 1);

        user->u_id = uid;
        user->u_stat = USER_REG;
        user->u_socketfd = fd;

        reg_user_num_inc();
    }
    printf("server: send reg resp, n=%d\n", n);
    return ret;
}

int handle_login_msg(int *usr_id, int fd, LOGIN_MSG *l)
{
    int uid = l->lg_id;
    IM_USER *u = &all_users[uid - CONF_LOW_ID];
    char buf[MAX_MSG_SIZE];
    // char err[MAX_ERR_LEN];
    int n;
    int ret;
    printf("server: %s\n", l->lg_pwd);
    printf("server: %s\n", u->password);
    int cmp = strcmp(u->password, l->lg_pwd);
    printf("server: handle login msg from clientr.\n");
    memset(buf, 0, MAX_MSG_SIZE);
    if ((u->u_stat & USER_REG) && !(u->u_stat & USER_LOGIN) && !cmp)
    {
        enc_login_resp(buf, &n, LOGIN_OK, u->username, NULL);
    }
    ret = serv_snd_msg(&uid, fd, buf, n);
    if (ret > 0)
    {
        u->u_socketfd = fd;
        u->u_stat |= USER_LOGIN;
        *usr_id = uid;
    }
    printf("server: send login resp(login ok), n=%d\n", n);
    return ret;
}

int handle_chat_msg(int *usr_id, int fd, char *buf, int n, CHAT_MSG *c, int rid)
{
    IM_USER *u = &all_users[rid - CONF_LOW_ID];
    int ret;
    char rbuf[MAX_MSG_SIZE];
    MSG_HDR *h;

    memset(rbuf, 0, MAX_MSG_SIZE);
    if (!(u->u_stat & USER_LOGIN) || u->u_socketfd < 0)
    {
        ret = -1;
        printf("serv: char msg snd failed, friend not login!\n");
        enc_chat_resp(rbuf, &n, SND_ERR, "friend not login!");
    }
    else
    {
        h = (MSG_HDR *)buf;
        h->msg_type = htons(ntohs(h->msg_type) | SERVER_FORWARD_FLAG);
        printf("begin send msg from %d to %d!!!\n", *usr_id, rid);
        ret = serv_snd_msg(usr_id, u->u_socketfd, buf, n);
        if (ret <= 0)
        {
            user_logout_exit(rid);
            enc_chat_resp(rbuf, &n, SND_ERR, "chat msg snd err!\n");
        }
        else
        {
            enc_chat_resp(rbuf, &n, SND_OK, NULL);
        }
    }
    printf("serv: for chat resp, rbuf = %s, n=%d\n", rbuf, n);
    return serv_snd_msg(usr_id, fd, rbuf, n);
}

int handle_logout_msg(int *usr_id, int fd, LOGOUT_MSG *l)
{
    int uid = l->lg_id;
    IM_USER *u = &all_users[uid - CONF_LOW_ID];

    /*
    0000 0000 0000 0001
    1111 1111 1111 1110

    */

    u->u_stat &= ~USER_LOGIN;
    return 0;
}

int handle_fmgt_msg(int *usr_id, int fd, unsigned char stype, FRND_OP *f, int *fid, int cnt)
{
    int uid = f->f_id;
    IM_USER *u = &all_users[uid - CONF_LOW_ID];

    char buf[MAX_MSG_SIZE];
    int friend_cnt = u->u_frndcount;
    int n;
    FRND_STAT *fs;
    USR_FRND *uf;
    FRND_STAT fnds;
    FRND_STAT users_ret[MAX_USER_SIZE];
    int user_cnt = MAX_USER_SIZE;
    int ret;
    if (cnt != 1 && cnt != 0)
    {
        printf("serv: fmgt msg, invalid cnt=%d\n", cnt);
        return -1;
    }
    /*
        #define F_STAT 0x01       // 好友状态查询
        #define F_ALIST 0x02      // 好友列表查询
        #define F_ADD 0x03        // 增加好友
        #define F_DEL 0x04        // 删除好友
        #define F_LISTA_USER 0x05 // 列出所有用户
    */
    if (stype == F_LISTA_USER)
    {
        printf("serv: user list reg msg\n");
        fs = malloc(friend_cnt * sizeof(FRND_STAT));
        if (fs == NULL)
        {
            printf("serv: memory allocation failed for frnd_st\n");
            return -1;
        }
        usr_fill_frndst(fs, friend_cnt, u);
        enc_fmgt_resp(buf, &n, F_LISTA_USER, OP_ALL_OK, fs, friend_cnt);
    }
    else if (stype == F_ADD)
    {
        printf("serv: user add msg\n");
        // rintf("fid=%d\n", *fid);
        ret = user_friend_add(u, *fid);
        printf("out of usr_friend_add\n");
        if (ret < 0)
        {
            printf("serv: user alread exist in friend list.\n");
            enc_fmgt_resp(buf, &n, F_ADD, OP_ALL_FAIL, NULL, 0);
        }
        else
        {
            memset(&fnds, 0, sizeof(fnds));
            u = &all_users[*fid - CONF_LOW_ID];
            strncpy(fnds.f_name, u->username, MAX_NAME_LEN - 1);
            fnds.f_id = u->u_id;
            fnds.f_stat = u->u_stat;
            u->u_frndcount++;
            printf("serv: entry enc_fmgt_resp\n");
            enc_fmgt_resp(buf, &n, F_ADD, OP_ALL_OK, &fnds, 1);
        }
    }
    else if (stype == F_DEL)
    {

        printf("serv: user del msg\n");
        ret = user_friend_del(u, *fid);
        printf("serv: usr_friend_del return %d\n", ret);
        if (ret < 0)
        {
            printf("serv: user NOT exist in friend list.\n");
            enc_fmgt_resp(buf, &n, F_DEL, OP_ALL_FAIL, NULL, 0);
        }
        else
        {
            memset(&fnds, 0, sizeof(fnds));
            u = &all_users[*fid - CONF_LOW_ID];
            strncpy(fnds.f_name, u->username, MAX_NAME_LEN - 1);
            fnds.f_id = u->u_id;
            fnds.f_stat = u->u_stat;
            u->u_frndcount--;
            printf("serv: entry enc_fmgt_resp[F_DEL] ALL_OK\n");
            enc_fmgt_resp(buf, &n, F_DEL, OP_ALL_OK, &fnds, 1);
        }
    }
    else if (stype == F_ALIST)
    {
        printf("serv: user ALIST msg\n");
        get_online_users(users_ret, &user_cnt);
        enc_fmgt_resp(buf, &n, F_ALIST, OP_ALL_OK, users_ret, user_cnt);
    }
    else if (stype == F_STAT)
    {
        printf("serv: user STAT msg\n");
        uf = user_friend_find(u, *fid);
        if (uf == NULL)
        {
            enc_fmgt_resp(buf, &n, F_STAT, OP_ALL_FAIL, NULL, 0);
        }
        else
        {
            memset(&fnds, 0, sizeof(fnds));
            u = &all_users[*fid - CONF_LOW_ID];
            strncpy(fnds.f_name, u->username, MAX_NAME_LEN - 1);
            fnds.f_id = u->u_id;
            fnds.f_stat = u->u_stat;

            enc_fmgt_resp(buf, &n, F_STAT, OP_ALL_OK, &fnds, 1);
        }
    }

    printf("serv: sending fmgt resp, n=%d\n", n);
    return serv_snd_msg(usr_id, fd, buf, n);
}

int handle_client_msgs(int *uid, int fd, char *buf, int n)
{
    unsigned char type, stype;
    REG_MSG *r;
    LOGIN_MSG *li;
    CHAT_MSG *c;
    LOGOUT_MSG *lo;
    FRND_OP *f;

    MSG_HDR *h = (MSG_HDR *)buf;

    int *fid, cnt, rid;

    short m_type, m_len;
    m_type = ntohs(h->msg_type);
    m_len = ntohs(h->msg_len);

    if (n != sizeof(MSG_HDR) - 1 + m_len)
    {
        printf("serv: msg len sanity check failed!(%d %ld)\n", n, sizeof(MSG_HDR) - 1 + m_len);
        return -1;
    }
    type = (m_type >> 8) & 0xff; // 0000 0011
    stype = m_type & 0xff;       // 0000 0010

    switch (type)
    {
    case MSG_REG:
        if (dec_reg_msg(buf, m_len, &r) >= 0)
        {
            return handle_reg_msg(uid, fd, r);
        }
        break;

    case MSG_LOGIN:
        if (dec_login_msg(buf, m_len, &li) >= 0)
        {
            return handle_login_msg(uid, fd, li);
        }
        break;

    case MSG_FRNDMNG:
        if (dec_fmgt_msg(buf, m_len, &f, &fid, &cnt) >= 0)
        {
            return handle_fmgt_msg(uid, fd, stype, f, fid, cnt);
        }
        break;

    case MSG_CHAT:
        if (dec_chat_msg(buf, m_len, &c, &rid) >= 0)
        {
            printf("server: h->msg_len =  %d\n", (h)->msg_len);
            CHAT_MSG *msg = (CHAT_MSG *)(&h->msg_data);
            printf("server: msg->ch_sid =  %d\n", ntohl((msg)->ch_sid));
            printf("server: msg->ch_rid =  %d\n", ntohl((msg)->ch_rid));
            printf("server: msg->ch_msg =  %s\n", (msg)->ch_msg);
            return handle_chat_msg(uid, fd, buf, n, c, rid);
        }
        break;

    case MSG_LOGOUT:
        if (dec_logout_msg(buf, m_len, &lo) >= 0)
        {
            return handle_logout_msg(uid, fd, lo);
        }
        break;

    default:
        return -1;
    }

    return -1;
}