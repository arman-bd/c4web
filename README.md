# C4Web

C for Web is a simple framework for developing Web Applications using C.

`
Note: Project is at initial development phase, 
code / methods might change without notice.
Please use with caution!
`

<img src="https://i.imgur.com/vIi6GHU.png" width="500" />

## 1. Overview

* Multi Threaded
* Very Low Dependency
* Web-Routing Supported
* Compression Supported ( Zlib )
* Rich HTTP API
* Custom Header
* Works on both Unix and Windows

## 2. Usage

### 2.1. Generating Project

First download or clone this repository in your development environment.
Use `cmake` to generate project on your preffered IDE.
We recommend using `Code::Blocks`

Open `terminal` inside the directory or use `cd` to navigate.

```bash
mkdir cb_project
cd cb_project
cmake -G"CodeBlocks - Unix Makefiles" ..
```

For other IDE please refer to this [guide](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).
Alternatively you can refer to this [Issue Page](https://github.com/district10/cmake-templates/issues/2#issue-154178247).

### 2.2. Generating Builds

If you want to skip generating project files,
you may use `cmake` to generate build files.
For that use the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

This will create an executable file in your `build` folder.
If you are on Unix system, you may run it by 

```bash
./c4web
```

CMake will by default build Debug builds.
If you want Release or Minified Release builds
please use the following commands.

```bash
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
make clean
make
```

## 3. Examples

### 3.1. Hello World

```C
#include "framework/c4web.h"

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


## Upcoming

* E-Tag for Files
* PCRE ( Regular Expression )
* GET / POST / Query Processing


## Copyright

* C4Web is licensed under the ISC license.

Copyright © 2019 Arman Hossain

Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.