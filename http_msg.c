#include "http_msg.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

bool complete_http_message(char *buffer) {
    if (strlen(buffer) < 10) {
        return false;
    }
    if (strncmp(buffer, "GET ", 4) != 0) {
        return false;
    }
    if (strncmp(buffer + strlen(buffer) - 4, "\r\n\r\n", 4) != 0) {
        return false;
    }
    return true;
}

void read_http_client_message(int client_sock, http_client_message_t **message,
                              http_read_result_t *result) {
    *message = malloc(sizeof(http_client_message_t));
    char buffer[1024];
    strcpy(buffer, "");

    while (!complete_http_message(buffer)) {
        int bytes_read =
            read(client_sock, buffer + strlen(buffer), sizeof(buffer) - strlen(buffer));
        if (bytes_read == 0) {
            *result = CLOSED_CONNECTION;
            return;
        }
        if (bytes_read < 0) {
            *result = BAD_REQUEST;
            return;
        }
    }
    (*message)->method = strdup(buffer + 4);
    *result = MESSAGE;
}

void free_http_client_message(http_client_message_t *message) { free(message); }
