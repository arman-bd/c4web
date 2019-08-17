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
    int closesocket(int __fd){
        return close(__fd);
    }

    size_t __send(SOCKET __socket, char * __buf, size_t __n, int __flags){
        return (size_t) send((int) __socket, (const void *) __buf, (size_t) __n, (int) __flags);
    }

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

#if defined(MSDOS) || defined(WIN32) || defined(__CYGWIN__)
int __start_server(ip_port in) {
    // IP : Port
    port = in.port ? in.port : 8080;
    dir = in.dir ? in.dir : "htdocs";
    ip = in.ip ? in.ip : "127.0.0.1";
    max_send_buffer = in.max_send_buffer ? in.max_send_buffer : 10240;
    max_receive_buffer = in.max_receive_buffer ? in.max_receive_buffer : 10240;


    printf("%s:%d\n\n", ip, port);

    // Socket
    SOCKET MainSocket;

    // Print Application Name
    printf("::: %s v%s - %s :::\n\n", __ServerName, __ServerVersion, __ServerOS);

    /* Start Socket Initialization: Windows */
    WORD versionRequested;
    WSADATA wsaData;
    int wsaError;
    versionRequested = MAKEWORD(2, 2);

    wsaError = WSAStartup(versionRequested, &wsaData);
    if(wsaError != 0){
        printf("Error: Winsock Dll Not Found!\n");
        return 0;
    }else{
        printf("Server: Winsock Found.\n");
    }

    // Check Winsock DLL Version : 2.2
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2 ){
        printf("Error: Unsupported Winsock Dll Version - v%u.%u!\n", LOBYTE(wsaData.wVersion), HIBYTE(wsaData.wVersion));
        WSACleanup();
        return 0;
    }

    // Define Socket Preference
    MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check Socket Validity
    if(MainSocket == INVALID_SOCKET){
        printf("Error: Socket Error - %d\n", WSAGetLastError());
        WSACleanup();
        return 0;
    }else{
        printf("Server: Socket Loaded\n");
    }


    /* Start Binding */
    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);

    // Bind Socket
    if (bind(MainSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR){
        printf("Error: Binding Failed - %d.\n", WSAGetLastError());
        printf("       Check If Port Is Already In Use.\n");
        closesocket(MainSocket);
        return 0;
    }else{
        printf("Server: Binded Successfully\n");
        printf("Server: IP [ %s ]\n", ip);
    }

    if(listen(MainSocket, 10) == SOCKET_ERROR){
       printf("Error: Unable To Listen On Port [ %d ] - %d.\n", port, WSAGetLastError());
    }else{
       printf("Server: Listening On Port [ %d ]\n", port);
    }

    printf("\n");

    //int req_id = 0;
    struct ThreadData tData;

    // Start Thread Handler
    CreateThread(NULL, 0, &HandleThread, NULL, 0, NULL);

    // Listener Socket handler
    tData.socket = MainSocket;
    CreateThread(NULL, 0, &HandleListening, &tData, 0, NULL);

    while(1){
        Sleep(1000);
        //printf("\n>Active Thread: %d", THREAD_ACTIVE);
    }

    //WSACleanup();
    return 0;
}
#else
int __start_server(ip_port in) {
    // IP : Port
    port = in.port ? in.port : 8080;
    dir = in.dir ? in.dir : "htdocs";
    ip = in.ip ? in.ip : "127.0.0.1";
    max_send_buffer = in.max_send_buffer ? in.max_send_buffer : 10240;
    max_receive_buffer = in.max_receive_buffer ? in.max_receive_buffer : 10240;

    // Socket
    SOCKET MainSocket;
    struct sockaddr_in service;

    // Print Application Name
    printf("::: %s v%s - %s :::\n\n", __ServerName, __ServerVersion, __ServerOS);

new_port:
    // Define Socket Preference
    MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Check Socket Validity
    if(MainSocket == INVALID_SOCKET){
        printf("Error: Socket Error\n");
        return 0;
    }else{
        printf("Server: Socket Loaded\n");
    }

    /* Start Binding */
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr(ip);
    service.sin_port = htons(port);

    // Bind Socket
    if (bind(MainSocket, (struct sockaddr *)&service, sizeof(service)) < 0){
        printf("Error: Binding Failed.\n");
        printf("       Check If Port Is Already In Use.\n");
        closesocket(MainSocket);
        port++;
        goto new_port;
        return 0;
    }else{
        printf("Server: Binded Successfully\n");
        printf("Server: IP [ %s ]\n", ip);
    }

    if(listen(MainSocket, 10) == SOCKET_ERROR){
       printf("Error: Unable To Listen On Port [ %d ].\n", port);
    }else{
       printf("Server: Listening On Port [ %d ]\n", port);
    }

    printf("\n");

    //int req_id = 0;
    struct ThreadData tData;
    THREAD_T t_handler, t_listener;


    // Start Thread Handler
    pthread_create(&t_handler, NULL, (void*)&HandleThread, NULL);

    // Listener Socket handler
    tData.socket = MainSocket;
    pthread_create(&t_listener, NULL, (void*)&HandleListening, &tData);
    pthread_join(t_listener, NULL);

    while(1){
        sleep_ms(1000);
        //printf("\n>Active Thread: %d", THREAD_ACTIVE);
    }

    return 0;
}
#endif

DWORD OSAPI HandleListening (void* data) {

    struct ThreadData *tData = (struct ThreadData *) data;
    struct ThreadData xData;
    THREAD_T thread_id;

    SOCKET AcceptSocket = SOCKET_ERROR;

    while(AcceptSocket == SOCKET_ERROR){
        AcceptSocket = accept(tData->socket, NULL, NULL);

        if(AcceptSocket != SOCKET_ERROR){
            xData.socket = AcceptSocket;
#if defined(MSDOS) || defined(WIN32) || defined(__CYGWIN__)
            CreateThread(NULL, 0, &HandleRequest, &xData, 0, NULL);
#else
            pthread_create(&thread_id, NULL, (void*)&HandleRequest, &xData);
            pthread_join(thread_id, NULL);
#endif
            AcceptSocket = SOCKET_ERROR;
        }

        if(THREAD_ACTIVE == 0){sleep_ms(10);}
    }

    return 0;
}



DWORD OSAPI HandleRequest(void* data) {

    struct ThreadData *tData = (struct ThreadData *) data;
    THREAD_T thread_id;

    #if defined(MSDOS) || defined(WIN32) || defined(__CYGWIN__)
        thread_id = GetCurrentThread();
    #else
        thread_id = pthread_self();
    #endif

    // Register Thread into Thread Pool
    RegisterThread(thread_id, tData->socket);

    //printf("\n>Active Thread: %d", THREAD_ACTIVE);
    //printf("\n>Thread ID: %lu", thread_id);

    // Receive Data From Client
    struct http_request this_request;
    this_request.socket = tData->socket;
    http_response_create(&this_request);

    process_request(&this_request);

    if(this_request.status == -1){
        // Error!
        closesocket(this_request.socket); // Close Socket
        return 0;
    }else{
        // Forward To Router
        router(this_request);
    }

    closesocket(this_request.socket); // Close Connection

    // Unregister Thread from Thread Pool
    UnregisterThread(thread_id, this_request.socket);


    return 0;
}
