#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

int main(void)
{
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;
    while(1)
    {
        unsigned char offset_buffer[10] = {'\0'};
        unsigned char command_buffer[2] = {'\0'};
        int offset;
        int command;
        while(read(STDIN_FILENO, command_buffer, 2) == 0);
                sscanf(command_buffer, "%d", &command);

        if(command == 0)
                offset = 0;
        else
        {
                while(read(STDIN_FILENO, offset_buffer, 10) == 0);
                sscanf(offset_buffer, "%d", &offset);

        }


        /* Open the file that we wish to transfer */
        FILE *fp = fopen("test","rb");
        if(fp==NULL)
        {
            return 1;
        }

        /* Read data from file and send it */
                 fseek(fp, offset, SEEK_SET);
        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[256]={0};
            int nread = fread(buff,1,256,fp);

            /* If read was success, send data. */
            if(nread > 0) {
		// fprintf(stderr, "nread=%d text=%s\n", nread, buff);
                write(STDOUT_FILENO, buff, nread);
            }

            /*
             * There is something tricky going on with read ..
             * Either there was error, or we reached end of file.
             */
            if (nread < 256)
            {
		// fprintf(stderr, "finished file\n");
		write(STDOUT_FILENO, "", 0);
		close(STDIN_FILENO);
		return 0;
            }


        }

        close(STDIN_FILENO);
        sleep(1);
    }


    return 0;
}


