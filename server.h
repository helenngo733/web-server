#ifndef HTTP_MSG_H
#define HTTP_MSG_H

typedef struct message {
    char *method;
    char *path;
    char *http_version;
    char *headers;
    char *body;
    int body_length;
} http_client_message_t;

typedef enum { BAD_REQUEST, CLOSED_CONNECTION, MESSAGE } http_read_result_t;

// responses: a message, bad request, closed connection
// allocates and returns a message
void read_http_client_message(int client_sock, http_client_message_t **message,
                              http_read_result_t *result);
void free_http_client_message(http_client_message_t *message);

#endif
