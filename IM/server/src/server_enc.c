#include "im_server.h"

void init_msg_hdr(MSG_HDR *h, int n, unsigned char type, unsigned char stype)
{
    /*
        大端字节序
        0000 0000 0000 0000        //short
        0000 0001                // uchar type
        0000 0010                // uchar stype

        0000 0001 0000 0000        //(u_short)type<<8
        0000 0000 0000 0010        //(u_stype)
        按位或
        0000 0001 0000 0010
    */
    h->msg_type = htons((type << 8) | stype);
    h->msg_len = htons(n);
}
int init_reg_struct(REG_RESPONSE_MSG *r, int id, char *reason)
{
    memset(r, 0, sizeof(*r));

    r->reponse_id = htonl(id);
    if (id == -1)
    {
        strncpy(r->err_reason, reason, MAX_ERR_LEN - 1);

        return sizeof(REG_RESPONSE_MSG) + strlen(reason);
    }
    return sizeof(REG_RESPONSE_MSG) - 1;
}
int enc_reg_resp(char *buf, int *n, int id, char *reason)
{
    *n = init_reg_struct((REG_RESPONSE_MSG *)((MSG_HDR *)buf)->msg_data, id, reason);

    init_msg_hdr((MSG_HDR *)buf, *n, MSG_REG, 0);
    *n += (sizeof(MSG_HDR) - 1);

    return *n;
}
int init_login_struct(LOGIN_RESPONSE_MSG *l, int stat, char *name, char *reason)
{
    memset(l, 0, sizeof(*l));
    l->lg_stat = htonl(stat);
    if (stat == LOGIN_ERR)
    {
        strncpy(l->lg_err_reason, reason, MAX_ERR_LEN - 1);
        return sizeof(LOGIN_RESPONSE_MSG) + strlen(reason);
    }
    if (name)
    {
        strncpy(l->lg_name, name, MAX_NAME_LEN - 1);
    }

    return sizeof(LOGIN_RESPONSE_MSG) - 1;
}
int enc_login_resp(char *buf, int *n, int stat, char *name, char *resaon)
{
    *n = init_login_struct((LOGIN_RESPONSE_MSG *)(((MSG_HDR *)buf)->msg_data), stat, name, resaon);
    init_msg_hdr((MSG_HDR *)buf, *n, MSG_LOGIN, 0);
    *n += (sizeof(MSG_HDR) - 1);
    printf("serv: enc_login_resp, n = %d\n", *n);
    return *n;
}
int init_fmgt_struct(FRND_OP_RESPONSE *f, unsigned char stype, short stat, FRND_STAT *fs, short cnt)
{
    int len = sizeof(FRND_OP_RESPONSE) - sizeof(f->f_ok_frnd);
    FRND_STAT *p = (FRND_STAT *)((char *)f + len);
    f->f_num = htons(cnt);
    f->f_stat = htons(stat);

    /*
        #define F_STAT 0x01       // 好友状态查询
        #define F_ALIST 0x02      // 好友列表查询
        #define F_ADD 0x03        // 增加好友
        #define F_DEL 0x04        // 删除好友
        #define F_LISTA_USER 0x05 // 列出所有用户
    */
    for (int i = 0; i < cnt; i++, p++, fs++)
    {
        p->f_id = htonl(fs->f_id);
        p->f_stat = htonl(fs->f_stat);
        memset(p->f_name, 0, MAX_NAME_LEN);
        strncpy(p->f_name, fs->f_name, MAX_NAME_LEN);

        len += sizeof(*p);
    }

    return len;
}
int enc_fmgt_resp(char *buf, int *n, unsigned char stype, short stat, FRND_STAT *fs, short cnt)
{
    *n = init_fmgt_struct((FRND_OP_RESPONSE *)(((MSG_HDR *)buf)->msg_data), stype, stat, fs, cnt);
    init_msg_hdr((MSG_HDR *)buf, *n, MSG_FRNDMNG, 0);
    printf("server: fgmt responce msg len is %d\n", *n);
    *n += sizeof(MSG_HDR) - 1;

    return *n;
}
int init_chat_struct(CHAT_RESPONCE *c, int stat, char *reason)
{
    memset(c, 0, sizeof(*c));
    c->c_stat = htonl(stat);
    if (stat == SND_ERR)
    {
        strncpy(c->c_msg, reason, MAX_ERR_LEN - 1);
        return sizeof(CHAT_RESPONCE) + strlen(reason);
    }

    return sizeof(CHAT_RESPONCE) - 1;
}
int enc_chat_resp(char *buf, int *n, int stat, char *reason)
{
    *n = init_chat_struct((CHAT_RESPONCE *)(((MSG_HDR *)buf)->msg_data), stat, reason);
    init_msg_hdr((MSG_HDR *)buf, *n, MSG_CHAT, 0);
    *n += sizeof(MSG_HDR) - 1;

    return *n;
}