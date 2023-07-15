#ifndef CLIENT_LOG_H__
#define CLIENT_LOG_H__

/*
聊天信息的结构

发送者        ID        10001
接收者        ID        10002
发送时间        时间字符串
消息的内容    字符串

文本文件

发送者 / 接收者 / 发送时间 / 具体消息


功能

存储   导入   导出

*/
/* 消息分割符 */
#define FILED_SPES "/"
/* 每一行的字段个数 */
#define MSG_FILEDS 4

#define CLIENT_LOG_FILE "msg_log.log"

void client_log_save(char *msg, int from, int to);
void client_log_export(FILE *sfp, FILE *dfp, int from, int to);


#endif // CLIENT_LOG_H__