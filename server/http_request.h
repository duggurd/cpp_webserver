#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <string>
#include <cstring>

struct request {
    char* type;
    char* req_content;
    char* payload;
    char* headers;
};

struct http_response {
    char* http = "HTTP/1.1";
    char* response_code;
    char* location;
    char* content_type;
    int content_length;
    char* body;
};

void process_request(char *req_str, request &request) {
    char* ptr = req_str;
    request.type = strtok(ptr, " ");
    ptr = strtok(NULL, " ");
    request.req_content = strtok(ptr, " ");
    ptr = strtok(NULL, " ");
    request.headers = strtok(ptr, " ");
}

void insert(char * s, char* insert, int start, int end) {
    for (int i = start, n = 0; i < end; i++, n++) {
        s[i] = insert[n];
    }
}



#endif