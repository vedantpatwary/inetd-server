all: echo_udp_server echo_tcp_server inet
inet:	inet_server.c
	gcc -g inet_server.c -o inet
echo_udp_server:	echo_udp_server.c
	gcc -g echo_udp_server.c -o echo_udp_server
echo_tcp_server:	echo_tcp_server.c
	gcc -g echo_tcp_server.c -o echo_tcp_server
clean:
	rm echo_tcp_server echo_udp_server