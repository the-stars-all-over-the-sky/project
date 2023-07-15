#ifndef CMSG_DEC_H__
#define CMSG_DEC_H__

int dec_server_resp(char *buf, int len);
int dec_reg_resp(char *buf, int len, REG_RESPONSE_MSG **resp);
int dec_login_resp(char *buf, int len, LOGIN_RESPONSE_MSG **resp);
int dec_fmgt_resp(char *buf, int len, FRND_OP_RESPONSE **fr, FRND_STAT **fs, short *cnt);
int dec_chat_msg_or_resp(char *buf, int len, CHAT_RESPONCE **resp, CHAT_MSG **msg);

#endif /* CMSG_DEC_H__ */