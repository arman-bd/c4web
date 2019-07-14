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

typedef struct ThreadData {
    int thread_id;
    SOCKET mysocket;
} ThreadData;

// Include Required File
#include "sds.c"
#include "function.c"
#include "request.c"
#include "response.c"
#include "router.c"

// Functions
DWORD WINAPI HandleRequest(void* data);
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
    //int bytesRecv = SOCKET_ERROR;
    SOCKET MainSocket;
    SOCKET AcceptSocket;

    // File
    //FILE *fp;
    //char *file_type = malloc(128);
    //size_t file_size = 0;

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

    int req_id = 0;
    struct ThreadData tData;

    // Listen For Request
    listen_for_client:

    while(1){
        AcceptSocket = SOCKET_ERROR;

        while(AcceptSocket == SOCKET_ERROR){
            AcceptSocket = accept(MainSocket, NULL, NULL);
        }

        break;
    }

    tData.thread_id = req_id++;
    tData.mysocket = AcceptSocket;
    CreateThread(NULL, 0, &HandleRequest, &tData, 0, NULL);

    goto listen_for_client; // Repeat Process

    WSACleanup();
    return 0;
}

#define start_server(...) __start_server((ip_port){__VA_ARGS__});




DWORD WINAPI HandleRequest(void* data) {

    struct ThreadData *tData = (struct ThreadData *) data;

    printf("\nThread ID: %d", tData->thread_id);

    // Receive Data From Client
    http_request this_request;
    this_request.socket = tData->mysocket;
    process_request(&this_request);

    //this_request = process_request(this_request.socket);


    if(this_request.status == -1){
        // Error!
        closesocket(this_request.socket); // Close Socket
        return 0;
    }else{
        // Forward To Router
        router(this_request);


        /*
        char sample_response[1000];
        sprintf(sample_response, "%s : %s : %s : %s", this_request.request_method, this_request.file_path, this_request.query, this_request.requested_uri);
        send_response_header(AcceptSocket, 200, "text/html", strlen(sample_response)+10);

        send_response_content(AcceptSocket, sample_response);
        Sleep(1000);
        send_response_content(AcceptSocket, "0\n");
        Sleep(1000);
        send_response_content(AcceptSocket, "1\n");
        Sleep(1000);
        send_response_content(AcceptSocket, "2\n");
        Sleep(1000);
        send_response_content(AcceptSocket, "3\n");
        Sleep(1000);
        send_response_content(AcceptSocket, "4\n");
        */



    }

    /*
    if((fp = fopen(request_data.file_path, "rb")) != NULL){
        // File Found, Now Get Some Information About File And Send It
        get_mime_type(request_data.file_name, file_type); // Get MIME Type
        file_size = getFileSize(fp); // Get File Size
        send_response_header(AcceptSocket, 200, file_type, file_size); // Send Header
        send_response_file(AcceptSocket, fp, file_size, server_conf.max_buffer); // Send File To Client
        fclose(fp); // Close File
    }else{
        send_response_error(AcceptSocket, 404, server_conf, 0); // No Debug Print
    }
    */



    closesocket(this_request.socket); // Close Connection

    return 0;
}
