#include<stdio.h>
#include "src/c4web.c"

void web_hello_world(http_request request);
void test_file(http_request request);

int main(){
    start_server(7071);
    return 0;
}

void router(http_request request){
    if(router_match(request, "hello/world")){
        //web_hello_world(request);
    }
    if(router_match(request, "testfile")){
        test_file(request);
    }
}

void test_file(http_request request){


    http_response_header(&request, "Content-Type", "text/*");
    http_response_header(&request, "Ding", "Dong");
    http_response_header(&request, "Tick", "Tock");
    send_file(request, "htdocs/LICENSE_x.html", 1);
    //Sleep(1000);

    //http_header_send(request);

    //send_response_header(request, 200, "text/plain");

}

/*
void web_hello_world(http_request request){
    char sample_response[1000];
    sprintf(sample_response, "<br>%s : %s : %s : %s", request.request_method, request.file_path, request.query, request.requested_uri);
    send_response_header(request, 200, "text/html");

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
*/
