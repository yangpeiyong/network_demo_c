#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * ssize_t write (int socketfd, const void *buffer, size_t size)
 *
 *
 * how to build
 * clang -g -o tcp_client tcp_client.c
 * */
int main(int argc, char *argv[]){

    int sockfd;

    struct sockaddr_in serv_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((void*)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(16018);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int r = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if(r<0){
        printf("connect failed:%d\n",r);
        exit(-1);
    }

    printf("connect success\n");

    char data_send[256];
    char data_recv[256];

    for(;;){

        bzero(data_send, 256);
        bzero(data_recv, 256);

        int i=0;

        while((data_send[i++]=getchar()) != '\n');


        if(i>1) {
            size_t rt = write(sockfd, data_send, i - 1);

            if(rt <= 0){
                fprintf(stderr,"send error\n");
                close(sockfd);
                exit(-1);
            }

            int n = read(sockfd, data_recv, 256);

            if(n<0) {
                fprintf(stderr, "error read\n");
                close(sockfd);
                exit(-1);
            } else if(n==0){
                fprintf(stderr, "server closed\n");
                close(sockfd);
                exit(0);
            }

            printf("%s\n", data_recv);

        }

    }


    return 0;
}