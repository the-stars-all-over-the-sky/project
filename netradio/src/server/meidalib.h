#ifndef MEDIALIB_H__
#define MEDIALIB_H__
#include <stdio.h>
#include "proto.h"

struct mlib_listentry_st
{
    chnid_t chnid;
    char *desc;
};

extern int mlib_getchnlist(struct mlib_listentry_st **, int *);

extern int mlib_freechnlist(struct mlib_listentry_st *);

extern ssize_t mlib_readchn(chnid_t chnid, void *buf, size_t size);

#endif