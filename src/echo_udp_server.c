#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#define BUFLEN 512

int main(void) {
	// fprintf(stderr, "echo\n");
	struct sockaddr_in cliaddr;
	// int servlen = sizeof(servaddr);
	int slen=sizeof(cliaddr);
	char buf[BUFLEN];
	// int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	// servaddr.sin_addr.s_addr = inet_addr("172.17.33.138");
	// servaddr.sin_port = htons(7);
	// servaddr.sin_family = AF_INET;
	// int bindret = bind(sfd, (struct sockaddr *) &servaddr, servlen);
	memset(buf,0,sizeof(buf));
	if (recvfrom(STDIN_FILENO, buf, BUFLEN, 0, (struct sockaddr *) &cliaddr, &slen) == -1) {
	// if (recvfrom(sfd, buf, BUFLEN, 0, (struct sockaddr *) &cliaddr, &slen) == -1) {
		perror("recvfrom");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		exit(0);
	}
	if (sendto(STDOUT_FILENO, buf, strlen(buf) , 0 , (struct sockaddr *) &cliaddr, slen)==-1) {
	// if (sendto(sfd, buf, strlen(buf) , 0 , (struct sockaddr *) &cliaddr, slen)==-1) {
		perror("sendto");
		exit(0);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	return 0;
}