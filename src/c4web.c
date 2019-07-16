/*
Project: Khudro Web-Server

By: Arman Hossain
CSE 11'th Batch,
Shanto-Mariam University of Creative Technology
Dhaka, Bangladesh

GitHub: https://github.com/arman-bd/c4web
*/

char __ServerName[128] = "Khudro";
char __ServerVersion[16] = "1.0.1";
char __ServerOS[32] = "Win32";

// Include Required Headers
#include<io.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<sys/stat.h>
#include<sys/types.h>

// Windows Library
#include<windows.h>
#include<winsock2.h>

typedef unsigned long long u_buffer;
u_buffer max_send_buffer;
u_buffer max_receive_buffer;
int port;
char *dir;
char *ip;

typedef struct {
    int port;
    char *dir;
    char *ip;
    u_buffer max_send_buffer;
    u_buffer max_receive_buffer;
} ip_port;

// Include Required File
#include "sds.c"
#include "function.c"
#include "request.c"
#include "response.c"
#include "router.c"
#include "thread.c"

// Functions
DWORD WINAPI HandleRequest(void* socket);
DWORD WINAPI HandleListening (void* socket);
void router(http_request request);

int __start_server(ip_port in) {
    // IP : Port
    port = in.port ? in.port : 8080;
    dir = in.dir ? in.dir : "htdocs";
    ip = in.ip ? in.ip : "127.0.0.1";
    max_send_buffer = in.max_send_buffer ? in.max_send_buffer : 10240;
    max_receive_buffer = in.max_receive_buffer ? in.max_receive_buffer : 10240;


    printf("%s:%d\n\n", ip, port);

    // Socket
    WORD versionRequested;
    WSADATA wsaData;
    int wsaError;
    SOCKET MainSocket;
    //SOCKET AcceptSocket;

    // Print Application Name
    printf("::: %s v%s - %s :::\n\n", __ServerName, __ServerVersion, __ServerOS);

    /* Start Socket Initialization */
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
        printf("\n>Active Thread: %d", THREAD_ACTIVE);
    }

    /*
    // Listen For Request
    listen_for_client:

    AcceptSocket = SOCKET_ERROR;
    while(AcceptSocket == SOCKET_ERROR){
        AcceptSocket = accept(MainSocket, NULL, NULL);
        // Rest While Low Request
        //if(THREAD_ACTIVE == 0){Sleep(10);}
    }

    tData.thread_number = req_id++;
    tData.mysocket = AcceptSocket;
    CreateThread(NULL, 0, &HandleRequest, &tData, 0, NULL);

    goto listen_for_client; // Repeat Process
    */

    //WSACleanup();
    return 0;
}

DWORD WINAPI HandleListening (void* data) {

    struct ThreadData *tData = (struct ThreadData *) data;
    struct ThreadData xData;

    SOCKET AcceptSocket = SOCKET_ERROR;

    while(AcceptSocket == SOCKET_ERROR){
        AcceptSocket = accept(tData->socket, NULL, NULL);

        if(AcceptSocket != SOCKET_ERROR){
            xData.socket = AcceptSocket;
            CreateThread(NULL, 0, &HandleRequest, &xData, 0, NULL);
            AcceptSocket = SOCKET_ERROR;
        }

        if(THREAD_ACTIVE == 0){Sleep(10);}
    }

    return 0;
}


#define start_server(...) __start_server((ip_port){__VA_ARGS__});

DWORD WINAPI HandleRequest(void* data) {

    struct ThreadData *tData = (struct ThreadData *) data;

    // Register Thread into Thread Pool
    RegisterThread(GetCurrentThread(), tData->socket);

    printf("\n>Active Thread: %d", THREAD_ACTIVE);

    // Receive Data From Client
    http_request this_request;
    this_request.socket = tData->socket;
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
    UnregisterThread(GetCurrentThread(), this_request.socket);

    return 0;
}
