
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*

 one to one tcp demo

#functions for send data:

ssize_t write (int socketfd, const void *buffer, size_t size)
ssize_t send (int socketfd, const void *buffer, size_t size, int flags)
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)


#functions for read data:

ssize_t read (int socketfd, void *buffer, size_t size)
ssize_t recv(int, void *, size_t, int)
ssize_t recvfrom(int, void *, size_t, int, struct sockaddr * __restrict, socklen_t * __restrict)
ssize_t recvmsg(int, struct msghdr *, int)

 */

/**
 *
 * @param argc
 * @param argv
 * @return
 */

/*
 * TCP SERVER :
 * init socket
 * bind socket to ip(or server) and port
 * accept
 * read&write
 */

int main(int argc, char *argv[]) {
    int listenfd;
    int connfd;

    socklen_t client_len;
    struct sockaddr_in serverAddr, clientAddr;

    // AF_INET - IPV4, SOCK_STREAM TCP, 0 忽略
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((void *) &serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(16018);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int ret = bind(listenfd, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (ret != 0) {
        printf("socket bind failed, code=%d\n", ret);
    } else {
        printf("server socket bind to %d\n", 16018);
    }

    listen(listenfd, 1024);//backlog


    signal(SIGPIPE, SIG_IGN);

    client_len = sizeof(clientAddr);

    connfd = accept(listenfd, (struct sockaddr *) &clientAddr, &client_len);

    if (connfd < 0) {
        fprintf(stderr, "accept error\n");
    }

    char buf[256];

    for (;;) {

        bzero(buf, 256);

        int n = read(connfd, buf, 256);

        if (n > 0) {
            printf("%s\n", buf);
            write(connfd, buf, n);

            if (strcmp(buf, "exit") == 0) {
                printf("server close\n");
                break;
            }
        }
    }

    close(connfd);
    close(listenfd);

}
