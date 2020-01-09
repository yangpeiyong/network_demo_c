
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>

#define MAX_CONN 128

/**
 *
 * clang -std=c11 -g -o tcp_poll_server tcp_poll_server.c
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {

    int listenfd;
    struct sockaddr_in serverAddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero((void *) &serverAddr, sizeof(serverAddr));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(16018);

    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // AF_INET - IPV4, SOCK_STREAM TCP, 0 忽略
    int ret = bind(listenfd, (const struct sockaddr *) &serverAddr, sizeof(serverAddr));

    if (ret != 0) {
        printf("socket bind failed, code=%d\n", ret);
    } else {
        printf("server socket bind to %d\n", 16018);
    }

    listen(listenfd, 1024);//1024 is backlog

    signal(SIGPIPE, SIG_IGN);


    struct pollfd event_set[MAX_CONN];

    event_set[0].fd=listenfd;
    event_set[0].events = POLLRDNORM;

    for (int i = 1; i < MAX_CONN; i++) {
        event_set[i].fd = -1;
    }

    struct sockaddr_in clientAddr;
    socklen_t client_len = sizeof(clientAddr);

    char buf[256];

    for (;;) {

        int ready_number = 0;

        if((ready_number = poll(event_set, MAX_CONN, -1)) <0){
            fprintf(stderr, "poll failed:%d\n", errno);
        }

        if(event_set[0].revents & POLLRDNORM) {
            int connfd = accept(listenfd, (struct sockaddr*) &clientAddr, &client_len);

            int i;
            for(i = 1; i<MAX_CONN; i++){
                if(event_set[i].fd<0){
                    event_set[i].fd = connfd;
                    event_set[i].events = POLLRDNORM;
                    break;
                }
            }

            if(i == MAX_CONN) {
                fprintf(stderr, "can not hold so many clients:%d\n",errno);
                close(connfd);
            }

            if (--ready_number <=0) continue;
        }

        for(int i=1; i< MAX_CONN; i++) {
            int socket_fd = event_set[i].fd;

            if (socket_fd < 0){
                continue;
            }

            if (event_set[i].revents & (POLLRDNORM | POLLERR)){

                bzero(buf, 256);

                int n = read(socket_fd, buf, 256);

                if (n > 0) {
                    printf("%s\n", buf);
                    write(socket_fd, buf, n);
                    if (strcmp(buf, "exit") == 0) {
                        printf("server close\n");
                        close(socket_fd);
                        event_set[i].fd = -1;
                    }
                } else if(n==0 || errno == ECONNRESET) {
                    close(socket_fd);
                    event_set[i].fd = -1;
                } else {
                    fprintf(stderr, "read error:%d\n",errno);
                    close(socket_fd);
                    event_set[i].fd = -1;
                }

            }

            if (--ready_number <=0) break;

        }

    }

    close(listenfd);

}
