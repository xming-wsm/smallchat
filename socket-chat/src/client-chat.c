#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <errno.h>

#define SERVER_ADDR_STR     "127.0.0.0"
#define SERVER_PORT         9009
#define MAX_CLIENT_NUM      50
#define MAX_MSG_LEN         1024

void perror_exit(const char* str) {
    perror(str);
    exit(EXIT_FAILURE);
}


int main(int argc, char** argv) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    /* Convert IP string to binary form in network bit order automatically */
    if (!inet_pton(AF_INET, SERVER_ADDR_STR, &server_addr.sin_addr)) {
        perror_exit("Valid IPv4 Address\n");
    } else {
        fprintf(stdout, "Connect to Server(%s::%d)\n",
            inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    }

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror_exit("Socket()");
    }

    /* Call connect() */
    if (connect(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror_exit("connect(): ");
    }

    /* send message to server    */
    char* tx_msg = "MSG Test\n";
    ssize_t tx_n = write(server_sockfd, tx_msg, strlen(tx_msg));
    fprintf(stdout, "user> MSG Test\n");

    
    /* receiver message from server */
    char* rx_msg = malloc(sizeof(char) * MAX_MSG_LEN);
    ssize_t rx_n = read(server_sockfd, rx_msg, sizeof(char) * MAX_MSG_LEN);
    fprintf(stdout, "%s\n", rx_msg);

    return 0;
}
