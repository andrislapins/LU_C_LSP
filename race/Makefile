
CC=gcc
CFLAGS=-g
LFLAGS=-lpthread -lncurses
COMMON_OBJS=common.o player.o serialization.o

all: common.o player.o serialization.o server.out client.out

common.o: common.h common.c
	$(CC) $(CFLAGS) -c common.c

player.o: player.h player.c
	$(CC) $(CFLAGS) -c player.c $(LFLAGS)

serialization.o: serialization.h serialization.c
	$(CC) $(CFLAGS) -c serialization.c $(LFLAGS)

server.out: server.c
	$(CC) -I $(CFLAGS) -o server.out server.c $(COMMON_OBJS) $(LFLAGS)

client.out: client.c
	$(CC) $(CFALGS) -o client.out client.c player.o serialization.o common.o $(LFLAGS)

clean: 
	rm -rf *.out