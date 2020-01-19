
This project contains the code I writen when I learn network programming.

Notice, if you are using linux system, using `gcc` instead of `clang`.

# 1. One to One TCP

You can read the source code `tcp_server.c` and `tcp_client.c`

`tcp_server.c` for tcp_server.

`tcp_client.c` for tcp_client.

*Build*

`clang -std=c11 -g -o tcp_server tcp_server.c`

`clang -std=c11 -g -o tcp_client tcp_client.c`

*RUN*

run server first:

`./tcp_server`

then run client:

`./tcp_client`



# 2. TCP Using `select`

You can read the source code `tcp_select_client.c`

*Build*


`clang -std=c11 -g -o tcp_client tcp_select_client.c`

You can use any tcp server to test this client.


# 3. TCP Using `poll`

You can read the source code `tcp_poll_server.c`

This tcp server support multiple clients.

*Build*

`clang -std=c11 -g -o tcp_poll_server tcp_poll_server.c`

You can use any tcp client to test this server.


# 4. TCP Using `epoll`

osx doesn't support epoll, you can only use linux to run this code.

The source code is `tcp_epoll_server.c`, any client code is OK.


# 5. epoll and multi-thread, is solution for C10K.


















I'm Helloa, you can contact me with email: yctx.yang@gmail.com.