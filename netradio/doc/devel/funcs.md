# socket  
```
#include <sys/socket.h>  
int setsockopt(int socket, int level, int option_name,  
               const void *option_value, socklen_t option_len);  
功能：设置一个套接字的选项（属性）  
参数：  
    socket：文件描述符  
    level：协议层次  
        SOL_SOCKET 套接字层次  
        IPPROTO_TCP tcp层次  
        IPPROTO_IP IP层次  
    option_name：选项的名称  
        SO_BROADCAST 允许发送广播数据（SOL_SOCKET层次的）  
    option_value：设置的选项的值  
        int类型的值，存储的是bool的数据（1和0）  
            0 不允许  
            1 允许  
    option_len：option_value的长度  
返回值：  
    成功：0  
    失败：-1  
```
# inet_pton && inet_ntop  
```
字符串ip地址转整型数据  
    #include <arpa/inet.h>  
    int inet_pton(int family,const char *strptr, void *addrptr);  
    功能：  
        将点分十进制数串转换成32位无符号整数  
    参数：  
        family 协议族  
            AF_INET IPV4网络协议  
            AF_INET6 IPV6网络协议  
        strptr 点分十进制数串  
        addrptr 32位无符号整数的地址  
    返回值：  
        成功返回1  
        失败返回其它  
整型数据转字符串格式ip地址  
    #include <arpa/inet.h>  
    const char *inet_ntop(int family, const void *addrptr,char *strptr, size_t len);  
    功能：  
        将32位无符号整数转换成点分十进制数串  
    参数：  
        family 协议族  
        addrptr 32位无符号整数  
        strptr 点分十进制数串  
        len strptr缓存区长度  
        len的宏定义  
            #define INET_ADDRSTRLEN 16 //for ipv4  
            #define INET6_ADDRSTRLEN 46 //for ipv6  
    返回值：  
        成功:则返回字符串的首地址  
        失败:返回NULL  
```
# setsockopt  
```
#include <sys/socket.h>  
int setsockopt(int socket, int level, int option_name,  
               const void *option_value, socklen_t option_len);  
功能：设置一个套接字的选项（属性）  
参数：  
    socket：文件描述符  
    level：协议层次  
        SOL_SOCKET 套接字层次  
        IPPROTO_TCP tcp层次  
        IPPROTO_IP IP层次  
    option_name：选项的名称  
        SO_BROADCAST 允许发送广播数据（SOL_SOCKET层次的）  
        IP_MULTICAST_TTL 设置多播组数据允许超时的TTL值  
        IP_ADD_MEMBERSHIP 在指定接口上加入组播组  
        IP_DROP_MEMBERSHIP 退出组播组  
        IP_MULTICAST_IF 获取/设置默认接口或设置接口  
        IP_MULTICAST_LOOP 禁止/允许组播数据回送  
    option_value：设置的选项的值  
        int类型的值，存储的是bool的数据（1和0）  
            0 不允许  
            1 允许  
    option_len：option_value的长度  
返回值：  
    成功：0  
    失败：-1  
```
# htonl && htons && ntohl && ntohs  
```
1、网络协议指定了通讯字节序—大端  
2、只有在多字节数据处理时才需要考虑字节序  
3、运行在同一台计算机上的进程相互通信时,一般不用考虑字节序  
4、异构计算机之间通讯，需要转换自己的字节序为网络字节序  
在需要字节序转换的时候一般调用特定字节序转换函数  
  
host --> network  
    1 -- htonl  
        #include <arpa/inet.h>  
        uint32_t htonl(uint32_t hostint32);  
        功能:  
            将32位主机字节序数据转换成网络字节序数据  
        参数：  
            hostint32：待转换的32位主机字节序数据  
        返回值：  
            成功：返回网络字节序的值  
  
    2 -- htons  
        #include <arpa/inet.h>  
        uint16_t htons(uint16_t hostint16);  
        功能：  
            将16位主机字节序数据转换成网络字节序数据  
        参数：  
            uint16_t：unsigned short int  
            hostint16：待转换的16位主机字节序数据  
        返回值：  
            成功：返回网络字节序的值  
  
network --> host  
    3 -- ntohl  
        #include <arpa/inet.h>  
        uint32_t ntohl(uint32_t netint32);  
        功能：  
            将32位网络字节序数据转换成主机字节序数据  
        参数：  
            uint32_t： unsigned int  
            netint32：待转换的32位网络字节序数据  
        返回值：  
            成功：返回主机字节序的值  
  
    4 -- ntohs  
        #include <arpa/inet.h>  
        uint16_t ntohs(uint16_t netint16);  
        功能：  
            将16位网络字节序数据转换成主机字节序数据  
        参数：  
            uint16_t： unsigned short int  
            netint16：待转换的16位网络字节序数据  
        返回值：  
            成功：返回主机字节序的值  
```
# bind  
```
#include <sys/types.h>  
#include <sys/socket.h>  
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);  
功能：将套接字与网络信息结构体绑定  
参数：  
    sockfd：文件描述符，socket的返回值  
    addr：网络信息结构体  
        通用结构体（一般不用）  
            struct sockaddr  
        网络信息结构体 sockaddr_in  
            #include <netinet/in.h>  
            struct sockaddr_in  
    addrlen：addr的长度  
返回值：  
    成功：0  
    失败：-1  
```
# dup2  
```
#include <unistd.h>  
int dup2(int oldfd, int newfd)  
功能：复制一份打开的文件描述符oldfd，并分配新的文件描述符newfd，newfd也标识oldfd所标识的文件。  
注意：  
    newfd是小于文件描述符最大允许值的非负整数，如果newfd是一个已经打开的文件描述符，则首先关闭该文件，然后再复制。  
参数：  
    oldfd：要复制的文件描述符  
    newfd：分配的新的文件描述符  
返回值：  
    成功：返回newfd  
    失败：返回‐1，错误代码存于errno中  
```
# exec函数族  
```
exec函数族，是由六个exec函数组成的。  
    1、exec函数族提供了六种在进程中启动另一个程序的方法。  
    2、exec函数族可以根据指定的文件名或目录名找到可执行文件。  
    3、调用exec函数的进程并不创建新的进程，故调用exec前后，进程的进程号并不会改变，其执行的程序完全由新的程序替换，而新程序则从其main函数开始执行。  
exec函数族取代调用进程的数据段、代码段和堆栈段。  
一个进程调用exec后，除了进程ID，进程还保留了下列特征不变：  
    父进程号  
    进程组号  
    控制终端  
    根目录  
    当前工作目录  
    进程信号屏蔽集  
    未处理信号  
    ...  
  
  
#include <unistd.h>  
int execl(const char *path, const char *arg, .../* (char *) NULL */);  
int execlp(const char *file, const char *arg, .../* (char *) NULL */);  
int execv(const char *path, char *const argv[]);  
int execvp(const char *file, char *const argv[]);  
int execle(const char *path, const char *arg, .../*, (char *) NULL*/, char * const envp[] );  
int execvpe(const char *file, char *const argv[], char *const envp[]);  
功能：在一个进程里面执行另一个程序，主要用于执行命令  
参数：  
    path：命令或者程序的路径  
    l：如果是带l的函数，对应的命令或者程序是通过每一个参数进行传递的，最后一个为NULL表示结束  
        例如："ls", "-l", NULL  
    v:如果是带v的函数，对应的命令或者程序是通过一个指针数组来传递的，指针数组的最后一个元素为NULL标识结束  
        char *str[] = {"ls", "-l", NULL};  
    p：如果是不带p的函数，第一个参数必须传当前命令或者程序的绝对路径，如果是带p的函数，第一个参数既可以是绝对路径，也可以是相对路径  
返回值：  
    失败：-1  
```
# recvfrom  
```
#include <sys/types.h>  
#include <sys/socket.h>  
  
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,  
                 struct sockaddr *src_addr, socklen_t *addrlen);  
功能：接收数据  
参数：  
    sockfd：文件描述符，socket的返回值  
    buf：保存接收的数据  
    len：buf的长度  
    flags：标志位  
        0 阻塞  
        MSG_DONTWAIT 非阻塞  
    src_addr：源的网络信息结构体（自动填充，定义变量传参即可）  
    addrlen：src_addr的长度  
返回值：  
    成功：接收的字节数  
    失败：-1  
```
# pause && alarm  
```
pause:让进程暂停直到信号出现  
    定义函数：int pause(void);  
    函数说明：pause()会令目前的进程暂停(进入睡眠状态), 直到被信号(signal)所中断  
    返回值：只返回-1.  
alarm:  
    定义函数：unsigned int alarm(unsigned int seconds);  
    函数说明：alarm()用来设置信号SIGALRM 在经过参数seconds 指定的秒数后传送给目前的进程. 如果参数seconds为0, 则之前设置的闹钟会被取消, 并将剩下的时间返回.  
    返回值：返回之前闹钟的剩余秒数, 如果之前未设闹钟则返回0  
```
# abort  
```
定义函数：void abort(void)  
函数说明：abort()会中止程序执行，直接从调用的地方跳出  
返回值：无  
```
# setsid  
```
定义函数：pid_t setsid (void)  
函数说明：  
    当进程是会话的领头进程时setsid()调用失败并返回（-1）。  
    setsid()调用成功后，返回新的会话的ID，调用setsid函数的进程成为新的会话的领头进程，并与其父进程的会话组和进程组脱离。  
    由于会话对控制终端的独占性，进程同时与控制终端脱离。  
返回值：  
    成功：返回新的会话的ID  
    失败：-1  
```
# umask  
```
#include <sys/types.h>  
#include <sys/stat.h>  
mode_t umask(mode_t mask);  
功能：  
    umask()会将系统umask值设成参数mask & 0777后的值, 然后将先前的umask值返回。  
    在使用open()建立新文件时, 该参数mode并非真正建立文件的权限, 而是(mode&~umask)的权限值。  
返回值：  
    成功：先前的umask值  
    失败：不会失败  
```
# openlog && syslog && closelog  
```
#include <syslog.h>  
void openlog(const char *ident, int option, int facility);  
void syslog(int priority, const char *format, ...);  
void closelog(void);  
功能：  
    syslogd守护进程用于解决守护进程的日志记录问题，而日志信息保存的位置和记录的信息级别是在syslogd守护进程的配置文件中设定的。  
    守护进程日志主要涉及3个函数，分别是openlog、syslog和closelog函数。  
    调用openlog是可选择的。如果不调用openlog，则在第一次调用syslog时，自动调用openlog。  
    调用closelog也是可选择的，它只是关闭被用于与syslog守护进程通信的描述符。  
    调用openlog使我们可以指定一个ident，以后，此ident将被加至每则记录消息中。ident一般是程序的名称（例如 ，cron ，ine 等）  
参数：  
    openlog：  
        ident：该参数常用来表示信息的来源。ident指向的字符信息会被固定地添加在每行日志的前面  
        option：用于指定openlog函数和接下来调用的syslog函数的控制标志  
            LOG_CONS 如果将信息发送给syslogd守护进程时发生错误，直接将相关信息输出到终端  
            LOG_NDELAY 立即打开与系统日志的连接（通常情况下，只有在产生第一条日志信息的情况下才会打开与日志系统的连接）  
            LOG_NOWAIT 在记录日志信息时，不等待可能的子进程的创建  
            LOG_ODELAY 类似于LOG_NDELAY参数，与系统日志的连接只有在syslog函数调用时才会创建  
            LOG_PERROR 在将信息写入日志的同时，将信息发送到标准错误输出（POSIX.1-2001不支持该参数）  
            LOG_PID 每条日志信息中都包括进程号  
        facility：要与syslogd守护进程的配置文件对应，日志信息会写入syslog.conf文件指定的位置  
            LOG_KERN kern  
            LOG_USER user  
            LOG_MAIL mail  
            LOG_DAEMON daemon  
            LOG_AUTH auth  
            LOG_SYSLOG syslog  
            LOG_LPR lpr  
            LOG_NEWS news  
            LOG_UUCP uucp  
            LOG_CRON cron  
            LOG_AUTHPRIV authpriv  
            LOG_FTP ftp  
            LOG_LOCAL0～LOG_LOCAL7 local0～local7  
    syslog：  
        priority：表示消息的级别。与openlog函数中的facility参数类似，priority参数与level也存在对应的关系。  
            LOG_EMERG emerg  
            LOG_ALERT alert  
            LOG_CRIT crit  
            LOG_ERR err  
            LOG_WARNING warning  
            LOG_NOTICE notice  
            LOG_INFO info  
            LOG_DEBUG debug  
        format：略（类比printf）  
```
# pthread_create  
```
头文件：#include <pthread.h>  
原型：```int pthread_create(pthread_t *tidp,const pthread_attr_t *attr,(void*)(*start_rtn)(void*),void *arg);```  
作用：创建一个线程  
编译链接参数：-lpthread  
返回值：线程创建成功返回0，创建失败返回错误编号。  
参数：参数一为线程标识符的指针  
     参数二表示线程熟悉，一般为NULL  
     参数三为线程运行函数的起始地址  
     参数四为运行函数的参数  
```
# pthread_join  
```
头文件：#include <pthread.h>  
原型：int pthread_join(pthread_t thread, void **retval);  
作用：阻塞当前线程等待指定的线程执行结束  
返回值：成功返回0，失败返回错误号  
参数：参数一表示线程标识符  
     参数二表示线程thread所执行的函数返回值（返回值地址需要保证有效），其中status可以为NULL。  
```
# pthread_mutex_init  
```
头文件：#include <pthread.h>  
原型：int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);  
     pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  
作用：用于多线程编程时，互斥锁的初始化  
返回值：成功返回0，失败返回其他值  
参数：参数一为锁变量  
     参数二为锁属性，NULL值表示默认属性  
```
# pthread_mutex_lock  
```
头文件：#include <pthread.h>  
原型：int  pthread_mutex_lock(pthread_mutex_t *mutex)  
作用：加锁  
```
# pthread_mutex_unlock  
```
头文件：#include <pthread.h>  
原型：int pthread_mutex_unlock(pthread_mutex_t *mutex)  
作用：解锁  
```
# pthread_mutex_destroy  
```
原型：int  pthread_mutex_destroy(pthread_mutex_t *mutex);  
作用：使用完后释放  
```
# pthread_once  
```
函数声明：int pthread_once(pthread_once_t *once_control, void (*init_routine) (void))；  
功能：本函数使用初值为 PTHREAD_ONCE_INIT 的 once_control 变量保证 init_routine() 函数在本进程执行序列中仅执行一次。  
参数：  
    once_control   控制变量（为 pthread_once_t * 类型，必须使用 PTHREAD_ONCE_INIT 宏静态地初始化。）  
    init_routine   初始化函数  
返回值：若成功返回0，若失败返回错误编号。  
```
# pthread_cond_init  
```
#include <pthread.h>  
int pthread_cond_init(pthread_cond_t *cv, const pthread_condattr_t *cattr);  
功能：  
    初始化一个条件变量。  
    cattr为空指针时，函数创建的是一个缺省的条件变量。否则条件变量的属性将由cattr中的属性值来决定。  
    调用 pthread_cond_init函数时，参数cattr为空指针等价于cattr中的属性为缺省属性，只是前者不需要cattr所占用的内存开销。  
    这个函数返回时，条件变量被存放在参数cv指向的内存中。  
参数：  
    cv：条件变量存放地址  
    cattr：cattr为空指针时，函数创建的是一个缺省的条件变量。否则条件变量的属性将由cattr中的属性值来决定。  
返回值：函数成功返回0；任何其他返回值都表示错误  
```
# glob  
```
#include <glob.h>  
int glob(const char *pattern, int flags,  
                int (*errfunc) (const char *epath, int eerrno),  
                glob_t *pglob);  
void globfree(glob_t *pglob);  
功能：Linux文件系统中路径名称的模式匹配，即查找文件系统中指定模式的路径。  
参数：  
    pattern： 匹配模型。如/*是匹配根文件下的所有文件（不包括隐藏文件，要找的隐藏文件需要从新匹配）  
    errfunc： 选择匹配模式。  
    errfunc:：查看错误信息用，一般置为NULL  
    pglob：存放匹配出的结果   
返回值：  
    成功 0  
    失败 非0  
```