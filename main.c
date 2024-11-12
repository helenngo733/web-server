#include <arpa/inet.h>
#include <dirent.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "server.h"

#define DEFAULT_PORT 80
#define LISTEN_BACKLOG 5

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    } else {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        return 1;
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    int return_value;
    return_value = bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));
    if (return_value == -1) {
        perror("Bind Failed");
        return 1;
    }
    return_value = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    printf("Server listening on port %d\n", port);
    printf("Command format: GET <command> HTTP/1.1\n");  // using telent

    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd =
            accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, client_fd);
        pthread_detach(thread);
    }
    close(socket_fd);
    return 0;
}
