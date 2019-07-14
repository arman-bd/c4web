#include<stdio.h>

typedef struct header_parameter {
    char *key;
    char *value;
} header_parameter;

typedef struct http_response {
    int header_size;
    header_parameter *headers;
    char method[16];
    char *body;
} http_response;

int main(){
    char *data = "Test: Data";
    http_response response;
    response.header_size = 0;

    int header_length = strlen(data);
    response.headers = malloc(sizeof(response.headers)+1);
    response.headers[0].key = "Test";
    response.headers[0].value = "Data";

    printf("%d : %d\n\n%s", header_length, 0, response.headers[0].value);

    return 0;
}
