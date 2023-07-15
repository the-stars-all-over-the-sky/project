#include "im_client.h"

/*
对收到的消息进行统一处理
处理流程包含获取消息的类型、子类型、消息长度
判断消息长度字段是否合法

switch 对不同的消息进行不同的处理
dec_reg_resp();
 */

int dec_server_resp(char *buf, int len)
{
    unsigned char type;
    REG_RESPONSE_MSG *reg_response;
    LOGIN_RESPONSE_MSG *login_response;
    FRND_OP_RESPONSE *frnd_op_response;
    FRND_STAT *frnd_status;
    CHAT_MSG *chat_msg;
    CHAT_RESPONCE *chat_response;

    short cnt;
    MSG_HDR *head = (MSG_HDR *)buf;
    short m_type, m_len;
    unsigned char stype;

    m_type = ntohs(head->msg_type);
    m_len = ntohs(head->msg_len);
    stype = m_type & 0xff;
    if (len != sizeof(MSG_HDR) - 1 + m_len)
    {
        printf("client: response msg len check error!\n");
        return -1;
    }
    /*
        0x03
        0000 0011

        0x02
        0000 0010

        short 2 byte

        0000 0011 0000 0010

        0000 0000 0000 0011
        0xff
        1111 1111 1111 1111

        0x03

        0xff =255
        0x7f = 127
    */
    type = (m_type >> 8) & 0xff;

    switch (type & 0x7f)
    {
    case MSG_REG:
        if (dec_reg_resp(buf, m_len, &reg_response) >= 0)
        {
            handle_reg_resp(reg_response);
        }
        break;
    case MSG_LOGIN:
        if (dec_login_resp(buf, m_len, &login_response) >= 0)
        {
            handle_login_resp(login_response);
        }
        break;
    case MSG_FRNDMNG:
        if (dec_fmgt_resp(buf, m_len, &frnd_op_response, &frnd_status, &cnt) >= 0)
        {
            handle_fmgt_resp(stype, frnd_op_response, frnd_status, cnt);
        }
        break;
    case MSG_CHAT:
        if (dec_chat_msg_or_resp(buf, m_len, &chat_response, &chat_msg) >= 0)
        {
            handle_chat_msg_or_response(chat_msg, chat_response);
        }
        break;
    default:
        break;
    }
    return 0;
}

