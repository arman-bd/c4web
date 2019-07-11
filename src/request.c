/*
07-06-2017:
Adding SDS Library to Parse Headers
*/


typedef struct rqpack {
    // Request Parameter
    char request_method[9];
    int compressed;

    // File Parameter
    char *requested_uri;
    char *file_name;
    char *file_path;
    char *query;
    char *host;
}rqpack;

typedef struct http_request {
    // Request Parameter
    int status;
    char request_method[9];
    int compressed;

    // File Parameter
    char requested_uri[4096];
    char file_name[1024];
    char file_path[2048];
    char query[4096];
    char host[1024];
} http_request;

http_request process_request(int AcceptSocket){

    int i;
    int bytesRecv = -1;
    http_request http_req;

    char *buffer_data;

    // SDS Library
    int  header_count, param_count, query_count;
    sds header_sds, param_sds, *header_token, *param_token;
    sds query_sds, *query_token;

    buffer_data = malloc(max_receive_buffer + 1);

    // Receive Data From Client
    bytesRecv = recv(AcceptSocket, buffer_data, max_receive_buffer, 0);
    //printf(">>> %s <<<", buffer_data);

    if(bytesRecv < 0){
        printf("Error: Data Receive Error [ 0x01 ]\n");
        http_req.status = -1;
        return http_req;
    }else{
        if(bytesRecv == 0){ // PING Request
            // Close Connection : Repeat Process
            http_req.status = -1;
            return http_req;
        }

        // Process Primary Buffer for Hint to Next

        header_sds = sdsnew(buffer_data);
        header_token = sdssplitlen(header_sds, sdslen(header_sds), "\n", 1, &header_count);

        for (i = 0; i < header_count; i++){

            if(strcmp(header_token[i], "") == 0){
                // End of Header
                sdsfreesplitres(header_token, header_count);
                break;
            }

            param_sds = sdsnew(buffer_data);



            if(i == 0){
                // First Line [ GET / POST ]
                param_token = sdssplitlen(param_sds, sdslen(param_sds), " ", 1, &param_count);

                // Process Request Method [ GET, POST, PUT, DELETE ]
                if(strcmp(param_token[0], "GET") == 0 || strcmp(param_token[0], "POST") == 0 ||
                   strcmp(param_token[0], "PUT") == 0 || strcmp(param_token[0], "DELETE") == 0){

                    strcpy(http_req.request_method, param_token[0]);
                    //printf(">> %s\n", http_req.request_method);

                    query_sds = sdsnew(param_token[1]);
                    query_token = sdssplitlen(query_sds, sdslen(query_sds), "?", 1, &query_count);

                    if(query_count > 1){
                        strcpy(http_req.file_path, query_token[0]);
                        strcpy(http_req.query, query_token[1]);
                    }else{
                        strcpy(http_req.file_path, query_token[0]);
                        //http_req.query[0] = '\0';
                    }

                }else{
                    // Return Error: Invalid Request Method
                    printf("Error: Invalid Request Method [ 0x02 ]\n");
                    http_req.status = -1;
                    return http_req;
                }

                // Process Request URI
                if(strcmp(param_token[1], "") != 0){
                    strcpy(http_req.requested_uri, param_token[1]);
                    //process_file_path(param_token[1]);
                }
            }else{
                break;
                // Other Headers :: Must Process As Much As Possible!
                param_token = sdssplitlen(param_sds, sdslen(param_sds), ": ", 1, &param_count);
            }

            sdsfreesplitres(param_token, param_count);
        }

        // Free SDS
        sdsfree(header_sds);
        sdsfree(param_sds);
        sdsfree(query_sds);
        free(buffer_data);

        // Display Info About Requested File

        printf("\n------------------------------------------\n");
        printf("> Requested Method: %s\n", http_req.request_method);
        printf("> Requested File: %s\n", http_req.file_path);
        printf("> Requested Query: %s\n", http_req.query);
        printf("> Requested URI: %s\n\n", http_req.requested_uri);
    }


    http_req.status = 1;
    return http_req;
}
