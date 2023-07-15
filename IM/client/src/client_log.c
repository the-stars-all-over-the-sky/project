#include "im_client.h"

void client_log_save(char *msg, int from, int to)
{
    FILE *fp;
    char buf[MAX_LINE_LEN];
    time_t ticks;
    ticks = time(NULL);
    int n = 0;

    fp = fopen(CLIENT_LOG_FILE, "a+");
    if (fp == NULL)
    {
        perror("client-log: open for append error!!!\n");
        return;
    }

    n += snprintf(buf + n, sizeof(buf) - n, "%d%s", from, FILED_SPES);
    n += snprintf(buf + n, sizeof(buf) - n, "%d%s", to, FILED_SPES);
    n += snprintf(buf + n, sizeof(buf) - n, "%.24s%s", ctime(&ticks), FILED_SPES);
    n += snprintf(buf + n, sizeof(buf) - n, "%s%s", msg, FILED_SPES);

    buf[n++] = '\n';
    buf[n] = '\0';
    fputs(buf, fp);
    fclose(fp);
}

void client_log_export(FILE *sfp, FILE *dfp, int from, int to)
{
    char buf[MAX_LINE_LEN];
    char *token = NULL;
    char *next_token = NULL;
    char *p[MSG_FILEDS] = {NULL};

    int msg_src, msg_dst;
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, MAX_LINE_LEN, sfp) != NULL)
    {
        token = __strtok_r(buf, FILED_SPES, &next_token);
        for (int i = 0; i < MSG_FILEDS; i++)
        {
            p[i] = token;
            token = __strtok_r(NULL, FILED_SPES, &next_token);
        }

        if (from)
        {
            msg_src = atoi(p[0]);
        }
        if (to)
        {
            msg_dst = atoi(p[1]);
        }
        /*
            from 和 to均为0，要导出所有消息
            from不为0， to为0， 并且msg_src==from，过滤来自某个用户发送的消息
            from为0， to不为0， msg_dst==to。 过滤发送至某个用户的的消息
            from与to都不为0， 并且 msg_src==from && msg_dst==to，过滤发送者是from并且接收者是to的消息
        */

        if ((!from && !to) || (from && !to && msg_src == from) || (!from && !to && msg_dst == to) || (from && to && msg_src == from && msg_dst == to))
        {
            fprintf(dfp, "client-log : %s->%s, Time:%s, msg: %s\r\n\r\n", p[0], p[1], p[2], p[3]);
        }
        memset(buf, 0, sizeof(buf));
    }
}