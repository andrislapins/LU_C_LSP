
IDIR=../include
LDIR=../lib
ODIR=obj

CC=gcc
CFLAGS=-g -Wall -I$(IDIR)

HEAD=$(IDIR)/protocol.h $(IDIR)/common.h $(IDIR)/serialization.h $(IDIR)/my_strings.h

all: client.exe server.exe

# Additional object files.

$(ODIR)/common.o: common.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ common.c

$(ODIR)/serialization.o: serialization.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ serialization.c

$(ODIR)/my_strings.o: my_strings.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ my_strings.c

# Client/Server object files.

$(ODIR)/server.o: server.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ server.c

$(ODIR)/client.o: client.c $(HEAD)
	$(CC) $(CFLAGS) -c -o $@ client.c

# Client/Server executables.

client.exe: $(ODIR)/client.o $(ODIR)/common.o $(ODIR)/serialization.o $(ODIR)/my_strings.o
	$(CC) $(CFLAGS) -o $@ $^

server.exe: $(ODIR)/server.o $(ODIR)/common.o $(ODIR)/serialization.o $(ODIR)/my_strings.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.exe $(ODIR)/*.o