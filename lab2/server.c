#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

FILE *fp = NULL;

// return 0 for success 1 for fail
int getDataFromMessage(char *msg) { 
    char delim[] = ":";
    char *total = strtok(msg,delim); 
    char *num = strtok(NULL, delim); 
    char *size = strtok(NULL, delim); 
    char *file = strtok(NULL, delim); 
    int isize = atoi(size);
    int inum = atoi(num); 
    int itot = atoi(total);
    if (inum == 1) {
        fp = fopen(file,"wb");
    } /*else { 
        fp = fopen(file,"ab");
    }*/
    if (fp == NULL) { 
        printf("Unknown error. Coulndt open/make file\n");  
        return 1;
    }
    
    int i = 0; 
    int start_len = strlen(total) + strlen(num) + strlen(size) + strlen(file) + 4;
	fwrite(msg+start_len,sizeof(char), isize,fp);
    if (inum == itot) {
        fclose(fp);
    }
    return 0;
   
    
}

int main(int argc, char *argv[]) { 
    if (argc < 2) { 
        printf("Incorrect usage: Use server <server port number> \n");
        return -1;
    }
    int sockfd,n;
    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[1100];

    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    int counter = 0;
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(argv[1]));
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    while (1) { 
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1100,0,(struct sockaddr *)&cliaddr,&len);
		int k = 0; printf("\n");for (k = 0; k < 1100; k++) { printf("%c",mesg[k]);} printf("\n");
        int sts =  getDataFromMessage(mesg);
        char sendline[1000];
        snprintf(sendline, 1000, "%d\n",sts);
        sendto(sockfd,sendline,strlen(sendline),0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
    }
    
}
