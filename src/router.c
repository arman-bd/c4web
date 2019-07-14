
int router_match(http_request request, char *path){

    sds request_path = sdsnew(request.file_path);
    sdstrim(request_path, "/");

    sds router_path = sdsnew(path);
    sdstrim(request_path, "/");

    if(strcmp(request_path, router_path) == 0){
        return 1;
    }else{
        return 0;
    }

    return 0;
}
