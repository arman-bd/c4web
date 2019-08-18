/*
Project: C4Web Framework

By: Arman Hossain
CSE 11'th Batch,
Shanto-Mariam University of Creative Technology
Dhaka, Bangladesh

GitHub: https://github.com/arman-bd/c4web
Portfolio: https://arman-bd.github.com/
*/

char __ServerName[128] = "C4Web";
char __ServerVersion[16] = "0.1.2";

// Include Required Headers
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<error.h>

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
    // Windows Libraries
    #include<io.h>
    #include<windows.h>
    #include<winsock2.h>

    typedef HANDLE THREAD_T;

    char __ServerOS[32] = "Win32";

    typedef OSAPI WINAPI; // This should replace Windows specific WINAPI

    int __send(SOCKET __socket, const void *__buf, size_t __n, int __flags){
        return send((SOCKET) __socket, (const char *) __buf, (int) __n, (int) __flags);
    }

#else
    // Linux Libraries
    #include<unistd.h>
    #include<resolv.h>
    #include<pthread.h>
    #include<fcntl.h>
    #include<arpa/inet.h>
    #include<netinet/in.h>
    #include<sys/socket.h>
    #include<sys/wait.h>

    char __ServerOS[32] = "Linux";

    typedef unsigned short ATOM;
    typedef ATOM WORD;
    typedef unsigned long DWORD;
    typedef int SOCKET;
    typedef int HANDLE;
    typedef pthread_t THREAD_T;

    #define INVALID_SOCKET ((SOCKET)(~0))
    #define SOCKET_ERROR (-1)
    #define OSAPI // This should replace Windows specific WINAPI
    //#define closesocket close
    int closesocket(int __fd);
    size_t __send(SOCKET __socket, char * __buf, size_t __n, int __flags);

#endif

#define COMPRESSED 1
#define UNCOMPRESSED 0

// Include Libraries
#include "lib/sds-2.0.0/sds.c"
#include "lib/zlib-1.2.3/zlib.h"

// Include Required File
#include "http.h"

#include "function.c"
#include "compress.c"
#include "request.c"
#include "response.c"
#include "router.c"
#include "thread.c"

// Functions
DWORD OSAPI HandleRequest(void* socket);
DWORD OSAPI HandleListening (void* socket);
void router(http_request request);
int __start_server(ip_port in);

#define start_server(...) __start_server((ip_port){__VA_ARGS__});

#include "c4web.c"
