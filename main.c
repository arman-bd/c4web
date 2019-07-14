#include<stdio.h>
#include "src/c4web.c"

void web_hello_world(http_request request);

int main(){
    start_server(7071);
    return 0;
}

void router(http_request request){
    if(router_match(request, "hello/world")){
        web_hello_world(request);
    }
}

void web_hello_world(http_request request){
    char sample_response[1000];
    sprintf(sample_response, "%s : %s : %s : %s", request.request_method, request.file_path, request.query, request.requested_uri);
    send_response_header(request, 200, "text/html", strlen(sample_response)+10);

    socket_send(request, sample_response);
    Sleep(1000);
    socket_send(request, "0\n");
    Sleep(2000);
    socket_send(request, "1\n");
    Sleep(3000);
    socket_send(request, "2\n");
    Sleep(4000);
    socket_send(request, "3\n");
    Sleep(5000);
    socket_send(request, "4\n");
}
