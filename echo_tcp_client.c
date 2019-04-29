#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RCVBUFSIZE 32

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in echoServAddr;
    unsigned short echoServPort;
    char *servIP;
    char *echoString = malloc(200);;
    char echoBuffer[RCVBUFSIZE];
    unsigned int echoStringLen;
    int bytesRcvd, totalBytesRcvd;

    if ((argc < 3) || (argc > 4))        {
       fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
               argv[0]);
       exit(1);
    }

    servIP = argv[1];     	//server IP in argv1
    //strcpy(echoString,argv[2]);         //message in argv2

    if (argc == 4)
        echoServPort = atoi(argv[3]); 	//server port no in argv3,else 7 default
    else
        echoServPort = 7;


    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)	//create a tcp socket
        DieWithError("socket() failed");


    memset(&echoServAddr, 0, sizeof(echoServAddr));

		//get server address structure

    echoServAddr.sin_family      = AF_INET;
    echoServAddr.sin_addr.s_addr = inet_addr(servIP);
    echoServAddr.sin_port        = htons(echoServPort);

    	//connect to that server

    if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("connect() failed");

	if(fork()==0){
			while(read(0,echoString,200)>0){
	   			echoStringLen = strlen(echoString);
			//send data to server
	    		if (send(sock, echoString, echoStringLen, 0) != echoStringLen)
	    		    DieWithError("send() sent a different number of bytes than expected");
				memset(echoString,0,200);
			}
			shutdown(sock,SHUT_WR);//sends the FIN segment
			exit(0);
	}
	else{
	        while ((bytesRcvd = recv(sock, echoBuffer, RCVBUFSIZE - 1, 0)) > 0){
	            //DieWithError("recv() failed or connection closed prematurely");
	        echoBuffer[bytesRcvd] = '\0';
	        printf("Received:%s", echoBuffer);
			memset(echoBuffer,0,200);
	    }

	    printf("\n");
	}
	printf("exited");
    close(sock);
    exit(0);
}