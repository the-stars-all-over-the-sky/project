#ifndef MSG_TYPE_H__
#define MSG_TYPE_H__
/* 实现通用消息结构 */

#define SERVER_FORWARD_FLAG (0x1 << 15)

typedef struct msg_header
{
    /*消息类型*/
    unsigned short msg_type;
    /*消息长度*/
    unsigned short msg_len;
    /*可变长度的消息内容， 虚拟的消息内容指针；不占空间，仅仅是一个占位符*/
    char msg_data[1];
} __attribute__((packed)) MSG_HDR;

/*所有的消息类型*/
#define MSG_REG 0x01      // 用户注册消息
#define MSG_LOGIN 0x02    // 用户登陆消息
#define MSG_FRNDMNG 0x03 // 好友管理消息
#define MSG_CHAT 0x04     // 用户聊天消息
#define MSG_LOGOUT 0x05   // 退出登陆消息

/*好友管理消息的子消息类型*/
#define F_STAT 0x01       // 好友状态查询
#define F_ALIST 0x02      // 好友列表查询
#define F_ADD 0x03        // 增加好友
#define F_DEL 0x04        // 删除好友
#define F_LISTA_USER 0x05 // 列出所有用户

/*最大用户名长度*/
#define MAX_NAME_LEN 16

/*最大用户密码长度*/
#define MAX_PASSWD_LEN 6

/*注册消息与注册回复消息*/
typedef struct reg_msgdata
{
    char r_name[MAX_NAME_LEN];
    char r_password[MAX_PASSWD_LEN];
} __attribute__((packed)) REG_MSG;

#define MAX_ERR_LEN 80
typedef struct reg_msg_response
{
    // 将uid返回给客户端，-1表示出错
    int reponse_id;
    // 出错的原因
    char err_reason[1];
} __attribute__((packed)) REG_RESPONSE_MSG;

/*登陆消息与登陆回复消息*/
typedef struct login_msg
{
    int lg_id;
    char lg_pwd[MAX_PASSWD_LEN];
} __attribute__((packed)) LOGIN_MSG;

#define LOGIN_OK 1
#define LOGIN_ERR 2

typedef struct login_response_msg
{
    /*登录状态 1表示ok  2表示出错*/
    int lg_stat;
    char lg_name[MAX_NAME_LEN];
    char lg_err_reason[1];
} __attribute__((packed)) LOGIN_RESPONSE_MSG;

/*好友操作消息与回复消息*/
typedef struct frnd_op
{
    /*用户本身*/
    int f_id;
    /*表示需要操作的用户id数组*/
    int f_fids[1];
} FRND_OP;

// 好友在线状态
#define USR_ONLINE 1
#define USR_OFFLINE 2
typedef struct frnd_stat
{
    char f_name[MAX_NAME_LEN];
    int f_id;
    int f_stat;
} FRND_STAT;
#define OP_ALL_OK 1
#define OP_PART_OK 2
#define OP_ALL_FAIL 3

typedef struct frnd_op_responce
{
    // 好友的状态
    short f_stat;
    // 成功数目
    short f_num;
    // 好友的在线状态
    FRND_STAT f_ok_frnd[1];
} __attribute__((packed)) FRND_OP_RESPONSE;

/*聊天信息与回复消息*/
typedef struct chat_msg
{
    // 消息发送者id
    int ch_sid;
    // 接收者id
    int ch_rid;
    // 需要发送的消息
    char ch_msg[1];
} __attribute__((packed)) CHAT_MSG;

#define SND_OK 1
#define SND_ERR 2
typedef struct chat_responce
{
    int c_stat;
    char c_msg[1]; // 若有错误，此为错误信息；若一切正常，则此为聊天信息
} __attribute__((packed)) CHAT_RESPONCE;

/*退出消息*/
typedef struct logout_msg
{
    int lg_id;
    char lg_pass[MAX_PASSWD_LEN];
} __attribute__((packed)) LOGOUT_MSG;


#endif