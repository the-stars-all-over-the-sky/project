#ifndef CLIENT_HANDLE_H__
#define CLIENT_HANDLE_H__

int handle_reg_resp(REG_RESPONSE_MSG *reg_response);
int handle_login_resp(LOGIN_RESPONSE_MSG *login_response);
int handle_fmgt_resp(unsigned short stype, FRND_OP_RESPONSE *fr, FRND_STAT *fs, short cnt);
int handle_chat_msg_or_response(CHAT_MSG *msg, CHAT_RESPONCE *resp);
int handle_chat_resp(CHAT_RESPONCE *resp);
int handle_chat_msg(CHAT_MSG *msg);

#endif