typedef unsigned long long u_buffer;
u_buffer max_send_buffer;
u_buffer max_receive_buffer;
int port;
char *dir;
char *ip;

typedef struct {
    int port;
    char *dir;
    char *ip;
    u_buffer max_send_buffer;
    u_buffer max_receive_buffer;
} ip_port;

typedef struct http_headers {
    char *key;
    char *value;
} http_headers;

typedef struct http_response {
    char *protocol;
    char *status_code;
    int header_sent;
    int header_size;
    struct http_headers *headers;
    char *body;
} http_response;

typedef struct http_request {
    // Request Parameter
    SOCKET socket;
    int status;
    char request_method[9];
    int compressed;

    // File Parameter
    char requested_uri[4096];
    char file_name[1024];
    char file_path[2048];
    char query[4096];
    char host[256];
    struct http_response response;
} http_request;


#define HTTP1X "HTTP/1.x"
#define HTTP200 "200 OK"
