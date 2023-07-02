CFLAGS += -I../include/ -Wall # 指定头文件寻找路径

all:client

client:client.o
	gcc $^ -o $@ $(CFLAGS) # $@--目标文件，$^--所有的依赖文件

clean:
	rm -rf *.o client