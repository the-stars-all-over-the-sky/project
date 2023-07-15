CFLAGS += -I../include/ -Wall  -pthread# 指定头文件寻找路径

all:client

client:client_handle.o client_log.o client_user.o cmsg_dec.o cmsg_enc.o list.o msg_cipher.o cusr_cmd.o client_main.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o client