#ifndef SERVER_H
#define SERVER_H

typedef enum { BAD_REQUEST, CLOSED_CONNECTION, MESSAGE } http_read_result_t;

typedef struct {
    int request_count;
    int received_bytes;
    int sent_bytes;
} stats_t;

extern stats_t server_stats;

int handle_static(int sock_fd, const char *file_path);
int handle_stats(int sock_fd);
int handle_calc(int sock_fd, const char *query);
void *handle_connection(void *sock_fd_ptr);

#endif