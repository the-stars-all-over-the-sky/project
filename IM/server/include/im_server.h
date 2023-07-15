#ifndef IM_SERVER_H__
#define IM_SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


#include "list.h"
#include "msg_type.h"
#include "server_config.h"
#include "server_user.h"
#include "server_handle.h"
#include "server_dec.h"
#include "server_enc.h"


typedef struct thread_args
{
    pthread_t t_tid;
    int t_sockfd;
    // 每个用户的id关联一个子线程
    int t_uid;
} THREAD_ARGS;

#endif