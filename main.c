#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "server.h"

#define DEFAULT_PORT 80
#define LISTEN_BACKLOG 5

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    }


    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_addr.s_addr = htonl(INADDR_ANY), .sin_port = htons(port)};


    bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(socket_fd, LISTEN_BACKLOG);


    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);


    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);


        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, client_fd);
        pthread_detach(thread);
    }
    close(socket_fd);
    return 0;
}
