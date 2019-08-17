/*
Project: C4Web Framework

By: Arman Hossain
CSE 11'th Batch,
Shanto-Mariam University of Creative Technology
Dhaka, Bangladesh

GitHub: https://github.com/arman-bd/c4web
Portfolio: https://arman-bd.github.com/
*/

#include "src/c4web.c"

void web_index(http_request request);
void hello_world(http_request request);

int main(){
    test_compress();
    start_server(7071);
    return 0;
}

void router(http_request request){
    if(router_match(request, "index")){
        web_index(request);
    }

    if(router_match(request, "hello/world")){
        hello_world(request);
    }
}

void web_index(http_request request){
    http_response_header_set(&request, "Content-Type", "text/html");
    http_send_file(request, "htdocs/index.html", 1);
}

void hello_world(http_request request){
    char sample_response[32] = "Hello World";
    http_send_text(request, sample_response);
}

/*
void test_file(http_request request){
    http_response_header_set(&request, "Content-Type", "text/*");
    http_response_header_set(&request, "Ding", "Dong");
    http_response_header_set(&request, "Tick", "Tock");
    http_response_header_set(&request, "Tick", "");
    http_send_file(request, "htdocs/LICENSE", 1);
}
*/
