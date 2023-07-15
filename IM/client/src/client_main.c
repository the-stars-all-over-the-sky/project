#include "im_client.h"

void sock_cleanup(int sockfd)
{
    close(sockfd);
}

void socklib_init()
{
}

int client_sock_init(char *addr)
{
    int sfd;
    struct sockaddr_in serv;
    // socklib_init();
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        printf("client : socket error\n");
        return -1;
    }
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, addr, &serv.sin_addr) <= 0)
    {
        printf("client : inet_pton error\n");
        close(sfd);
        return -2;
    }

    /* 链接服务器 */
    if (connect(sfd, (struct sockaddr *)&serv, sizeof(serv)) < 0)
    {
        printf("client : connect error\n");
        return -3;
    }

    return sfd;
}

int init_args(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: %s <IPv4 server ip>\n ", argv[0]);
        return -1;
    }
    /* init client socket fd */
    myself.w_socketfd = client_sock_init(argv[1]);
    if (myself.w_socketfd < 0)
    {
        return -1;
    }
    return 0;
}

void *client_cli_thread(void *arg)
{
    char line[MAX_LINE_LEN];
    char user_prompt[MAX_PROMPT_LEN];

    while (1) {
        if (myself.w_id != -1 && myself.w_cstat == LOGIN_CSTAT) {
            /*
                im_client(lisi)#
            */
            snprintf(user_prompt, MAX_PROMPT_LEN, "%s(%s)#", USER_PROMPT, myself.w_name);
        } else {
            /*
                im_client(unknown)#
            */
            snprintf(user_prompt, MAX_PROMPT_LEN, "%s(unknown)#", USER_PROMPT);
        }
        printf("%s", user_prompt);
        memset(line, 0, sizeof(line));
        if (fgets(line, MAX_LINE_LEN, stdin) == NULL) {
            break;
        }
        if (line[strlen(line) - 1] == '\n') {
            // printf(" %ld %s", strlen(line), line);
            line[strlen(line) - 1] = '\0';
        }
        /* 执行并判读是否退出指令*/
        if (usr_cmd_process(line) == -2) {
            printf("client:user logout...\n");
            break;
        }
    }
    close(myself.w_socketfd);
    pthread_exit(NULL);
}

void client_main_loop()
{
    int n = 0;
    pthread_t client_stdin_pid;
    pthread_create(&client_stdin_pid, NULL, client_cli_thread, NULL);

    while ((n = recv(myself.w_socketfd, myself.w_buf, MAX_MSG_SIZE, 0)) > 0)
    {
        printf("client: recv msg len = %d\n", n);
        dec_server_resp(myself.w_buf, n);
    }
    printf("client: recv len < 0, exiting......\n");
}
/*
要求只有一个参数：IPv4
*/
int main(int argc, char **argv)
{
    /* 用户数据结构初始化 */
    init_user_struct(&myself);
    /* 参数初始化 */
    if (init_args(argc, argv) < 0)
    {
        return -1;
    }
    /* 客户端主循环处理函数 */
    /* 多线程 */
    client_main_loop();
    /* 清理函数*/
    sock_cleanup(myself.w_socketfd);
    return 0;
}