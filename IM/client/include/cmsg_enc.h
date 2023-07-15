#ifndef CMSG_ENC_H__
#define CMSG_ENC_H__

// #include "client_user.h"

int init_reg_struct(REG_MSG *reg, MYSELF *m);
int enc_reg_msg(char *buf, int *len, unsigned char stype);
void init_msg_header(MSG_HDR *h, unsigned char type, unsigned char stype, int n);
int init_login_struct(LOGIN_MSG *login, MYSELF *m);
int enc_login_msg(char *buf, int *len, unsigned char stype);
int init_chat_struct(CHAT_MSG *chat, MYSELF *m, char *msg, int rid);
int enc_chat_msg(char *buf, int *len, unsigned char stype, int rid, char *msg);
int init_logout_struct(LOGOUT_MSG *logout, MYSELF *m);
int enc_logout_msg(char *buf, int *len, unsigned char stype);
int init_fmgt_struct(FRND_OP *f, MYSELF *m, unsigned char stype, int *id, int count);
int enc_fmgt_msg(char *buf, int *len, unsigned char stype, int *id, int count);

#endif /* CMSG_ENC_H__ */