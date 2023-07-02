#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "thr_list.h"
#include "proto.h"
#include "server_conf.h"

static pthread_t tid_list;

static int nr_list_ent;
static struct mlib_listentry_st *list_ent;

static void *thr_list(void *p)
{
    int totalsize;
    struct msg_list_st *entlistp;
    struct msg_list_entry_st *entryp;

    totalsize = sizeof(chnid_t);
    for (int i = 0; i < nr_list_ent; i++) {
        syslog(LOG_INFO, "[thr_list] chnid = %d, desc = %s", list_ent[i].chnid, list_ent[i].desc);
        totalsize += sizeof(struct msg_list_entry_st) + strlen(list_ent[i].desc);
    }

    entlistp = malloc(totalsize);
    if (entlistp == NULL) {
        syslog(LOG_ERR, "[thr_list] malloc error:%s", strerror(errno));
        exit(1);
    }
    entlistp->chnid = LIST_CHANNEL_ID;
    entryp = entlistp->entry;
    int size;
    for (int i = 0; i < nr_list_ent; i++) {
        size = sizeof(struct msg_list_entry_st) + strlen(list_ent[i].desc);
        entryp->chnid = list_ent[i].chnid;
        entryp->len = htons(size);
        strcpy(entryp->desc, list_ent[i].desc);
        entryp = (void *)(((char *)entryp) + size);
    }
    int ret;
    while (1)
    {
        ret = sendto(serversd, entlistp, totalsize, 0, (void *)&sndaddr, sizeof(sndaddr));
        if (ret < 0) {
            syslog(LOG_WARNING, "[thr_list] send data fail:%s", strerror(errno));
        } else {
            syslog(LOG_DEBUG, "[thr_list] send data success!");
        }
        sleep(1);
    }
}

int thr_list_create(struct mlib_listentry_st *listp, int nr_ent)
{
    int err;
    list_ent = listp;
    nr_list_ent = nr_ent;
    err = pthread_create(&tid_list, NULL, thr_list, NULL);
    if (err) {
        syslog(LOG_ERR, "[thr_list] pthread_create fail, the reason is :%s.", strerror(err));
        return -1;
    }
    return 0;
}
int thr_list_destroy(void)
{
    pthread_cancel(tid_list);
    pthread_join(tid_list, NULL);
    return 0;
}