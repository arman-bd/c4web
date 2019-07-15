#include<stdio.h>

typedef struct http_headers {
    char *key;
    char *value;
} http_headers;

typedef struct http_response {
    char *protocol;
    char *status_code;
    int header_sent;
    int header_size;
    http_headers *headers;
    char *body;
} http_response;

http_response http_response_create(){
    http_response response;
    response.protocol = "HTTP/1.x";
    response.status_code = "200 OK";
    response.header_size = 0;
    response.header_sent = 0;
    return response;
}

void http_response_header(http_response *response, char *key, char *value){
    int i = 0;
    int j = response->header_size;

    for(i = 0; i < j; i++){
        if(strcmp(response->headers[i].key, key) == 0){
            response->headers[i].value = value;
            return;
        }
    }

    http_headers *new_headers;
    new_headers = (http_headers*) realloc(response->headers, (j + 1) * sizeof(response->headers));

    new_headers[j].key = key;
    new_headers[j].value = value;
    response.header_size++;

    response->headers = new_headers;
    free(new_headers);

}

int main(){

    http_response response = http_response_create();
    http_response_header(&response, "Hello", "World");

    return 0;
}
