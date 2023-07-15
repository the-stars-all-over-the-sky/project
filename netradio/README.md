# 基于IPv4的流媒体广播项目  
  
#### 介绍  
```
基于IPv4的流媒体广播项目(UDP)：  
- 具备流媒体的特点，client不需要关心server发送的数据处于文件的什么位置，直接解析收到的数据即可。  
- 比如，打开收音机听音乐，接受到广播的时候，歌曲可能刚开始，可能刚播放到一半也可能刚好是结尾。  
  
server   --->   socket    <---  client  
  
server:  
    media模块：存取所有待播放的数据（PS：**本程序使用文件系统，也可以使用mysql、oracle等**）  
    thr_list模块：节目单接口（PS：client接收到节目单之后选择频道，然后接收对应频道数据）  
    mytbf模块：提供流量控制接口  
    thr_channel模块：通过线程实现频道，每个线程对应一个频道，调用mytbf模块接口，通过流量控制发出数据  
    server模块：全局控制流程  
client:  
    主进程接收数据，发送给子进程  
    子进程调用解码器播放数据  
    切换频道  
```
#### 软件架构  
```
INSTALL --- 部署指导  
LISENCE --- 使用许可：需要遵循哪些协议  
README --- 软件介绍：介绍功能/内容  
Makefile --- 编译产品  
doc --- 项目相关文档  
    admin --- 管理员文档  
    devel --- 开发文档  
    user --- 用户文档  
src --- 源码  
```
#### 安装教程(ubuntu环境)  
  
1.  cd netradio/src/client;make;./client
2.  cd netradio/src/server;make;./server -F  
  
#### 使用说明  
问题：如果在虚拟机上运行，由于虚拟机不停的校验声卡时间轴，声音会断断续续(甚至没有声音)，建议真机尝试  
规避方案：调整流量，**medialib.c->MP3_BITRATE** 速率值调整比解码器稍微快一点即可(不要太高，建议小步径多尝试几次)  
  
  
#### 参与贡献  
  
搭建客户端和服务器端，实现相应功能  