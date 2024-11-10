#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_msg.h"

#define PORT 45601
#define LISTEN_BACKLOG 5

int response_to_client(int sock_fd, http_client_message_t *http_message) {
    char *response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
    write(sock_fd, response, strlen(response));
    return 0;
}

void *handle_connection(void *sock_fd_ptr) {
    int sock_fd = *(int *)sock_fd_ptr;
    free(sock_fd_ptr);

    while (1) {
        printf("Handling Connection on %d\n", sock_fd);

        http_client_message_t *http_message;
        http_read_result_t result;

        read_http_client_message(sock_fd, &http_message, &result);

        if (result == BAD_REQUEST) {
            printf("Bad Request\n");
            close(sock_fd);
            return NULL;
        } else if (result == CLOSED_CONNECTION) {
            printf("Closed Connection\n");
            close(sock_fd);
            return NULL;
        }
        response_to_client(sock_fd, http_message);
        free_http_client_message(http_message);
    }
    printf("Done with connection %d\n", sock_fd);
}

int main(int argc, char *argv[]) {
    int port = PORT;

    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
        port = atoi(argv[2]);
    } else {
        fprintf(stderr, "Usage: %s -p <port>\n", argv[0]);
        return 1;
    }

    // Create a socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Configure the server address strcuture
    struct sockaddr_in socket_address;
    memset(&socket_address, '\0', sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address.sin_port = htons(port);

    // Bind the socket to the address and start listening
    int return_value;
    return_value = bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));
    if (return_value < 0) {
        perror("bind");
        return 1;
    }
    return_value = listen(socket_fd, LISTEN_BACKLOG);

    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);

    // Accept connections and create threads to handle them
    while (1) {
        int *client_fd = malloc(sizeof(int));
        *client_fd =
            accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);

        printf("Accept connection on %d\n", *client_fd);

        pthread_t thread;
        pthread_create(&thread, NULL, handle_connection, client_fd);
        pthread_detach(thread);
    }
    close(socket_fd);
    return 0;
}
