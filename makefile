# Makefile for COS 217 Assignment 3

CC = gcc217

all: testsymtablelist testsymtablehash

clean:
	rm -f testsymtablelist testsymtablehash *.o

# ---- linked list ----

testsymtablelist: testsymtable.o symtablelist.o
	$(CC) testsymtable.o symtablelist.o -o testsymtablelist

symtablelist.o: symtablelist.c symtable.h
	$(CC) -c symtablelist.c

# ---- hash table ----

testsymtablehash: testsymtable.o symtablehash.o
	$(CC) testsymtable.o symtablehash.o -o testsymtablehash

symtablehash.o: symtablehash.c symtable.h
	$(CC) -c symtablehash.c

# ---- shared client ----

testsymtable.o: testsymtable.c symtable.h
	$(CC) -c testsymtable.c
