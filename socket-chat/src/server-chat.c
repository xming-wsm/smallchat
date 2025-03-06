#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
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
    /* Struct [sockaddr_in] Init*/
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    /* Convert IP string to binary form in network bit order automatically */
    if (!inet_pton(AF_INET, SERVER_ADDR_STR, &server_addr.sin_addr)) {
        perror_exit("Valid IPv4 Address\n");
    } else {
        fprintf(stdout, "Setting Server IP %s::%d\n",
            inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    }

    /* Socket Init */
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1) {
        perror_exit("Socket()");
    }

    if (bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        close(server_sockfd);
        perror_exit("bind(): ");
    }

    if (listen(server_sockfd, MAX_CLIENT_NUM) == -1) {
        close(server_sockfd);
        perror_exit("listen(): ");
    }

    char* rx_msg = malloc(sizeof(char) * MAX_MSG_LEN);
    struct sockaddr_in client_addr;
    socklen_t client_len;

    // while (1) {
        /* call accept(), waiting for client connect */
        int client_sockfd = accept(server_sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("accept() ");
            // break;
        }

        /* Blocking, wait for client message */
        ssize_t rx_n = read(client_sockfd, rx_msg, sizeof(char) * MAX_MSG_LEN);
        if (rx_n == -1) {
            perror("read() ");
            // break;
        }
        printf("Receive from Client(Port:%d): %s",
            ntohs(client_addr.sin_port), rx_msg);

        char* tx_msg = "Server> OKOK!";
        ssize_t tx_n = write(client_sockfd, tx_msg, strlen(tx_msg));
        close(client_sockfd);
    // }

    fprintf(stdout, "Close Server!\n");
    free(rx_msg);
    close(server_sockfd);
    return 0;
}
