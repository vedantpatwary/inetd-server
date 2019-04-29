all: inet
inet:	inet_server.c
	gcc -g inet_server.c -o inet
clean:
	rm inet