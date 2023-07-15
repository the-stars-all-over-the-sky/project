#ifndef CUSR_CMD_H__
#define CUSR_CMD_H__

#define C_LOCAL 1
#define C_SERVER 2

typedef struct user_cmd
{
    char *u_str;
    short u_local;
    short u_mtype;
    int u_optcnt;
} USR_CMD;

#define MAX_LINE_LEN 256
#define MAX_PROMPT_LEN 64

#define REG_CMD_FLDS 4
#define LOGIN_CMD_FLDS 3
#define FLIST_CMD_FLDS 1
#define ADD_CMD_FLDS 2
#define DEL_CMD_FLDS 2
#define ALIST_CMD_FLDS 1
#define FSTAT_CMD_FLDS 2
#define CHAT_CMD_FLDS 2
#define HELP_CMD_FLDS 1
#define DEBUG_CMD_FLDS 1
#define MESG_CMD_FLDS 3
#define LOGOUT_CMD_FLDS 1

#define USER_PROMPT "im_client"

#define REG_CMD "reg"
#define LIN_CMD "login"
#define FLST_CMD "flist"
#define FADD_CMD "add"
#define FDEL_CMD "del"
#define ALIST_CMD "alist"
#define FSTAT_CMD "stat"
#define CHAT_CMD "talk"
#define EXIT_CMD "exit"
#define HELP_CMD "help"
#define MESG_CMD "mesg" // 聊天记录
#define DEBUG_CMD "debug"

#define REG_C_USAGE "reg <name> <password> <password>      通过用户名和密码注册用户。\n"
#define LOGIN_C_USAGE "login <uid> <password>   用户登陆。\n"
#define FLIST_C_USAGE "flist      列出所有的好友用户。\n"
#define FADD_C_USAGE "add <uid>  根据用户id添加用户。\n"
#define FDEL_C_USAGE "del <uid>  根据好友id删除好友。\n"
#define ALIST_C_USAGE "alist  查询所有的在线用户。\n"
#define FSTAT_C_USAGE "stat <uid>  查询好友状态。\n"
#define CHAT_C_USAGE "talk <uid> <msg>  发送消息给好友。\n"
#define EXIT_C_USAGE "exit     用户退出\n"
#define HELP_C_USAGE "help  打印帮助信息。\n"
#define MESG_C_USAGE "mesg <from> <to>  显示从from用户到to用户的聊天记录。\n"
#define DEBUG_C_USAGE "debug  显示当前用户信息。\n"

#define CLIENT_C_USAGE "客户端聊天软件命令帮助（<>为必选参数）。\n"
#define CLIENT_USAGE \
    REG_C_USAGE      \
    LOGIN_C_USAGE    \
    FLIST_C_USAGE      \
    FADD_C_USAGE       \
    ALIST_C_USAGE    \
    FSTAT_C_USAGE    \
    CHAT_C_USAGE     \
    EXIT_C_USAGE     \
    HELP_C_USAGE     \
    MESG_C_USAGE     \
    DEBUG_C_USAGE    \
    CLIENT_C_USAGE   \
    CLIENT_C_USAGE

#define INV_C_USAGE "无法解析输入的命令\n\n"
#define INV_STAT_USAGE "请先登陆\n\n"

extern USR_CMD all_cmds[];

/* 对命令的各个字段进行分解 */
/*
@next_token 去除了命令关键字的命令行字符串指针
@p 各个字段的指针数组
@cnt 数组的成员个数
@strict 格式是否有严格的格式标志
*/
int cmdline_proc(char *next_token, char **p, int cnt, int strict);
int client_send(char *buf, int n);
int reg_cmd_proc(char *next_token);
int login_cmd_proc(char *next_token);
int mesg_cmd_proc(char *next_token);
int flist_cmd_proc(char *next_token);
int add_cmd_proc(char *next_token);
int del_cmd_proc(char *next_token);
int alist_cmd_proc(char *next_token);
int fstat_cmd_proc(char *next_token);
int chat_cmd_proc(char *next_token);
int logout_cmd_proc(char *next_token);
int help_cmd_proc(char *next_token);
/*
命令处理函数
@buf： 用户输入的命令字符串缓冲区
*/
int usr_cmd_process(char *buf);
int debug_cmd_proc(char *next_token);
int usr_stat_check(char *token);


#endif /* CUSR_CMD_H__ */