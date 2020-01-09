
#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_CONN 128

/**
 *
 * gcc -std=c11 -g -o tcp_epoll_server tcp_epoll_server.c
 * osx use kqueue instead of epoll
 * this code cann't run under osx
 */

#define MAX_EVENT_NUMBER 16
int main(int argc, char *argv[]) {

    int listenfd;
    int epollfd;
    struct sockaddr_in serverAddr;
    struct epoll_event event; //for listenfd
    struct epoll_event *events;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(listenfd, F_SETFL, O_NONBLOCK);

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

    epollfd = epoll_create1(0);

    if (epollfd == -1) {
        fprintf(stderr,"epoll create failed\n");
        exit(-1);
    }

    event.data.fd = listenfd;
    event.events = EPOLLIN | EPOLLET;

    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event) == -1){
        fprintf(stderr,"epoll_ctl add listenfd fail\n");
        exit(-1);
    }


    events = calloc(MAX_EVENT_NUMBER, sizeof(event));

    for(;;) {

        int n = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        printf("epoll_wait wakeup\n");

        for(int i=0; i<n; i++){

            if((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
                fprintf(stderr, "epoll error\n");
                close(events[i].data.fd);
            } else if (listenfd == events[i].data.fd) {

                struct sockaddr_in clientAddr;
                struct sockaddr_storage ss;
                socklen_t client_len = sizeof(clientAddr);

                int connfd = accept(listenfd, (struct sockaddr*) &clientAddr, &client_len);

                if(connfd<0) {
                    fprintf(stderr, "accept failed\n");
                    exit(-1);
                }

                fcntl(connfd, F_SETFL, O_NONBLOCK);

                event.data.fd = connfd;
                event.events = EPOLLIN | EPOLLET;

                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &event) == -1){
                    fprintf(stderr,"epoll_ctl add connection fd fail\n");
                    exit(-1);
                }

            } else {
                int socket_fd = events[i].data.fd;

                char buf[256];

                while(1) {
                    bzero(buf, 256);
                    int n = read(socket_fd, buf, 256);

                    if (n > 0) {
                        printf("%d:%s\n",socket_fd, buf);
                        write(socket_fd, buf, n);
                        if (strcmp(buf, "exit") == 0) {
                            printf("client exit\n");
                            close(socket_fd);
                        }
                    } else if(n==0) {
                        close(socket_fd);
                        break;
                    } else {
                       if (errno != EAGAIN) {
                            fprintf(stderr, "read error\n");
                            close(socket_fd);

                       }
                       break;
                    }
                }

            }
        }
    }

    free(events);
    close(listenfd);

}
