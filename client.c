#include<stdio.h>
#include<ctype.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8989
#define BUFFSIZE 1024

/* write to the socket using thread */
void *writeToSock(void* sockfd) {
    char buff[BUFFSIZE];

    while(1) {
    uint8_t n = 0;
        while((n < BUFFSIZE) && ((buff[n++] = getchar()) != '\n')) 
            ;
        write(*(int*)sockfd, buff, sizeof(buff));
    }
    return NULL;
}

/* trim the read bytes */
void sanitize(char* buff, int readbytes) {
    while(readbytes-- > 0) {
        if(isspace(buff[readbytes])) {
            buff[readbytes] = '\0';
        } else {
            break;
        }
    }
}

/* read the socket */
void communicate(int sockfd) {
    char buff[BUFFSIZE];
    while(1){
        int readbytes = read(sockfd, buff, (BUFFSIZE - 1));
        if(readbytes > 0) {
            sanitize(buff, readbytes);
            puts(buff);
        } else {
            printf("Connection lost\n");
            return;
        }
    }
}

int main (int argc, char const* argv[]) {

    int sockfd;
    struct sockaddr_in servaddr;
    pthread_t thread_id;

    /* create socket with IPv4, TCP */
    sockfd = socket(AF_INET, SOCK_STREAM , 0);
    if(sockfd < 0) {
        perror("create socket cannot create socket!\n");
        return -1;
    } else {
        printf("Socket created successfully\n");
        /* server socket informaton */
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(IPADDRESS);
        servaddr.sin_port = htons(PORT);
        /* connect the client socket to server socket */ 
        if(connect (sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
            printf("Connection with server failed\n");
            exit(0);
        } else {
            printf("connected to the server\n");
        }
    }

    if(pthread_create(&thread_id, NULL, writeToSock, &sockfd) != 0) {
        perror("error in creating thread\n");
        exit(1);
    }

    /* function for read write */
    communicate(sockfd);
    
    /* thread join */
    if(pthread_join(thread_id, NULL) != 0) {
        perror("pthread error\n");
        exit(3);
    }

    /* close the socket*/
    close(sockfd);

    return 0;

}
