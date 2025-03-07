#ifndef __LIBCHAT_H
#define __LIBCHAT_H

#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENT_NUM      512
#define MAX_MSG_LEN         1024

#define CHAT_OK     1
#define CHAT_ERR    0


typedef int socketfd_t;

/* struct of client */
typedef struct chat_client {
    socketfd_t sockfd;      /* client fd */
    char name[20];          /* client user name */
    struct sockaddr_in ip;  /* client ip information */
} chat_client;


/* struct of server */
typedef struct chat_server {
    int sockfd;        /* server fd */
    int client_nums;        /* client nums */
    int max_client_sockfd;  /* the greatest client sockfd */    
    chat_client clients[MAX_CLIENT_NUM];
} chat_server;



/* networking API */
socketfd_t tcp_server_create(int port);
socketfd_t tcp_server_accept(int serv_sockfd, struct sockaddr_in* client_addr);
int tcp_connect();


/* server API */
int server_init(chat_server* s, int port);
int server_free(chat_server* s);
int server_add_client(chat_server* s, socketfd_t fd, struct sockaddr_in client_addr);



/* client API */




#endif /* __LIBCHAT_H */
