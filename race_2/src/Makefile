
IDIR=../include
LDIR=../lib
ODIR=obj

CC=gcc
CFLAGS=-g -Wall -Werror -I$(IDIR)

HEAD=$(IDIR)/protocol.h $(IDIR)/common.h $(IDIR)/serialization.h \
			$(IDIR)/linked_list.h $(IDIR)/log_messages.h

OTHER_OBJ=$(ODIR)/common.o $(ODIR)/serialization.o \
		    $(ODIR)/linked_list.o $(ODIR)/log_messages.o

all: client.exe server.exe

# Additional object files.

$(ODIR)/common.o: common.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ common.c

$(ODIR)/serialization.o: serialization.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ serialization.c

$(ODIR)/linked_list.o: linked_list.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ linked_list.c

$(ODIR)/log_messages.o: log_messages.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ log_messages.c

# Client/Server object files.

$(ODIR)/client.o: client.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ client.c

$(ODIR)/server.o: server.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ server.c

# Client/Server executables.

client.exe: client.o $(OTHER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

server.exe: server.o $(OTHER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.exe $(ODIR)/*.o