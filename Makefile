CC = gcc
CFLAGS = -g  -Wall -ansi -pedantic
MAIN = mytar
OBJS = mytar.o create.o check.o extract.o list.o
all : $(MAIN)



$(MAIN) : $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS)


mytar.o : mytar.c header.h
	$(CC) $(CFLAGS) -c mytar.c 

create.o : create.c header.h
	$(CC) $(CFLAGS) -c create.c

check.o : check.c header.h
	$(CC) $(CFLAGS) -c check.c

extract.o : extract.c header.h
	$(CC) $(CFLAGS) -c extract.c

list.o : list.c header.h
	$(CC) $(CFLAGS) -c list.c

clean :
	rm *.o $(MAIN) core
