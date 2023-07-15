#include "im_client.h"

int init_reg_struct(REG_MSG *reg, MYSELF *m)
{
    memset(reg, 0, sizeof(*reg));
    memcpy(reg->r_name, m->w_name, MAX_NAME_LEN);
    strncpy(reg->r_password, m->w_password, MAX_PASSWD_LEN - 1);
    return sizeof(REG_MSG);
}

int enc_reg_msg(char *buf, int *n, unsigned char stype)
{
    *n = init_reg_struct((REG_MSG *)(((MSG_HDR *)buf)->msg_data), &myself);
    init_msg_header((MSG_HDR *)(buf), MSG_REG, stype, sizeof(REG_MSG));
    *n += (sizeof(MSG_HDR) - 1);

    return *n;
}

void init_msg_header(MSG_HDR *h, unsigned char type, unsigned char stype, int n)
{
    h->msg_type = htons((type << 8) | stype);
    h->msg_len = htons(n);
}

int init_login_struct(LOGIN_MSG *login, MYSELF *m)
{
    memset(login, 0, sizeof(*login));
    login->lg_id = htonl(m->w_id);
    strncpy(login->lg_pwd, m->w_password, MAX_PASSWD_LEN - 1);

    return sizeof(LOGIN_MSG);
}
/*
    封装登陆消息
    @buf ： 消息缓冲区
    @len ： 消息长度
    @stype ： 消息子类型
*/
int enc_login_msg(char *buf, int *len, unsigned char stype)
{
    *len = init_login_struct((LOGIN_MSG *)(((MSG_HDR *)buf)->msg_data), &myself);
    init_msg_header((MSG_HDR *)buf, MSG_LOGIN, stype, sizeof(LOGIN_MSG));

    *len += (sizeof(MSG_HDR) - 1);
    return *len;
}

int init_chat_struct(CHAT_MSG *chat, MYSELF *m, char *msg, int rid)
{
    memset(chat, 0, sizeof(*chat));
    chat->ch_sid = htonl(m->w_id);
    chat->ch_rid = htonl(rid);

    /* 加密的消息 */
    msg_encipher(msg, CIPHER_KEY);
    strncpy(chat->ch_msg, msg, strlen(msg));
    return sizeof(CHAT_MSG) + strlen(msg);
}
int enc_chat_msg(char *buf, int *len, unsigned char stype, int rid, char *msg)
{
    *len = init_chat_struct((CHAT_MSG *)(((MSG_HDR *)buf)->msg_data), &myself, msg, rid);
    init_msg_header((MSG_HDR *)buf, MSG_CHAT, stype, *len);
    *len += (sizeof(MSG_HDR) - 1);

    return *len;
}
int init_logout_struct(LOGOUT_MSG *logout, MYSELF *m)
{
    memset(logout, 0, sizeof(*logout));
    logout->lg_id = htonl(m->w_id);
    strncpy(logout->lg_pass, m->w_password, MAX_PASSWD_LEN - 1);
    return sizeof(LOGOUT_MSG);
}
int enc_logout_msg(char *buf, int *len, unsigned char stype)
{
    init_logout_struct((LOGOUT_MSG *)(((MSG_HDR *)buf)->msg_data), &myself);
    init_msg_header((MSG_HDR *)buf, MSG_LOGOUT, stype, sizeof(LOGOUT_MSG));
    *len = sizeof(LOGOUT_MSG) + sizeof(MSG_HDR) - 1;
    return *len;
}
int init_fmgt_struct(FRND_OP *f, MYSELF *m, unsigned char stype, int *id, int count)
{
    /*
        #define F_STAT 0x01       // 好友状态查询
        #define F_ALIST 0x02      // 好友列表查询
        #define F_ADD 0x03        // 增加好友
        #define F_DEL 0x04        // 删除好友
        #define F_LISTA_USER 0x05 // 列出所有用户
    */
    int *p = f->f_fids;
    f->f_id = htonl(m->w_id);
    for (int i = 0; i < count; i++, p++, id++)
    {
        *p = htonl(*id);
    }

    return sizeof(FRND_OP) + sizeof(int) * count - sizeof(f->f_fids);
}
int enc_fmgt_msg(char *buf, int *len, unsigned char stype, int *id, int count)
{
    *len = init_fmgt_struct((FRND_OP *)(((MSG_HDR *)buf)->msg_data), &myself, stype, id, count);
    printf("len : %d\n", *len);
    init_msg_header((MSG_HDR *)buf, MSG_FRNDMNG, stype, *len);
    *len += (sizeof(MSG_HDR) - 1);

    return *len;
}