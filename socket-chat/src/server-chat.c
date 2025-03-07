#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>
#include "libChat.h"



void perror_exit(const char* str) {
    perror(str);
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: <prog> <Inet Port>\n");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);

    /* init server */
    chat_server s;
    server_init(&s, port);

    /* Call select() to monitor fd */
    fd_set rd_sockfd_set;
    struct timeval tv;
    int ret;

    while (1) {
        FD_ZERO(&rd_sockfd_set);
        /* 遍历 Server 的 Clients 数组，将描述符添加到 rd_socked_set 中 */
        FD_SET(s.sockfd, &rd_sockfd_set);

        for (int i = 0; i < s.max_client_sockfd; i ++) {
            if (s.clients[i].sockfd == 0) {
                continue;
            } else {
                FD_SET(s.clients[i].sockfd, &rd_sockfd_set);
            }
        }

        /* wait up to five seconds */
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        ret = select(s.max_client_sockfd + 1, &rd_sockfd_set, NULL, NULL, &tv);

        /* check return value */
        if (ret == -1) {
            perror("select(): ");
        } else if (ret) {
            /* check server socket first */
            if (FD_ISSET(s.sockfd, &rd_sockfd_set)) {
                /* 说明有client发送了连接请求 */
                struct sockaddr_in client_ip_addr;
                socketfd_t client_sockfd;
                client_sockfd = tcp_server_accept(s.sockfd, &client_ip_addr);
                server_add_client(&s, client_sockfd, client_ip_addr);
            }

            /* 遍历剩余的文件描述符， */
            for (int i = 0; i < s.max_client_sockfd; i ++) {
                if (FD_ISSET(i, &rd_sockfd_set)) {
                    char rx_msg[MAX_MSG_LEN]; 
                    ssize_t rx_n = read(s.clients[i].sockfd, rx_msg, MAX_MSG_LEN);
                    printf("%s%s", s.clients[i].name, rx_msg);
                }
            }
        } else {
            fprintf(stdout, "Time out!\n");
        }
        
    }
    
    



    return EXIT_SUCCESS;
}
