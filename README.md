# C4Web
C for Web is a simple framework for developing Web Applications using C.

Note: This project is currently under basic development phase.

![C4Web Framework](https://i.imgur.com/vIi6GHU.png "C4Web Framework")

## A Simple Hello World Project in C4Web

```C
#include "src/c4web.c"

void hello_world(http_request request);

int main(){
    start_server(7071);
    return 0;
}

void router(http_request request){
    if(router_match(request, "hello-world")){
        hello_world(request);
    }
}

void hello_world(http_request request){
    char sample_response[12] = "Hello World";
    http_send_text(request, sample_response);
}
```

At this moment, the executable produced in this "hello world" project is roughly ~40 KB ( on Unix ).
Yes, this ~40 KB executable contains the features mentioned below.

## Features

* Multi Threaded
* Very Low Dependency
* Web-Routing Supported
* Compression Supported ( Zlib )
* Rich HTTP API
* Custom Header
* Works on both Unix and Windows

## Upcoming

* E-Tag for Files
* PCRE ( Regular Expression )
* GET / POST / Query Processing
