#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define BUFLEN 512

int main(void) {
	struct sockaddr_in cliaddr;
	int slen=sizeof(cliaddr);
	char buf[BUFLEN];
	memset(buf,0,sizeof(buf));
	if (recvfrom(STDIN_FILENO, buf, BUFLEN, 0, (struct sockaddr *) &cliaddr, &slen) == -1) {
		perror("recvfrom");
		exit(0);
	}
	if (sendto(STDOUT_FILENO, buf, strlen(buf) , 0 , (struct sockaddr *) &cliaddr, slen)==-1) {
		perror("sendto");
		exit(0);
	}
	return 0;
}