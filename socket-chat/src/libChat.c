#define _POSIX_C_SOURCE     200112L

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "libChat.h"


/*  @brief create tcp server
    @param Server 端口号
    @return if sucess, return the file description of server socket, 
            otherwise return -1.
 */
socketfd_t tcp_server_create(int port) {
    socketfd_t sockfd;
    int optVal = 1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Server Create Failed: ");
        return -1;
    }

    /*  Set socket option: SO_REUSEADDR 选项允许 socket 在关闭后连接本地端口，释放
        原先的端口，这样就不会出现端口被占用的情况. 若不设置该 option, 套接字关闭后会
        进入 TIME_WAIT 状态，在此状态下套接字所使用的本地地址和端口是无法被其他套接字
        使用的。
    */
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optVal, sizeof(optVal));
    
    /* bind() and listen() */
    struct sockaddr_in serv_ip;
    memset(&serv_ip, 0, sizeof(serv_ip));
    serv_ip.sin_family = AF_INET;
    serv_ip.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_ip.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&serv_ip, sizeof(serv_ip)) == -1 || listen(sockfd, MAX_CLIENT_NUM) == -1) {
        perror("bind(): ");
        close(sockfd);
        return -1;
    }

    return sockfd;
}


/** 
    @brief  调用 accept 函数，等待 client 连接.
    @param  serv_sockfd 服务端的套接字文件描述符
 */
socketfd_t tcp_server_accept(int serv_sockfd, struct sockaddr_in* client_addr) {
    /* 创建客户端套接字文件描述符 */
    socketfd_t client_sockfd;
    struct sockaddr_in client_ip;
    socklen_t client_ssize_len;

    while (1) {
        /* accept 会从等待序列中选取一个请求 */
        client_sockfd = accept(serv_sockfd, (struct sockaddr*)&client_ip, &client_ssize_len); 
        if (client_sockfd == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept(): ");
            return -1;
        } else {
            break;
        }
    }

    *client_addr = client_ip;
    return client_sockfd;
}


/** 
    @brief 将 fd 和 client_addr 添加到 server 的客户端数组中.
    @param fd 客户端文件描述符
    @param client_addr 客户端 IP 地址与端口
 */
int server_add_client(chat_server* s, socketfd_t fd, struct sockaddr_in client_addr) {
    /* 更新客户端数量 */
    s->client_nums ++;

    /* 更新客户端数组 */
    s->clients[fd].sockfd = fd;
    s->clients[fd].ip = client_addr;
    sprintf(s->clients[fd].name, "user%d", fd);

    /* 添加客户端文件描述符 */
    if (fd > s->max_client_sockfd) {
        s->max_client_sockfd = fd;
    }
    return CHAT_OK;
}




/*  @brief  根据给定的网络地址、端口号连接目标服务器
    @param  
    @param  
    @note   由于服务器协议未知，tcp_Connect() 会先调用 getaddrinfo() 获取
            服务器的协议后，在创建对应的 socket 进行连接
 */
int tcp_connect(char* addr, char* port) {

}


/**
   @brief  server 初始化
   @param  s 服务器结构体指针
   @param  port 服务器端口号
   @return CHAT_OK if success, otherwise CHAT_ERR. 
 */
int server_init(chat_server* s, int port) {
    socketfd_t fd_tmp = tcp_server_create(port);
    if (fd_tmp == -1) {
        return -1;
    }

    s->sockfd = fd_tmp;
    s->client_nums = 0;
    s->max_client_sockfd = 0;
    memset(s->clients, 0, sizeof(chat_client) * MAX_CLIENT_NUM);


    return CHAT_OK;
}

