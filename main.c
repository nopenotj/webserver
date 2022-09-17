#include <sys/socket.h>
#include <stdio.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "array/array.h"
#include "logger/logger.h"


#define BACKLOG 50

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
    printf("Responded\n");
};
void SAMPLE_FALLBACK_handler(int reqfd) {
    char* msg =
        "HTTP/1.1 404 BAD REQUEST\r\n"
        "Content-Length: 0\r\n"
        "\r\n"
        ;
    send(reqfd,msg, strlen(msg),0);
    printf("Responded\n");
};

// Server related

struct server server_create(char* ip, int port){
    // Get Socket
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    // Bind Socket / Name socket
    struct sockaddr_in server_add = {0};
    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(port); // convert port number to network byte order / big endian..i think
    server_add.sin_addr.s_addr = inet_addr(ip); // convert str ip to binary
    if (bind(sockfd, (struct sockaddr *)&server_add, sizeof(server_add)) == -1) exit(1); // Failed to bind


    char* _ip = strdup(ip);
    if(_ip == NULL) exit(1);

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
        if (reqfd == -1) printf("accept error");
        printf("New Request\n");

        // Process request
        char buff[100]; // TODO : How to solve buffer size issue
        if (recv(reqfd, buff, sizeof(buff), 0) == -1) return log_err("error occured while reading request.");
        // Request structure
        // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
        // GET / HTTP/1.1
        // enum method method;
        char* method_raw = strtok(buff, " ");
        char* request_uri = strtok(NULL, " ");
        char* http_vers = strtok(NULL, "\r\n");
        printf("Received Request:\n%s %s %s\n", method_raw, request_uri, http_vers);

        // Getting Headers
        char *h;
        while(1) {
            h = strtok(NULL, "\r\n");
            if(h == NULL) break;
            printf("%s\n",h);
        }

        // Handling Requests
        req_handler hdlr = NULL;
        switch (get_method(method_raw)) {
            case GET:
                hdlr = *(req_handler*) array_get(s.handlers[GET], 0);
                break;
            default:
                hdlr = SAMPLE_FALLBACK_handler;
        }
        if(hdlr != NULL) hdlr(reqfd);


    }
};

void server_cleanup() { // TODO: how to do a signal triggered cleanup without relying on global var
    free(s.ip);
    for(int i =0; i < NO_METHODS; i++)
        array_free(s.handlers[i]);
}

// Adding Handlers

void get(char* path, req_handler hdlr) {
    array_push(&s.handlers[GET], hdlr);
}
void post(char* path, req_handler hdlr) {}
void update(char* path, req_handler hdlr) {}
void delete(char* path, req_handler hdlr) {}

void sigterm_handler(int);
int main() {

    signal(SIGINT, sigterm_handler);

    s = server_create("127.0.0.1", 8080);

    get("/", SAMPLE_GET_handler);

    server_listen(s);

    server_cleanup();
}

void sigterm_handler(int _) {
    printf("SIGTERM called\n");
    server_cleanup();
    exit(0);
};
