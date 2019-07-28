

int socket_send(http_request request, char *content){
    return send(request.socket, content, strlen(content), 0);
}

void http_response_create(http_request *request){
    struct http_response response = {};

    response.protocol = "HTTP/1.x";
    response.status_code = "200 OK";
    response.header_size = 0;
    response.header_sent = 0;
    response.headers = (http_headers*) malloc(sizeof(http_headers));

    request->response = response;
}


void http_response_header(http_request *request, char *key, char *value){
    int i = 0;
    int j = request->response.header_size;
    int found = 0;

    //printf("\n***HS_HRD %d ***\n", request->response.header_size);

    for(i = 0; i < j; i++){
        if(strcmp(request->response.headers[i].key, key) == 0){
            request->response.headers[i].value = malloc(sizeof(char) * (strlen(value) + 1));
            request->response.headers[i].value = value;
            found = 1;
            return;
        }
    }

    if(found == 0){
        request->response.headers = (http_headers*) realloc(request->response.headers, (j + 1) * sizeof(http_headers));

        request->response.headers[j].key = malloc(sizeof(char) * (strlen(key) + 1));
        strcpy(request->response.headers[j].key, key);

        request->response.headers[i].value = malloc(sizeof(char) * (strlen(value) + 1));
        strcpy(request->response.headers[i].value, value);

        request->response.header_size++;
    }
}


int http_header_send(http_request request){

    printf("\n>X 1 X<\n");

    if(request.response.header_sent == 1){
        // Error: Header Already Sent
        return -1;
    }

    int i = 0;
    int j = request.response.header_size;
    int headerSize = 128; // Basic
    char *header;

    printf("\n>X 2 X<\n");

    for(i = 0; i < j; i++){
        headerSize += strlen(request.response.headers[i].key) + strlen(request.response.headers[i].value) + 3; // 2 byte for [: ] + 1 byte for \n
    }

    header = malloc(headerSize * sizeof(char));

    // Add Response Code
    sprintf(header, "%s %s\n", request.response.protocol, request.response.status_code);

    // Add Server Info
    sprintf(header, "%sServer: %s/%s (%s)\n", header, __ServerName, __ServerVersion, __ServerOS);

    // Add Header Fields
    for(i = 0; i < j; i++){
        sprintf(header, "%s%s: %s\n", header, request.response.headers[i].key, request.response.headers[i].value);
    }

    // Add Trailing New Line
    sprintf(header, "%s\n", header);

    printf("\n------------------\n%s\n------------\n", header);

    // Send Header
    if(send(request.socket, header, strlen(header), 0)){
        request.response.header_sent = 1;
        return 1;
    }else{
        return 0;
    }
}


int file_send_compressed(FILE *source, SOCKET socket, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            // Send to Socket
            if (send(socket, (char*)out, have, 0) == -1) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }

        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    fclose(source);
    (void)deflateEnd(&strm);
    return Z_OK;
}

long file_send_uncompressed(FILE *fp, SOCKET socket){
    size_t file_size;
    long byteSent = 0;
    char *file_buff;

    file_size = getFileSize(fp);

    if(max_send_buffer == 0 || max_send_buffer > file_size){
        file_buff = malloc(file_size + 1);
        fread(file_buff, sizeof(char), file_size + 1, fp);
        byteSent = send(socket, file_buff, file_size, 0);
    }else{
        size_t c_position = 0;
        file_buff = malloc(max_send_buffer + 1);
        byteSent = 0;
        fseek(fp, 0L, SEEK_SET);

        while(c_position + max_send_buffer <= file_size){
            fseek(fp, c_position, SEEK_SET);
            fread(file_buff, sizeof(char), max_send_buffer + 1, fp);
            byteSent += send(socket, file_buff, max_send_buffer, 0);
            c_position = c_position + max_send_buffer;
        }
        size_t left_buffer = file_size - c_position;
        if(left_buffer > 0){
            fseek(fp, c_position, SEEK_SET);
            fread(file_buff, sizeof(char), left_buffer + 1, fp);
            if(left_buffer < max_send_buffer){
                file_buff[left_buffer] = '\0';
            }
            byteSent += send(socket, file_buff, left_buffer, 0);
        }
    }

    fclose(fp);
    free(file_buff);
    return byteSent;
}


int send_file(http_request request, char *file_path, int compression){
    FILE *fp;
    fp = fopen(file_path, "rb");
    if(compression == 1){
        http_response_header(&request, "Content-Encoding", "deflate");
    }

    if(request.response.header_sent == 0){
        http_header_send(request);
        Sleep(100);
    }

    if(compression == 1){
        file_send_compressed(fp, request.socket, Z_DEFAULT_COMPRESSION);
    }else{
        file_send_uncompressed(fp, request.socket);
    }



    return 0;
}







