#ifndef SERVER_ENC_H__
#define SERVER_ENC_H__

int init_reg_struct(REG_RESPONSE_MSG *r, int id, char *reason);
void init_msg_hdr(MSG_HDR *h, int n, unsigned char type, unsigned char stype);
int enc_reg_resp(char *buf, int *n, int id, char *reason);
int init_login_struct(LOGIN_RESPONSE_MSG *l, int stat, char *name, char *reason);
int enc_login_resp(char *buf, int *n, int stat, char *name, char *resaon);
int init_fmgt_struct(FRND_OP_RESPONSE *f, unsigned char stype, short stat, FRND_STAT *fs, short cnt);
int enc_fmgt_resp(char *buf, int *n, unsigned char stype, short stat, FRND_STAT *fs, short cnt);
int init_chat_struct(CHAT_RESPONCE *c, int stat, char *reason);
int enc_chat_resp(char *buf, int *n, int stat, char *reason);

#endif // !SERV_ENC_H