int dec_reg_resp(char *buf, int len, REG_RESPONSE_MSG **resp)
{
    MSG_HDR *head = (MSG_HDR *)buf;
    REG_RESPONSE_MSG *r = (REG_RESPONSE_MSG *)((char *)(&(head->msg_data)));
    if (len > sizeof(REG_RESPONSE_MSG) - 1)
    {
        if (len > sizeof(REG_RESPONSE_MSG) - 1 + MAX_ERR_LEN ||
            len != sizeof(REG_RESPONSE_MSG) + strlen(r->err_reason))
        {
            printf("client: reg resp, invalid msg len = %d\n", len);
            return -1;
        }
    }
    else if (len != sizeof(REG_RESPONSE_MSG) - 1)
    {
        printf("client:reg resp2, invalid msg len = %d\n", len);
    }
    /*
        0000 0000 1111 1111	大端
        1111 1111 0000 0000 小端
    */
    r->reponse_id = ntohl(r->reponse_id);
    *resp = r;
    return 0;
}
int dec_login_resp(char *buf, int len, LOGIN_RESPONSE_MSG **resp)
{
    MSG_HDR *head = (MSG_HDR *)buf;
    *resp = (LOGIN_RESPONSE_MSG *)((char *)(&(head->msg_data)));
    if (len > sizeof(LOGIN_RESPONSE_MSG) - 1)
    {
        if (len > sizeof(LOGIN_RESPONSE_MSG) - 1 + MAX_ERR_LEN ||
            len != sizeof(LOGIN_RESPONSE_MSG) + strlen((*resp)->lg_err_reason))
        {
            printf("client: login resp ,invalid msg len=%d\n", len);
            return -1;
        }
        else if (len != sizeof(LOGIN_RESPONSE_MSG) - 1)
        {
            printf("client: login resp2,invalid msg len=%d\n", len);
            return -1;
        }
    }
    (*resp)->lg_stat = ntohl((*resp)->lg_stat);
    return 0;
}
int dec_fmgt_resp(char *buf, int len, FRND_OP_RESPONSE **fr, FRND_STAT **fs, short *cnt)
{
    MSG_HDR *head = (MSG_HDR *)buf;
    FRND_OP_RESPONSE *resp = (FRND_OP_RESPONSE *)((char *)(&(head->msg_data)));
    FRND_STAT *s = (FRND_STAT *)((char *)resp + sizeof(short) * 2);

    *cnt = (len - sizeof(short) * 2) / sizeof(FRND_STAT);

    if (len > sizeof(FRND_OP_RESPONSE) - sizeof(FRND_STAT))
    {
        if ((len - sizeof(short) * 2) % sizeof(FRND_STAT) != 0 ||
            *cnt != ntohs(resp->f_num))
        {
            printf("client: fmgt resp, invalid len = %d, cnt = %d\n", len, *cnt);
            return -1;
        }
    }
    else if (len != sizeof(FRND_OP_RESPONSE) - sizeof(FRND_STAT))
    {
        printf("client: fmgt resp2 ,invalid len=%d\n", len);
        return -1;
    }

    resp->f_stat = ntohs(resp->f_stat);
    *fr = resp;
    *fs = s;

    for (int i = 0; i < *cnt; i++, s++)
    {
        s->f_id = ntohl(s->f_id);
        s->f_stat = ntohl(s->f_stat);
        printf("server: user id = %d, stat = %d\n", s->f_id, s->f_stat);
    }
    return 0;
}
int dec_chat_msg_or_resp(char *buf, int len, CHAT_RESPONCE **resp, CHAT_MSG **msg)
{
    MSG_HDR *head = (MSG_HDR *)buf;
    short msg_type = ntohs(head->msg_type);

    /*
        0000 0000 0000 0001
        1000 0000 0000 0000

        0x04
        0000 0000 0000 0100

        MSG_CHAT X
        0000 0100 0000 0000
        1000 0000 0000 0000


        server :用户发送消息 MSG_CHAT | SERVER_FORWARD_FLAG
        0000 0100 0000 0000
        1000 0000 0000 0000
        实际收到的标志位消息↓
        1000 0100 0000 0000



        server :服务器回复消息 MSG_CHAT
        实际收到的标志位消息↓
        MSG_CHAT
        0000 0100 0000 0000
        1000 0000 0000 0000
    */
    /*msg_type：1000 0100 0000 0000*/
    if (msg_type & SERVER_FORWARD_FLAG)
    {
        printf("client: real chat msg, len= %d\n", len);
        *resp = NULL;
        *msg = (CHAT_MSG *)(&(head->msg_data));
    }
    else
    {
        printf("client: chat resp msg, len=%d\n", len);
        *msg = NULL;
        *resp = (CHAT_RESPONCE *)(&(head->msg_data));
        (*resp)->c_stat = ntohl((*resp)->c_stat);
        printf("resp->c_stat = %d\n", (*resp)->c_stat);
    }
    if (*resp)
    {
        if (len > sizeof(CHAT_RESPONCE) - 1)
        {
            if (len > sizeof(CHAT_RESPONCE) - 1 + MAX_ERR_LEN ||
                len != sizeof(CHAT_RESPONCE) + strlen((*resp)->c_msg))
            {
                printf("client: chat resp, invalid msg len = %d\n", len);
                return -1;
            }
            else if (len != sizeof(CHAT_RESPONCE) - 1)
            {
                printf("client: chat resp2,invalid msg len=%d\n", len);
                return -1;
            }
        }
    }
    else
    {
        if (len > MAX_MSG_SIZE || len < sizeof(CHAT_MSG) - 1)
        {
            printf("client: chat msg, invalid msg len = %d\n", len);
            return -1;
        }
        (*msg)->ch_sid = ntohl((*msg)->ch_sid);
        (*msg)->ch_rid = ntohl((*msg)->ch_rid);
        // printf("client: msg->ch_sid =  %d\n", (*msg)->ch_sid);
        // printf("client: msg->ch_rid =  %d\n", (*msg)->ch_rid);
        // printf("client: msg->ch_msg =  %s\n", (*msg)->ch_msg);
    }
    return 0;
}