/*
int send_response_header(http_request request, int response, char *c_type){
    char header[4096];

    // Add Response Code
    if(response == 200){
        strcpy(header, "HTTP/1.1 200 OK\n");
    }

    if(response == 404){
        strcpy(header, "HTTP/1.1 404 Not Found\n");
    }

    // Process Header
    sprintf(header, "%sServer: %s/%s (%s)\n", header, __ServerName, __ServerVersion, __ServerOS);
    sprintf(header, "%sContent-Type: %s; charset=utf-8\n", header, c_type);
    sprintf(header, "%sAccess-Control-Allow-Origin: *\n", header);
    sprintf(header, "%sContent-Encoding: deflate\n", header);
    //sprintf(header, "%sContent-Length: %ld\n", header, c_size);
    sprintf(header, "%s\n", header);

    // Send Header
    return send(request.socket, header, strlen(header), 0);
}

int send_file(http_request request, char *file_path, int compression){
    FILE *fp;
    fp = fopen(file_path, "rb");
    if(compression == 1){
        file_send_compressed(fp, request.socket, Z_DEFAULT_COMPRESSION);
    }else{
        file_send_uncompressed(fp, request.socket);
    }

    return 0;
}
*/

/*
int send_response_file(SOCKET socket, FILE *fp, size_t file_size, size_t max_buffer){
    int byteSent = SOCKET_ERROR;
    char *file_buff;

    if(max_buffer == 0 || max_buffer > file_size){
        file_buff = malloc(file_size + 1);
        fread(file_buff, sizeof(char), file_size + 1, fp);
        byteSent = send(socket, file_buff, file_size, 0);
    }else{
        size_t c_position = 0;
        file_buff = malloc(max_buffer + 1);
        byteSent = 0;
        fseek(fp, 0L, SEEK_SET);

        while(c_position + max_buffer <= file_size){
            fseek(fp, c_position, SEEK_SET);
            fread(file_buff, sizeof(char), max_buffer + 1, fp);
            byteSent += send(socket, file_buff, max_buffer, 0);
            c_position = c_position + max_buffer;
        }
        size_t left_buffer = file_size - c_position;
        if(left_buffer > 0){
            fseek(fp, c_position, SEEK_SET);
            fread(file_buff, sizeof(char), left_buffer + 1, fp);
            if(left_buffer < max_buffer){
                file_buff[left_buffer] = '\0';
            }
            byteSent += send(socket, file_buff, left_buffer, 0);
        }
    }

    free(file_buff);
    return byteSent;
}

int send_response_error(SOCKET socket, int error_code, s_conf server_conf, int display_error){

    // Web Output
    FILE *fp;
    int file_size; // Error Handler File
    char file_name[128];
    char file_type[128] = "text/html";
    char file_path[512];
    char web_buffer[512];

    if(display_error == 1){
        // Content Not Found!
        printf("Error: File Not Found [ 0x02 ]\n");
    }

    // Try 404 Handler!
    sprintf(file_name, "\\error404.html");
    sprintf(file_path, "%s%s", server_conf.default_dir, file_name);

    // Get MIME Type
    // get_mime_type(file_name, file_type);


    if((fp = fopen(file_path, "rb")) != NULL){
        // Get File Size
        file_size = getFileSize(fp);
        // Pack Header
        send_response_header(socket, error_code, file_type, file_size);
        // Send File To Client
        send_response_file(socket, fp, file_size, server_conf.max_buffer);
        // Close File
        fclose(fp);
    }else{
        if(display_error == 1){
            printf("Error: Error Handler Not Found [ 0x03 ]\n");
        }

        if(error_code == 404){
            // 404 Handler Not Found!
            strcpy(web_buffer, "Error 404 page not found,<br>");
            strcat(web_buffer, "additional error occurred<br>");
            strcat(web_buffer, "while finding 404 error handler page!");
        }

        // Send Header
        if(send_response_header(socket, error_code, "text/html", strlen(web_buffer)) == SOCKET_ERROR){
            if(display_error == 1){
                printf("Error: Unable To Send [Header] To Client [ 0x04 ]\n");
            }
        }

        // Send Content
        if(send_response_content(socket, web_buffer) == SOCKET_ERROR){
            if(display_error == 1){
                printf("Error: Unable To Send [Body] To Client [ 0x05 ]\n");
            }
        }
    }

    return 1;
}
*/
