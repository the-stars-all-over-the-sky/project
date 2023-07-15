#include "im_client.h"

/*
1、调用fgets函数从标准输入中获取用户输入的命令，并对该命令进行预处理
2、__strtok_r函数将用户命令的第一个字段解析出来（命令关键字解析）
3、用一个多分支的if-else语句判断命令的类型，根据命令调用不同的命令处理参数
4、在命令处理函数中调用cmdline_proc函数对用户的命令关键字后的各字段进行提取
5、若用户输入的命令字段合法，对字段内容进行合法性验证
6、根据命令类型判断，改命令是网络命令还是本地命令
    若为网络命令，则对对应的消息进行封装，然后发往服务器
    若为本地命令，则调用相应的程序进行处理，然后命令处理完毕，再转至第一步，继续等待用户输入的下一个命令进行处理
*/

int cmdline_proc(char *next_token, char **p, int cnt, int strict)
{
    int n;
    char *token = NULL;
    char spes[] = "\t ";
    if (strict)
    {
        n = cnt;
    }
    else
    {
        n = cnt - 1;
    }
    for (int i = 0; i < n; i++)
    {
        token = __strtok_r(NULL, spes, &next_token);
        p[i] = token;
        /* 用户提供的命令参数不够 */
        if (i != n - 1 && !p[i])
        {
            return -1;
        }
    }

    if (strict && p[cnt - 1])
    {
        return -1;
    }
    if (!strict)
    {
        p[cnt - 1] = next_token;
        if (!next_token)
        {
            return -1;
        }
    }
    return 0;
}

int client_send(char *buf, int n)
{
    int len;
    len = send(myself.w_socketfd, buf, n, 0);
    if (len != n)
    {
        close(myself.w_socketfd);
        init_user_struct(&myself);
    }
    return len;
}

int reg_cmd_proc(char *next_token)
{
    int n;
    char *p[REG_CMD_FLDS];
    if (cmdline_proc(next_token, p, REG_CMD_FLDS, 1) < 0)
    {
        fprintf(stderr, "%s%s\n", INV_C_USAGE, REG_C_USAGE);
        return -1;
    }

    if (strlen(p[0]) > MAX_NAME_LEN - 1)
    {
        fprintf(stderr, "name is to long, max = %d characters allowed\n%s", MAX_NAME_LEN - 1, REG_C_USAGE);
    }

    if (strcmp(p[1], p[2]))
    {
        fprintf(stderr, "passwd must match\n %s", REG_C_USAGE);
    }

    if (strlen(p[1]) > MAX_PASSWD_LEN - 1)
    {
        fprintf(stderr, "passwd to long , max = %d characters allowed\n%s", MAX_PASSWD_LEN - 1, REG_C_USAGE);
    }

    strcpy(myself.w_name, p[0]);
    strcpy(myself.w_password, p[1]);

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    enc_reg_msg(myself.w_buf, &n, 0);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;

    return 0;
}

int login_cmd_proc(char *next_token)
{
    int id;
    int n;
    char *p[LOGIN_CMD_FLDS];

    int result = (cmdline_proc(next_token, p, LOGIN_CMD_FLDS, 1) == -1 || !(id = atoi(p[0])) || strlen(p[1]) > MAX_PASSWD_LEN - 1);
    if (result)
    {
        fprintf(stderr, "client: %s%s", INV_C_USAGE, LOGIN_C_USAGE);
        return -1;
    }
    myself.w_id = id;
    memset(myself.w_password, 0, MAX_PASSWD_LEN);
    strncpy(myself.w_password, p[1], MAX_PASSWD_LEN - 1);

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    enc_login_msg(myself.w_buf, &n, 0);
    client_send(myself.w_buf, n);

    myself.w_msgstat = MSG_RCVING;
    return 0;
}

int mesg_cmd_proc(char *next_token)
{
    char *p[MESG_CMD_FLDS];
    int from, to;
    FILE *fp;

    if (cmdline_proc(next_token, p, MESG_CMD_FLDS, 1) == -1 || !(from = atoi(p[0])) || !(to = atoi(p[1])))
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, MESG_C_USAGE);
        return -1;
    }

    fp = fopen(CLIENT_LOG_FILE, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "open file \"%s\" failed!\n", CLIENT_LOG_FILE);
        return -1;
    }

    client_log_export(fp, stdout, from, to);

    fclose(fp);
    return 0;
}

