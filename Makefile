CC = gcc
CFLAGS = -pthread -g
PROG = s-talk
OBJS = list.o util.o main.o keyboard.o screen.o send.o recv.o

s-talk: $(OBJS)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJS)

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

main.o: main.c globals.h util.h
	$(CC) $(CFLAGS) -c main.c

keyboard.o: keyboard.c keyboard.h
	$(CC) $(CFLAGS) -c keyboard.c

screen.o: screen.c screen.h
	$(CC) $(CFLAGS) -c screen.c

send.o: send.c send.h
	$(CC) $(CFLAGS) -c send.c

recv.o: recv.c recv.h
	$(CC) $(CFLAGS) -c recv.c

clean:
	rm *.o s-talk
