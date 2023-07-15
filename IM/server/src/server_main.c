#include "im_server.h"

int server_socket_init()
{
    int sfd;
    struct sockaddr_in saddr;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)
    {
        perror("socket get error");
        return -1;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(SERVER_PORT);

    int ret = bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0)
    {
        perror("socker bind error!\n");
        return -1;
    }

    listen(sfd, 10);

    return sfd;
}

void server_socket_clean(int sfd)
{
    close(sfd);
}

void *client_thread(void *args)
{
    THREAD_ARGS *a = (THREAD_ARGS *)args;
    int fd = a->t_sockfd;
    int uid = a->t_uid;
    int n;
    char buf[MAX_MSG_SIZE];

    while (1)
    {
        n = recv(fd, buf, MAX_MSG_SIZE, 0);
        if (n <= 0) {
            printf("server: receive error, exit thread! n = %d, fd = %d\n", n, fd);
            break;
        }
        if (handle_client_msgs(&uid, fd, buf, n) == -2) {
            printf("serv: handle msg return -2, thread exiting...\n");
            break;
        }
    }
    user_logout_exit(uid);
    free(args);
    return NULL;
}

void server_main_loop(int fd)
{
    int cfd;
    socklen_t len;
    char line[80];
    struct sockaddr_in caddr;
    THREAD_ARGS *args;
    for (;;)
    {
        len = sizeof(caddr);
        printf("server: waiting for client connection......\n");
        cfd = accept(fd, (struct sockaddr *)&caddr, &len);
        printf("server: connection from %s, port %d\n", 
            inet_ntop(AF_INET, &caddr.sin_addr, line, sizeof(line)), ntohs(caddr.sin_port));

        args = (THREAD_ARGS *)malloc(sizeof(*args));
        if (args == NULL) {
            printf("server: memory malloc for thread args failed! \n");
            close(cfd);
            continue;
        }
        memset(args, 0, sizeof(*args));
        args->t_sockfd = cfd;
        pthread_create(&args->t_tid, NULL, &client_thread, (void *)args);
    }
}

int init_args(int argc, char **argv)
{
    if (argc > 3)
    {
        fprintf(stderr, "%s <user file> <friend file>\n", argv[0]);
        return -1;
    }
    init_all_usr_struct(argc, argv);
    return 0;
}

int server_init(int argc, char **argv)
{
    if (init_args(argc, argv) < 0)
    {
        return -1;
    }
    return server_socket_init();
}

int main(int argc, char **argv)
{
    int lfd; // listen fd
    lfd = server_init(argc, argv);
    if (lfd < 0)
    {
        fprintf(stderr, "server: socket init error!\n");
        return -1;
    }

    server_main_loop(lfd);
    server_socket_clean(lfd);
    return 0;
}