
CC=gcc
CFLAGS=-g
LFLAGS=-lpthread -lncurses
DEPS=common.h player.h

all: chat_server.out chat_client.out player.o

chat_server.out: chat_server.c
	$(CC) $(CFLAGS) -o chat_server.out chat_server.c $(LFLAGS)

chat_client.out: chat_client.c
	$(CC) $(CFALGS) -o chat_client.out chat_client.c $(LFLAGS)

player.o: player.h player.c
	$(CC) $(CFLAGS) -c player.c $(LFLAGS)

clean: rm -rf *.out