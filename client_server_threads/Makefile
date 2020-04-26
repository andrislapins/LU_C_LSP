
CC=gcc
CFLAGS=-g
DT_PATH=../data_structures/

COMMON_OBJS = common.o $(DT_PATH)myqueue.o

all: client.bin server.bin $(COMMON_OBJS) server_multi_threaded.bin ncurses.bin

common.o: common.h common.c
	$(CC) $(CFLAGS) -c common.c

myqueue.o: $(DT_PATH)myqueue.h $(DT_PATH)myqueue.c
	$(CC) $(CFLAGS) -c $(DT_PATH)myqueue.c

client.bin: client.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o client.bin client.c $(COMMON_OBJS)

server.bin: server.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o server.bin server.c $(COMMON_OBJS)

server_multi_threaded.bin: server_multi_threaded.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o server_multi_threaded.bin server_multi_threaded.c $(COMMON_OBJS) -lpthread

ncurses.bin: ncurses.c
	$(CC) $(CFALGS) -o ncurses.bin ncurses.c -lncurses

clean:
	rm -rf *.dSYM *.o client.bin server.bin client.bin