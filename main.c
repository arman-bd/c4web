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
    sprintf(sample_response, "<br>%s : %s : %s : %s", request.request_method, request.file_path, request.query, request.requested_uri);
    send_response_header(request, 200, "text/html", strlen(sample_response)+10);

    socket_send(request, sample_response);
    Sleep(1000);
    socket_send(request, "<br>0\n");
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    Sleep(2000);
    socket_send(request, "<br>1\n");
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    Sleep(3000);
    socket_send(request, "<br>2\n");
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    Sleep(4000);
    socket_send(request, "<br>3\n");
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    Sleep(5000);
    socket_send(request, "<br>4\n");
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
    socket_send(request, sample_response);
}
