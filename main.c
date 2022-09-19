#include <sys/socket.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "array/array.h"
#include "logger/logger.h"


#define BACKLOG 50

// TODO: Tmp method
void print_exit(char* s) {
    perror(s);
    exit(1);
}

enum method {
    GET,
    POST,
    UPDATE,
    DELETE,
    UNKNOWN,
    NO_METHODS,
};
enum method get_method(char* raw_str) {
    if(strncmp("GET", raw_str, strlen(raw_str)) == 0) {
        return GET;
    } else if (strncmp("POST", raw_str, strlen(raw_str)) == 0) {
        return POST;
    } else if (strncmp("UPDATE", raw_str, strlen(raw_str)) == 0) {
        return UPDATE;
    } else if (strncmp("DELETE", raw_str, strlen(raw_str)) == 0) {
        return DELETE;
    } else {
        return UNKNOWN;
    }
}
typedef void (*req_handler)(int);
struct server {
    char* ip;
    int port;
    int socketfd;
    struct array handlers[NO_METHODS];
} s; // Global server variable for so i can access it to do cleanup...

// Sample Handlers
void SAMPLE_GET_handler(int reqfd) {
    char* msg =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 0\r\n"
        "\r\n"
        ;
    send(reqfd,msg, strlen(msg),0);
    log_debug("Responded with GET Handler\n");
};
void SAMPLE_FALLBACK_handler(int reqfd) {
    char* msg =
        "HTTP/1.1 404 BAD REQUEST\r\n"
        "Content-Length: 0\r\n"
        "\r\n"
        ;
    send(reqfd,msg, strlen(msg),0);
    log_debug("Responded with FALLBACK handler\n");
};

// Server Utils
struct http_request {
    int sockfd;
    char* protocol;
    char* http_vers;
    enum method method;
    char *raw; // Must free
    struct array headers;
};
struct http_request parse_http_request(int reqfd) {
        // Process request
        #define RECV_BUFF_SIZE 100
        char buff[RECV_BUFF_SIZE] = {0};
        char* raw = NULL;
        size_t raw_sz = 0;
        int bytes_recv;

        do {
            // Keep recv-ing and pushing into raw
            bytes_recv = recv(reqfd, buff, sizeof(buff), 0);
            if(bytes_recv == -1) break;

            log_debug("[sockid %d] Filling raw with buffer (size: %d bytes)\n", reqfd, bytes_recv);

            raw = realloc(raw, raw_sz + bytes_recv);
            strncpy(raw + raw_sz, buff, bytes_recv);
            raw_sz += bytes_recv;
        } while (bytes_recv == RECV_BUFF_SIZE && buff[RECV_BUFF_SIZE - 1] != '\0');

        if (bytes_recv == -1)  {
            log_err("Error occured while reading request.");
            return (struct http_request){.method = UNKNOWN};
        }

        // Request structure
        // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
        // GET / HTTP/1.1
        // enum method method;
        char* method_raw = strtok(raw, " ");
        char* request_uri = strtok(NULL, " ");
        char* http_vers = strtok(NULL, "\r\n");
        log_debug("Received Request: %s %s %s\n", method_raw, request_uri, http_vers);

        // Getting Headers
        char *h;
        struct array headers;
        while(1) {
            h = strtok(NULL, "\r\n");
            if(h == NULL) break;
            // TODO: fix header
            // char* s = strdup(h);
            // array_push(&headers, s);
        }
        // array_foreach(&headers, printf);

        // Getting Payload
        return (struct http_request){ .method = GET };

}

// Server related
struct server server_create(char* ip, int port){
    // Get Socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    // Bind Socket / Name socket
    struct sockaddr_in server_add = {0};
    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(port); // convert port number to network byte order / big endian..i think
    server_add.sin_addr.s_addr = inet_addr(ip); // convert str ip to binary
    if (bind(sockfd, (struct sockaddr *)&server_add, sizeof(server_add)) == -1) print_exit("Failed to bind"); // Failed to bind


    char* _ip = strdup(ip);
    if(_ip == NULL) print_exit("Failed to strdup ip.");

    struct server res = {_ip, port, sockfd, {0}};
    for(int i =0; i < NO_METHODS; i++)
        res.handlers[i].esize = sizeof(req_handler);
    return res;
};
void server_listen(struct server s){

    listen(s.socketfd, BACKLOG);

    int reqfd;
    struct sockaddr req_addr;
    socklen_t req_size = sizeof(req_addr);

    while(1) {
        reqfd = accept(s.socketfd, &req_addr, &req_size);
        if (reqfd == -1) log_err("accept call Error");

        // Parse Request
        struct http_request req = parse_http_request(reqfd);

        // Handling Requests
        req_handler hdlr = NULL;
        switch (req.method) {
            case GET:
                hdlr = *(req_handler*) array_get(s.handlers[GET], 0);
                break;
            default:
                hdlr = SAMPLE_FALLBACK_handler;
        }
        if(hdlr != NULL) hdlr(reqfd);


        // Clean up request
        close(reqfd);
        free(req.raw);
    }
};
void server_cleanup() { // TODO: how to do a signal triggered cleanup without relying on global var
    close(s.socketfd);
    free(s.ip);
    for(int i =0; i < NO_METHODS; i++)
        array_free(s.handlers[i]);
}

// Adding Handlers
// TODO : How do i register routes?
void get(char* path, req_handler hdlr) {
    array_push(&s.handlers[GET], hdlr);
}
void post(char* path, req_handler hdlr) {}
void update(char* path, req_handler hdlr) {}
void delete(char* path, req_handler hdlr) {}

// Misc
void sigterm_handler(int _) {
    log_debug("SIGTERM called\n");
    server_cleanup();
    exit(0);
};

int main() {

    signal(SIGINT, sigterm_handler);

    s = server_create("127.0.0.1", 8080);
    get("/", SAMPLE_GET_handler);
    server_listen(s);
    server_cleanup();
}