int flist_cmd_proc(char *next_token)
{
    int n;
    char *p[FLIST_CMD_FLDS];
    if (cmdline_proc(next_token, p, FLIST_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, FLIST_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    enc_fmgt_msg(myself.w_buf, &n, F_LISTA_USER, NULL, 0);

    client_send(myself.w_buf, n);

    myself.w_msgstat = MSG_RCVING;
    return 0;
}

int add_cmd_proc(char *next_token)
{
    int n, uid;
    char *p[ADD_CMD_FLDS];

    if (cmdline_proc(next_token, p, ADD_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, FADD_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    uid = atoi(p[0]);

    enc_fmgt_msg(myself.w_buf, &n, F_ADD, &uid, 1);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;
    myself.w_fndcount++;

    return 0;
}

int del_cmd_proc(char *next_token)
{
    int n, uid;
    char *p[DEL_CMD_FLDS];

    if (cmdline_proc(next_token, p, DEL_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, FDEL_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    uid = atoi(p[0]);

    enc_fmgt_msg(myself.w_buf, &n, F_DEL, &uid, 1);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;

    return 0;
}

int alist_cmd_proc(char *next_token)
{
    int n;
    char *p[ALIST_CMD_FLDS];

    if (cmdline_proc(next_token, p, ALIST_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, ALIST_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);

    enc_fmgt_msg(myself.w_buf, &n, F_ALIST, NULL, 0);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;
    return 0;
}

int fstat_cmd_proc(char *next_token)
{
    int n, uid;
    char *p[FSTAT_CMD_FLDS];

    if (cmdline_proc(next_token, p, FSTAT_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, FSTAT_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    uid = atoi(p[0]);

    enc_fmgt_msg(myself.w_buf, &n, F_STAT, &uid, 1);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;

    return 0;
}

int chat_cmd_proc(char *next_token)
{
    int n, rid;
    char *p[CHAT_CMD_FLDS];

    if (cmdline_proc(next_token, p, CHAT_CMD_FLDS, 0) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, CHAT_C_USAGE);
        return -1;
    }

    memset(myself.w_buf, 0, MAX_MSG_SIZE);
    rid = atoi(p[0]);
    if (!rid)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, CHAT_C_USAGE);
        return -1;
    }

    enc_chat_msg(myself.w_buf, &n, 0, rid, p[1]);
    client_send(myself.w_buf, n);
    myself.w_msgstat = MSG_RCVING;
    return 0;
}

int logout_cmd_proc(char *next_token)
{
    int n;
    char *p[LOGOUT_CMD_FLDS];
    if (cmdline_proc(next_token, p, LOGOUT_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s\n", INV_C_USAGE, EXIT_C_USAGE);
        return -1;
    }
    if (myself.w_cstat == LOGIN_CSTAT)
    {
        memset(myself.w_buf, 0, MAX_MSG_SIZE);
        enc_logout_msg(myself.w_buf, &n, 0);
        send(myself.w_socketfd, myself.w_buf, n, 0);
        myself.w_msgstat = MSG_RCVING;
    }
    return -2;
}

int help_cmd_proc(char *next_token)
{
    char *p[HELP_CMD_FLDS];

    if (cmdline_proc(next_token, p, HELP_CMD_FLDS, 1) == -1)
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, HELP_C_USAGE);
        return -1;
    }

    fprintf(stdout, "%s", CLIENT_USAGE);
    return 0;
}

/*
命令处理函数
@buf： 用户输入的命令字符串缓冲区
*/
int usr_cmd_process(char *buf)
{
    char *token = NULL;
    char *next_token = NULL;
    char spes[] = "\t ";
    token = __strtok_r(buf, spes, &next_token);

    if (!token)
        return 0;

    if (usr_stat_check(token) < 0)
        return -1;

    if (!strcmp(token, REG_CMD))
    {
        return reg_cmd_proc(next_token);
    }
    if (!strcmp(token, LIN_CMD))
    {
        return login_cmd_proc(next_token);
    }
    else if (!strcmp(token, FLST_CMD))
    {
        return flist_cmd_proc(next_token);
    }
    else if (!strcmp(token, FADD_CMD))
    {
        return add_cmd_proc(next_token);
    }
    else if (!strcmp(token, FDEL_CMD))
    {
        return del_cmd_proc(next_token);
    }
    else if (!strcmp(token, ALIST_CMD))
    {
        return alist_cmd_proc(next_token);
    }
    else if (!strcmp(token, FSTAT_CMD))
    {
        return fstat_cmd_proc(next_token);
    }
    else if (!strcmp(token, CHAT_CMD))
    {
        return chat_cmd_proc(next_token);
    }
    else if (!strcmp(token, EXIT_CMD))
    {
        return logout_cmd_proc(next_token);
    }
    else if (!strcmp(token, MESG_CMD))
    {
        return mesg_cmd_proc(next_token);
    }
    else if (!strcmp(token, DEBUG_CMD))
    {
        return debug_cmd_proc(next_token);
    }
    else if (!strcmp(token, HELP_CMD))
    {
        return help_cmd_proc(next_token);
    }
    else
    {
        fprintf(stderr, "%s%s", INV_C_USAGE, CLIENT_USAGE);
        return -1;
    }
}

int debug_cmd_proc(char *next_token)
{
    CLIENT_FRIEND *pos;
    printf("client: id=%d, name=%s, password=%s\n", myself.w_id, myself.w_name, myself.w_password);
    printf("client: sckfd=%d, cstat=%d, friend cnt=%d\n", myself.w_socketfd, myself.w_cstat, myself.w_fndcount);

    ufrnd_for_each_entry(pos, &myself.w_flisthd, c_node)
    {
        printf("client: friend id=%d, name=%s, stat=%d", pos->c_id, pos->c_name, pos->c_stat);
    }

    return 0;
}

int usr_stat_check(char *token)
{
    if (strcmp(token, REG_CMD) && strcmp(token, LIN_CMD) && strcmp(token, HELP_CMD) && strcmp(token, MESG_CMD) /*&& strcmp(token, EXIT_CMD)*/)
    {
        if (myself.w_cstat != LOGIN_CSTAT)
        {
            fprintf(stderr, "%s", INV_STAT_USAGE);
            return -1;
        }
    }
    return 0;
}

/*
命令处理函数
@buf： 用户输入的命令字符串缓冲区
*/
