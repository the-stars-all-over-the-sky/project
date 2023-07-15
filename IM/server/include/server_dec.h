#ifndef SERVER_DEC_H__
#define SERVER_DEC_H__

int dec_reg_msg(char *buf, int n, REG_MSG **r);
int dec_login_msg(char *buf, int n, LOGIN_MSG **l);
int dec_chat_msg(char *buf, int n, CHAT_MSG **c, int *rid);
int dec_logout_msg(char *buf, int n, LOGOUT_MSG **l);
int dec_fmgt_msg(char *buf, int n,FRND_OP **f, int **fid, int *cnt);
int dec_client_msgs(char *buf, int n);

#endif // !SERV_DEC_H