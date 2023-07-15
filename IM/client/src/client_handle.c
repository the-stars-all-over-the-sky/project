#include "im_client.h"

int handle_reg_resp(REG_RESPONSE_MSG *reg_response)
{
    myself.w_id = reg_response->reponse_id;
    printf("client: recv reg msg from server. uid=%d\n", reg_response->reponse_id);

    if (reg_response->reponse_id == -1)
    {
        fprintf(stderr, "%s register failed,reason:%s", myself.w_name, reg_response->err_reason);
    }

    return 0;
}
int handle_login_resp(LOGIN_RESPONSE_MSG *login_response)
{
    printf("client: revc login resp from server.\n");
    if (login_response->lg_stat == LOGIN_OK)
    {
        myself.w_cstat = LOGIN_CSTAT;
        memset(myself.w_name, 0, MAX_NAME_LEN);
        strncpy(myself.w_name, login_response->lg_name, MAX_NAME_LEN - 1);
        return 0;
    }
    fprintf(stderr, "user %s login failed, reason:%s\n", myself.w_name, login_response->lg_err_reason);
    return -1;
}
int handle_fmgt_resp(unsigned short stype, FRND_OP_RESPONSE *fr, FRND_STAT *fs, short cnt)
{
    /*
        #define F_STAT 0x01       // 好友状态查询
        #define F_ALIST 0x02      // 好友列表查询
        #define F_ADD 0x03        // 增加好友
        #define F_DEL 0x04        // 删除好友
        #define F_LISTA_USER 0x05 // 列出所有用户
    */
    switch (stype)
    {
    case F_LISTA_USER:
        /* code */
        break;
    case F_ADD:
        user_list_add(fs, cnt);
        break;
    case F_DEL:
        user_list_del(fs, cnt);
        break;
    case F_ALIST:
        user_list_output(fs, cnt);
        break;
    case F_STAT:
        user_list_update(fs, cnt);
        break;
    default:
        break;
    }
    return 0;
}

int handle_chat_msg_or_response(CHAT_MSG *msg, CHAT_RESPONCE *resp)
{
    /*
        其他用户---> 自己的
        自己发送给别人的消息----->服务器反馈发送是否成功
    */
    if (resp)
    {
        printf("sueccess send msg to friend\n");
        return handle_chat_resp(resp);
    }
    printf("receive msg from friend\n");
    return handle_chat_msg(msg);
}

int handle_chat_resp(CHAT_RESPONCE *resp)
{
    if (resp->c_stat == SND_OK)
    {
        printf("client: chat msg send ok\n");
        return 0;
    }

    fprintf(stderr, "chat msg send error, reason: %s", resp->c_msg);
    return -1;
}

int handle_chat_msg(CHAT_MSG *msg)
{
    msg_decipher(msg->ch_msg, CIPHER_KEY);
    printf("client: chat msg from %d : %s\n", msg->ch_sid, msg->ch_msg);
    client_log_save(msg->ch_msg, msg->ch_sid, msg->ch_rid);
    return 0;
}