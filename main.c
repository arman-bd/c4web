#include<stdio.h>
#include "src/c4web.c"

int main(){

    start_server(7071);

    return 0;
}

void router(http_request request){
    router_match(request, "hello/world");
}
