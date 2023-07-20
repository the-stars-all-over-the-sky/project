CFLAGS += -lm -Wall# 指定头文件寻找路径

all:cjson

cjson:example.o cJson.o
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -rf *.o cjson