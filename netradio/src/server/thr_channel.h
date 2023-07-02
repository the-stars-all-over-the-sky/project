#ifndef THR_CHANNEL_H__
#define THR_CHANNEL_H__
#include "meidalib.h"

int thr_channel_create(struct mlib_listentry_st *ptr);
int thr_channel_destory(struct mlib_listentry_st *ptr);
int thr_channel_destroyall(void);

#endif