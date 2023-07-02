#include <stdlib.h>
#include <glob.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "meidalib.h"
#include "server_conf.h"
#include "proto.h"
#include "site_type.h"
#include "mytbf.h"

#define PATH_SIZE 1024
#define LINEBUFSIZE 1024
#define MP3_BITRATE 1024 * 256

struct channel_context_st
{
    chnid_t chnid;
    char *desc;
    glob_t mp3glob;
    int pos; // chnid对应的文件在mp3glob.pathv的索引
    int fd;
    off_t offset;
    mytbf_t *tbf;
};

struct channel_context_st channel[MAX_CHANNEL_ID + 1];

static struct channel_context_st *path2entry(const char *path)
{
    char pathstr[PATH_SIZE];
    char linebuf[LINEBUFSIZE];
    FILE *fp;
    struct channel_context_st *me;
    static chnid_t curr_id = MIN_CHANNEL_ID;
    strncpy(pathstr, path, strlen(path) + 1);
    strncat(pathstr, "/desc.text", strlen("/desc.text") + 1);

    fp = fopen(pathstr, "r");
    if (fp == NULL) {
        syslog(LOG_INFO, "[medialib][channel_context_st] %s is not a channel dir(Cannot find desc.text)", path);
        return NULL;
    }
    if (fgets(linebuf, LINEBUFSIZE, fp) == NULL) {
        syslog(LOG_INFO, "[medialib][channel_context_st] %s is not a channel dir(Cannot get the desc.text)", path);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    me = malloc(sizeof(*me));
    if (me == NULL) {
        syslog(LOG_ERR, "[medialib][channel_context_st] malloc fail :%s\n", strerror(errno));
        return NULL;
    }
    me->tbf = mytbf_init(MP3_BITRATE/8, MP3_BITRATE/8*10);
    if (me->tbf == NULL) {
        syslog(LOG_ERR, "[medialib][channel_context_st] mytbf_init fail :%s\n", strerror(errno));
        free(me);
        return NULL;
    }
    me->desc = strdup(linebuf);
    strncpy(pathstr, path, strlen(path) + 1);
    strncat(pathstr, "/*.mp3", strlen("/*.mp3") + 1);
    if (glob(pathstr, 0, NULL, &me->mp3glob) != 0) {
        // curr_id++;
        syslog(LOG_ERR, "[medialib][channel_context_st] %s is not a channel dir(cannot find mp3 files)", path);
        free(me);
        return NULL;
    }
    me->pos = 0;
    me->offset = 0;
    me->fd = open(me->mp3glob.gl_pathv[me->pos], O_RDONLY);
    if (me->fd < 0) {
        syslog(LOG_WARNING, "[medialib][channel_context_st] %s open failed", me->mp3glob.gl_pathv[me->pos]);
        free(me);
        return NULL;
    }
    me->chnid = curr_id;
    curr_id++;
    return me;
}

int mlib_getchnlist(struct mlib_listentry_st **result, int *resnum)
{
    char path[PATH_SIZE];
    glob_t globres;
    int num = 0;
    struct mlib_listentry_st *ptr;
    struct channel_context_st *res;
    for (int i = 0; i < MAX_CHANNEL_ID + 1; i++)
    {
        channel[i].chnid = -1;
    }
    snprintf(path, PATH_SIZE, "%s*", server_conf.media_dir);
    syslog(LOG_INFO, "[medialib][mlib_getchnlist] path :%s", path);
    if (glob(path, 0, NULL, &globres))
    {
        syslog(LOG_ERR, "[medialib][mlib_getchnlist] fail to glob path :%s", strerror(errno));
        return -1;
    }

    ptr = malloc(sizeof(struct mlib_listentry_st) * globres.gl_pathc);
    if (ptr == NULL)
    {
        syslog(LOG_ERR, "[medialib][mlib_getchnlist] malloc error.");
        exit(1);
    }
    for (int i = 0; i < globres.gl_pathc; i++)
    {
        // globres.gl_pathv[i] -> "../../media/channel01"
        res = path2entry(globres.gl_pathv[i]);
        if (res != NULL) {
            syslog(LOG_DEBUG, "[medialib][mlib_getchnlist] path2entry success [chnid: %d desc: %s]", res->chnid, res->desc);
            memcpy(channel + res->chnid, res, sizeof(*res));
            ptr[num].chnid = res->chnid;
            ptr[num].desc = strdup(res->desc);
            num++;
        }
    }
    *result = realloc(ptr, sizeof(struct mlib_listentry_st) * num);
    if (*result == NULL) {
        syslog(LOG_ERR, "[medialib][mlib_getchnlist]realloc failed.");
    }
    *resnum = num;
    return 0;
}

int mlib_freechnlist(struct mlib_listentry_st *)
{
    return 0;
}

static int open_next(chnid_t chnid)
{
    for (int i = 0; i < channel[chnid].mp3glob.gl_pathc; i++)
    {
        channel[chnid].pos++;
        if (channel[chnid].pos == channel[chnid].mp3glob.gl_pathc)
        {
            channel[chnid].pos = 0;
            break;
        }
        close(channel[chnid].fd);
        // syslog(LOG_INFO, "channelid.fd before open: %d", channel[chnid].fd);
        // syslog(LOG_INFO, "channelid.fd open: %s", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos]);
        channel[chnid].fd = open(channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], O_RDONLY);
        // syslog(LOG_INFO, "channelid.fd: %d", channel[chnid].fd);

        if (channel[chnid].fd < 0)
        {
            syslog(LOG_WARNING, "[medialib][open_next] open(%s) fail:%s", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], strerror(errno));
        }
        else
        {
            channel[chnid].offset = 0;
            return 0;
        }
    }
    syslog(LOG_ERR, "[medialib][open_next] None of mp3s in channel %d id available.", chnid);
    return -1;
}

ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size)
{
    int tbfsize;
    tbfsize = mytbf_fetchtoken(channel[chnid].tbf, size);

    int len;
    while (1)
    {
        len = pread(channel[chnid].fd, buf, tbfsize, channel[chnid].offset);
        if (len < 0)
        {
            syslog(LOG_WARNING, "[medialib][mlib_readchn] media file %s pread() fail:%s", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos], strerror(errno));
            // if (open_next(chnid) < 0)
            // {
            //     syslog(LOG_ERR, "channel %d: There is no successed open", chnid);
            // }
            open_next(chnid);
        }
        else if (len == 0)
        {
            syslog(LOG_DEBUG, "[medialib][mlib_readchn] media file %s is over.", channel[chnid].mp3glob.gl_pathv[channel[chnid].pos]);
            // if (open_next(chnid) < 0)
            // {
            //     syslog(LOG_ERR, "channel %d: There is no successed open", chnid);
            // }
            channel[chnid].offset = 0;
            open_next(chnid);
        }
        else
        {
            channel[chnid].offset += len;
            break;
        }
    }

    if (tbfsize - len > 0)
    {
        mytbf_returntoken(channel[chnid].tbf, tbfsize - len);
    }
    return len;
}