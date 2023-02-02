CC = gcc
all: server client MyDns

MyDns: MyDns.o
	$(CC)  -o MyDns MyDns.o

MyDns.o: MyDns.c
	$(CC)  -c MyDns.c

server: server.o
	$(CC)  -o server server.o

server.o: server.c
	$(CC)  -c server.c

client: client.o
	$(CC)  -o client client.o

client.o: client.c
	$(CC)  -c client.c

clean:
	rm -f *.o MyDns server client

.PHONY: clean
