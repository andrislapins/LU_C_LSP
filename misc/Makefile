
CC=gcc
CFLAGS=-g -Wall

all: frexp.out bit_fields.out bit_fields_macros.out

frexp.out: frexp.c
	$(CC) $(CFLAGS) -o $@ $^

bit_fields.out: bit_fields.c
	$(CC) $(CFLAGS) -o $@ $^

bit_fields_macros.out: bit_fields_macros.c
	$(CC) $(CFLAGS) -o $@ $^