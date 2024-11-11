#include "server.h"

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

stats_t server_stats = {0, 0, 0};

int handle_static(int sock_fd, const char *file_path) {
    char full_path[256];
    snprintf(full_path, sizeof(full_path), "./static%s", file_path + 7);  // Skip "/static"

    FILE *file = fopen(full_path, "rb");
    if (!file) {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        write(sock_fd, not_found, strlen(not_found));
        return -1;
    }

    struct stat file_stat;
    stat(full_path, &file_stat);
    int file_size = file_stat.st_size;
    server_stats.sent_bytes += file_size;

    char header[128];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: "
             "%d\r\n\r\n",
             file_size);
    write(sock_fd, header, strlen(header));

    char buffer[1024];
    int bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(sock_fd, buffer, bytes_read);
    }
    fclose(file);
    return 0;
}

int handle_stats(int sock_fd) {
    char response[256];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %ld\r\n\r\n"
             "<html><body><h1>Server Stats</h1>"
             "<p>Requests: %d</p>"
             "<p>Received bytes: %d</p>"
             "<p>Sent bytes: %d</p>"
             "</body></html>",
             strlen(response), server_stats.request_count, server_stats.received_bytes,
             server_stats.sent_bytes);

    write(sock_fd, response, strlen(response));
    return 0;
}

int handle_calc(int sock_fd, const char *query) {
    int a, b;
    if (sscanf(query, "a=%d&b=%d", &a, &b) != 2) {
        const char *bad_request = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        write(sock_fd, bad_request, strlen(bad_request));
        return -1;
    }

    char response[128];
    int result = a + b;
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %ld\r\n\r\n"
             "Sum: %d",
             strlen(response), result);

    write(sock_fd, response, strlen(response));
    return 0;
}

void *handle_connection(void *sock_fd_ptr) {
    int sock_fd = *(int *)sock_fd_ptr;
    free(sock_fd_ptr);

    char buffer[1024];
    int bytes_read = read(sock_fd, buffer, sizeof(buffer) - 1);
    buffer[bytes_read] = '\0';

    if (bytes_read <= 0) {
        close(sock_fd);
        return NULL;
    }
    server_stats.request_count++;
    server_stats.received_bytes += bytes_read;

    char method[8], path[256];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(method, "GET") != 0) {
        const char *bad_request =
            "HTTP/1.1 405 Method Not Allowed\r\nContent-Length: 0\r\n\r\n";
        write(sock_fd, bad_request, strlen(bad_request));
        close(sock_fd);
        return NULL;
    }

    if (strncmp(path, "/static/", 8) == 0) {
        handle_static(sock_fd, path);
    } else if (strcmp(path, "/stats") == 0) {
        handle_stats(sock_fd);
    } else if (strncmp(path, "/calc?", 6) == 0) {
        handle_calc(sock_fd, path + 6);
    } else {
        const char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        write(sock_fd, not_found, strlen(not_found));
    }
    close(sock_fd);
    return NULL;
}