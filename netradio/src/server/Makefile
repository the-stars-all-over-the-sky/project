CFLAGS += -I../include/ -pthread
LDFLAGS += -pthread

all:server

server:server.o thr_channel.o thr_list.o mytbf.o medialib.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS) # $@--目标文件，$^--所有的依赖文件

clean:
	rm -rf *.o server