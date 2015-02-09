#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

/* This func mallocs, free after ack received */
char * make_string(int num_packets, int pack_num, int size, char *file, char *data) {
    char * msg_str = (char *) calloc (1100,sizeof(char));
    sprintf(msg_str, "%d:%d:%d:%s:",num_packets, pack_num, size, file); 
    int i = 0; 
    int len = strlen(msg_str);
    for (i = 0; i < size; i++) {
        msg_str[len + i] = data[i];
    }
    return msg_str;
}

int main(int argc, char *argv[]) {
    
    if (argc < 5) { 
        printf("Incorrect usage: Use deliver <server address> <server port number> <client listen port> <file name> \n");
        return -1;
    }
 
    FILE *fp = fopen(argv[4],"rb");
    if (fp == NULL) { 
        printf("Cant open file %s\n",argv[4]);
        return -1;
    }
    fseek(fp, 0L, SEEK_END); 
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *file_as_string = (char *) malloc (size*sizeof(char));
    fread(file_as_string, sizeof(char), size, fp);
	//int l = 0; for (l = 0; l < size; l++ ) { printf("%c",file_as_string[l]);}
    fclose(fp);
    file_as_string[size] = '\0';

    int num_packets = size / 1000;
    if (num_packets % 1000 != 0) { 
        num_packets++;
    } 
	printf("Packets %d\n",num_packets);

    int fd, port;
    int serverlen;
    struct sockaddr_in servaddr;
    struct hostent *server;
    char recvline[1000];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    int try = 0;
    
    server = gethostbyname(argv[1]);
    port = atoi(argv[2]);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
    servaddr.sin_port=htons(port);
    int i = 0;
    for (i = 0; i < 1; i++) { 
        int p_size = (i == (num_packets - 1)) ? (size % 1000) : 1000;
        char *start_ptr = file_as_string + (i * 1000);
        char *sendline = make_string(num_packets, i + 1, p_size, argv[4], start_ptr);
		int k = 0; printf("\n");for (k = 0; k < 1100; k++) { printf("%c",sendline[k]);} printf("\n");
        sendto(fd,sendline,strlen(sendline),0,
             (struct sockaddr *)&servaddr,sizeof(servaddr));
        int n=recvfrom(fd,recvline,1000,0,NULL,NULL);
        recvline[n]=0;
        int sts = atoi(recvline);
        if (sts == 1 && try < 3) {
            try++;
            i--;
        } else if (sts == 0){ 
            try = 0;
        } else if (sts == 1 && try >= 3) { 
            printf("One packet failed three times. Abort\n");
        }
        free(sendline);
    }

    free(file_as_string);   
}
