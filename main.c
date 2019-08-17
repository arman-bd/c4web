#include<stdio.h>
#include "src/c4web.c"

void hello_world(http_request request);
void test_file(http_request request);

int main(){
    test_compress();
    start_server(7071);
    return 0;
}

void router(http_request request){
    if(router_match(request, "hello/world")){
        hello_world(request);
    }
    if(router_match(request, "testfile")){
        test_file(request);
    }
}

void test_file(http_request request){
    http_response_header_set(&request, "Content-Type", "text/*");
    http_response_header_set(&request, "Ding", "Dong");
    http_response_header_set(&request, "Tick", "Tock");
    http_response_header_set(&request, "Tick", "");
    http_send_file(request, "htdocs/LICENSE", 1);
}


void hello_world(http_request request){
    char sample_response[32] = "Hello World";
    http_send_text(request, sample_response);
}
