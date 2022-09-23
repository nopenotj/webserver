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
#include "dict/dict.h"


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
    char* http_vers;
    enum method method;
    struct dict headers;
    char* body;
    char* uri;
};
struct http_request parse_http_request(int reqfd) {
        #define RECV_BUFF_SIZE 100
        char buff[RECV_BUFF_SIZE] = {0};
        int bytes_recv;

        char* raw = NULL;
        size_t raw_sz = 0;

	// get all the raw bytes 
        do {
            // Keep recv-ing and pushing into raw
            bytes_recv = recv(reqfd, buff, sizeof(buff), 0);

	    // if err while recv-ing just exit
	    if(bytes_recv == -1) {
		log_err("Error occured while reading request.");
		return (struct http_request){.method = UNKNOWN};
	    }

            log_debug("[sockid %d] Filling raw with buffer (size: %d bytes)\n", reqfd, bytes_recv);

            raw = realloc(raw, raw_sz + bytes_recv);
            strncpy(raw + raw_sz, buff, bytes_recv);
            raw_sz += bytes_recv;
        } while (bytes_recv == RECV_BUFF_SIZE && buff[RECV_BUFF_SIZE - 1] != '\0');


        // Request structure
        // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
        // GET / HTTP/1.1
        // enum method method;
	char* h;
        char* method_raw = strtok_r(raw, " ",&h);
        char* request_uri = strtok_r(NULL, " ",&h);
        char* http_vers = strtok_r(NULL, "\r\n",&h);
        log_debug("Received Request: %s %s %s\n", method_raw, request_uri, http_vers);

        // Parsing Headers
	// TODO: all my personal datastructures assume data is initialized to 0
        struct dict headers = {0};
	headers.keys.esize = sizeof(char*);

	// Go through raw string header by header(deliminated by \r\n
	char* e;
        while(1) {
	    int has_colon = 0;
	    e = h;
	    while (*e != '\r') {
		if(*e == ':') has_colon = 1;
		e++;
	    }
	    if(*++e != '\n') print_exit("HEADER : newline not found after carraige return");
	    // change \r\n to \0\0
	    *(e-1) = '\0';
	    *e = '\0';

	    // break if we found \r\n without anyheader => this marks the end of the headers
            if(!has_colon||h == NULL) break;

            char* s = strdup(h), *p;
	    char* key = strtok_r(s, ": ", &p);
	    char* val = strtok_r(NULL, ": ", &p);
            dict_put(&headers, key, val);
	    h = ++e;
        }

	// Get body after header if content length is >0
	char* body = NULL;
	if(atoi(dict_get(&headers, "Content-Length", char*)) > 0) body = strdup(++e);

	// Clean up
	free(raw);
	// Getting Payload
	return (struct http_request){ 
		.sockfd=reqfd,
		.http_vers=http_vers,
		.method = GET, 
		.headers=headers, 
		.body=body,
		.uri=request_uri
	};

}
void print_http_request(struct http_request req) {
    printf("method(ENUM) : %d \n", req.method);
    printf("sockfd       : %d \n", req.sockfd);
    printf("http_vers    : %s \n", req.http_vers);
    printf("body         : %s \n", req.body);
    printf("uri          : %s \n", req.uri);
    printf("Headers      :\n");
    int i = 0; char* key;
    while( i < req.headers.keys.len ) {
	key = array_get(req.headers.keys, i, char*);
	printf("%i : %s\n",i,key);
	i++;
    }
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

	print_http_request(req);

        // Handling Requests
        req_handler hdlr = NULL;
        switch (req.method) {
            case GET:
                hdlr = array_get(s.handlers[GET], 0, req_handler);
                break;
            default:
                hdlr = SAMPLE_FALLBACK_handler;
        }
        if(hdlr != NULL) hdlr(reqfd);


        // Clean up request
        close(reqfd);
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

