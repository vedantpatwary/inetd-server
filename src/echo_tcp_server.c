#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFLEN 100
#define MAXPENDING 5

int main() {
	// printf("here\n");
	char buf[BUFLEN];
	for(;;) {
		// printf("here\n");
		memset(buf,0,strlen(buf));
		int readret = read(STDIN_FILENO, buf, BUFLEN);
		if(readret < 0) {
			perror("read");
			exit(0);
		}
		else if(readret==0){
			close(STDIN_FILENO);
			close(STDOUT_FILENO);
			exit(0);
		}

		int wrtret = write(STDOUT_FILENO, buf, strlen(buf));
		if(wrtret < 0) {
			perror("write");
			exit(0);
		}
	}
}