#ifndef MYTBF_H__
#define MYTBF_H__

#define MYTBF_MAX 1024
typedef void mytbf_t;

mytbf_t *mytbf_init(int cps, int burst); // 初始化时需要指定速率和上限
int mytbf_fetchtoken(mytbf_t *, int );
int mytbf_returntoken(mytbf_t *, int );
int mytbf_destroy(mytbf_t *);

#endif