CFLAGS += -I../include/ -Wall  -pthread# 指定头文件寻找路径

all:server

server:list.o server_enc.o server_dec.o server_handle.o server_user.o server_main.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o server