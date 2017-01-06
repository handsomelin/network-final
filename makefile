all: server client
server: server.c
	gcc -g -Wall server.c -o server -lpthread

client: client.c
	gcc client.c -o client
