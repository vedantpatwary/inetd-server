#include <sys/epoll.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>

#define MAX_LINE_LENGTH 200
fd_set allset, rset;

struct inet{
	char service[20];
	char sockType[20];
	char protocol[20];
	char flags[20];
	char login[20];
	char *execFile;
	char *args[10];
	int fd;
	struct inet *next;
};
typedef struct inet inet_t;

inet_t * fillStructure(FILE * fp);
int findInServices(char *name, char * protocol);
inet_t* add_inet(inet_t *head,inet_t *new_inet);
inet_t *findbyfd(inet_t *head,int fd);
inet_t *deletebyfd(inet_t *head,int fd);
void createSocketAddFd(inet_t *inet);
int execServer(inet_t * head, int ifd);
void handler(int signo);

int main() {
	FILE * fp = fopen("/etc/inetd.conf", "r");
	inet_t *head =NULL;
	int maxfd = 0;
	if(fp == NULL) {
		fp = fopen("inetd.conf", "r");
		if(fp == NULL) {
			fprintf(stderr, "No file /etc/inetd.conf present\n");
			exit(0);
		}
	}
	while(!feof(fp)) {
		char line[MAX_LINE_LENGTH];
		if(fgetc(fp)=='#'){
			fscanf(fp, "%[^\n]\n", line);
		}
		else{
			inet_t * inet = fillStructure(fp);
			createSocketAddFd(inet);
			head = add_inet(head,inet);
			maxfd = (maxfd<inet->fd)?inet->fd:maxfd;
			FD_SET (inet->fd, &allset);
		}
	}
	for(;;) {
		rset = allset;
		int ready = -1;
		do {
			ready = select (maxfd + 1, &rset, NULL, NULL, NULL);
		}
		while(ready == -1 && errno == EINTR);
		if(ready == -1) {
			perror("select");
			exit(0);
		}
		for (int ifd = 0; ifd <= maxfd; ifd++) {
			if (FD_ISSET (ifd, &rset)){
				int isWait = execServer(head, ifd);
			}
		}

	}

	return  0;
}
// /*

inet_t* add_inet(inet_t *head,inet_t *new_inet){
	if(head==NULL)
		return new_inet;
	new_inet->next = head;
	return new_inet;
}

inet_t *findbyfd(inet_t *head,int fd){
	inet_t *temp = head;
	while(temp && temp->fd!=fd)
		temp=temp->next;
	return temp;
}

inet_t *deletebyfd(inet_t *head,int fd){
	inet_t *temp = findbyfd(head,fd);
	if(temp==NULL)	//nothing to delete
		return head;
	if(temp==head){	//first node to delete
		temp=temp->next;
		free(head);
		return temp;
	}
	inet_t *prev = head;	//any other node than first
	while(prev->next!=temp)
		prev=prev->next;
	prev->next = temp->next;
	free(temp);
	return head;
}


inet_t * fillStructure(FILE * fp) {
	inet_t * inet =(inet_t*)malloc(sizeof(struct inet));
	fseek(fp, -1, SEEK_CUR);
	char line[MAX_LINE_LENGTH];
	fscanf(fp, "%s %s %s %s %s %[^\n]\n",inet->service,inet->sockType,inet->protocol,inet->flags,inet->login,line);
	printf("%s %s %s %s %s %s ",inet->service,inet->sockType,inet->protocol,inet->flags,inet->login,line);
	char delim[] = " \n";
	inet->execFile = strtok(line, delim);
	printf("exec = %s ", inet->execFile);
	int i=0;
	inet->args[i] = strtok(NULL, delim);
	while(inet->args[i] != NULL) {
		printf("args[%d]:%s ", i, inet->args[i]);
		i++;
		inet->args[i] = strtok(NULL, delim);
	}
	printf("\n");
	return inet;
}

int findInServices(char *name, char * protocol) {
	FILE * fpservices = fopen("/etc/services", "r");
	int port;
	char name2[MAX_LINE_LENGTH], protocol2[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH];
	while(!feof(fpservices)) {
		fscanf(fpservices, "%[^\n]\n", line);
		if(line[0] != '#') {
			sscanf(line, "%s %d/%s %[^\n]\n", name2, &port, protocol2, line);
			if(strcmp(name, name2) == 0 && strcmp(protocol, protocol2) == 0) {
				fclose(fpservices);
				return port;
			}
		}
	}
	fclose(fpservices);
	return -1;
}


void createSocketAddFd(inet_t *inet){
	struct sockaddr_in claddr, servaddr;
	if(strcmp((inet->sockType),"stream")==0){
		int lfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		bzero (&servaddr, sizeof (servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
		servaddr.sin_port = htons (findInServices(inet->service, inet->protocol));
		int bindret = bind (lfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
		if (bindret < 0)
			perror("bind");
		int lret = listen (lfd, 10);
		if(lret < 0)
			perror("error in listen");
		inet->fd = lfd;
	}
	else{
		int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
		inet->fd = sockfd;
	}
}

int execServer(inet_t * head, int ifd) {		// 1 return is WAIT
	inet_t * inetstr = findbyfd(head, ifd);
	struct sockaddr_in cli_addr;
	int clilen;
	if(strcmp(inetstr->protocol,"tcp")==0){						//TCP
		int confd = accept(inetstr->fd,(struct sockaddr*)&cli_addr,&clilen);
		if(fork()==0){		//child
			//close all fds except confd
			inet_t *temp = head;
			while(temp!=NULL){
				close(temp->fd);
				temp=temp->next;
			}
			dup2(confd, 0);
			dup2(confd, 1);
			execv(inetstr->execFile, inetstr->args);
			//exec
		}
		else{
			close(confd);
			if(strcmp(inetstr->flags,"wait")==0)
				FD_CLR(inetstr->fd,&allset);
		}
	}
	else{	//udp
		if(fork()==0){		//child
			//close all fds except confd
			inet_t *temp = head;
			while(temp!=NULL){
				if(temp->fd !=ifd)
					close(temp->fd);
				temp=temp->next;
			}
			dup2(confd, 0);
			dup2(confd, 1);
			execv(inetstr->execFile, inetstr->args);
			//exec
		}
		else{
			if(strcmp(inetstr->flags,"wait")==0)
				FD_CLR(inetstr->fd,&allset);
		}
	}
}