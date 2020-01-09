#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <errno.h>
/*
 * ssize_t write (int socketfd, const void *buffer, size_t size)
 * ssize_t recv(int socketfd, void *buffer, size_t size, int flag)
 *
 *
 * how to build
 * clang -g -o tcp_select_client tcp_select_client.c
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

    fd_set readmask;
    fd_set allreads;

    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(sockfd, &allreads);


    for(;;){

        readmask = allreads;

        int rc = select(sockfd+1, &readmask, NULL, NULL, NULL);

        if (rc <= 0){
            fprintf(stderr, "select failed:%d\n", errno);
            exit(-1);
        }

        if (FD_ISSET(sockfd, &readmask)) {

            bzero(data_recv, 256);

            int n = read(sockfd, data_recv, 256);

            if (n < 0) {
                fprintf(stderr, "error read\n");
                close(sockfd);
                exit(-1);
            } else if (n == 0) {
                fprintf(stderr, "server closed\n");
                close(sockfd);
                exit(0);
            }

            printf("%s\n", data_recv);

        }

        if(FD_ISSET(STDIN_FILENO, &readmask)){
            int i=0;
            bzero(data_send, 256);
            while ((data_send[i++] = getchar()) != '\n');
            if (i > 1) {
                size_t rt = write(sockfd, data_send, i - 1);

                if (rt <= 0) {
                    fprintf(stderr, "send error\n");
                    close(sockfd);
                    exit(-1);
                }
            }

        }

    }


    return 0;
}