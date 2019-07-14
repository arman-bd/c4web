
int router_match(http_request request, char *path){

    /*
    Type 1: /path/to/convert/
    Type 2: path/to/convert/
    Type 3: path/to/convert
    Type 4: /path/to/convert
    */

    sds *tokens;
    int count, j;

    sds line = sdsnew(request.file_path);
    tokens = sdssplitlen(line,sdslen(line),"/",1,&count);

    for (j = 0; j < count; j++)
        printf("SDS: %s\n", tokens[j]);

    sdsfreesplitres(tokens,count);


    return 0;
}
