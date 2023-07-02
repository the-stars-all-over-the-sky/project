#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>

#include "thr_channel.h"
#include "proto.h"
#include "server_conf.h"

struct thr_channel_ent_st
{
    chnid_t chnid;
    pthread_t tid;
};

struct thr_channel_ent_st thr_channel[CHANNEL_NUM];
static int tid_nextpos = 0;

static void *thr_channel_snder(void *ptr)
{
    struct msg_channel_st *sbufp;
    struct mlib_listentry_st *ent = ptr;
    sbufp = malloc(MSG_CHANNEL_MAX);
    if (sbufp == NULL) {
        syslog(LOG_ERR, "[thr_channel][thr_channel_snder] malloc fail:%s\n", strerror(errno));
        exit(1);
    }
    sbufp->chnid = ent->chnid;
    int len;
    while (1) {
        len = mlib_readchn(ent->chnid, sbufp->data, MAX_DATA);

        int err = sendto(serversd, sbufp, len + sizeof(chnid_t), 0, (void *)&sndaddr, sizeof(sndaddr));
        if (err < 0) {
            syslog(LOG_ERR, "[thr_channel][thr_channel_snder] send data of channel %d fail:%s", ent->chnid, strerror(errno));
            exit(1);
        }
        syslog(LOG_INFO, "[thr_channel][thr_channel_snder] send data of channel %d success\n", ent->chnid);
        sched_yield(); // 主动出让调度器
    }
    pthread_exit(NULL);
}

int thr_channel_create(struct mlib_listentry_st *ptr)
{
    int err = pthread_create(&thr_channel[tid_nextpos].tid, NULL, thr_channel_snder, ptr);
    if (err) {
        syslog(LOG_WARNING, "[thr_channel][thr_channel_create] pthread_create fail: %s, ", strerror(err));
        return -err;
    }
    thr_channel[tid_nextpos].chnid = ptr->chnid;
    tid_nextpos++;
    return 0;
}
int thr_channel_destory(struct mlib_listentry_st *ptr)
{
    for (int i = 0; i < CHANNEL_NUM; i++) {
        if (thr_channel[i].chnid == ptr->chnid)
        {
            if (pthread_cancel(thr_channel[i].tid) < 0) {
                syslog(LOG_ERR, "[thr_channel][thr_channel_destory] pthread_cancel: the thread of channel %d", ptr->chnid);
                return -ESRCH;
            }
        }
        pthread_join(thr_channel[i].tid, NULL);
        thr_channel[i].chnid = -1;
        return 0;
    }
}
int thr_channel_destroyall(void)
{
    for (int i = 0; i < CHANNEL_NUM; i++) {
        if (thr_channel[i].chnid > 0)
        {
            if (pthread_cancel(thr_channel[i].tid) < 0) {
                syslog(LOG_ERR, "[thr_channel][thr_channel_destroyall] pthread cancel fail, the thread of channel id is %d", thr_channel[i].chnid);
                return -ESRCH;
            }
            pthread_join(thr_channel[i].tid, NULL);
            thr_channel[i].chnid = -1;
        }
    }
    return 0;
}