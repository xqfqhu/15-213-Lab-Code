#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "csapp.h"

static int read_request(int fd, rio_t * client_rio, char * request_type, char * request_hostname, 
                        char * request_path, char * request_port){

    char buf[MAXLINE], uri[MAXLINE], version[MAXLINE];

    char * request_path_tmp = request_path;
    memcpy(request_path_tmp, "/", 1);
    request_path_tmp++;
    

    Rio_readinitb(client_rio, fd);

    /* read the request */
    if (!Rio_readlineb(client_rio, buf, MAXLINE)) return -1;

    sscanf(buf, "%s http://%s %s", request_type, uri, version); //parse request_type

    if (strcasecmp(request_type, "GET")) {                     

        return -1;
    }  

    if (strchr(uri, ':') != NULL){
        sscanf(uri, "%[^:]:%[^/]/%s", request_hostname, request_port, request_path_tmp); 
        // parse request_hostname, request_port, request_path if port is provided
    }
    else{
        sscanf(uri, "%[^/]/%s", request_hostname, request_path_tmp);
        memcpy(request_port, "80", 2); 
        // parse request_hostname, request_port, request_path if port is not provided
    }
    
    return 0;
}

static void forward_request(int fd, char * request_type, char * request_path){
    char buf[MAXLINE];
    sprintf(buf, "%s %s %s\r\n", request_type, request_path, "HTTP/1.0");
    Rio_writen(fd, buf, strlen(buf));
}

int main(){
    int fd;
    fd = open("test.txt", S_IRUSR);
    int fdw = open("test_out.txt", S_IWOTH);
    rio_t client_rio;
    char request_type[MAXLINE], request_hostname[MAXLINE], request_path[MAXLINE], request_port[MAXLINE];
    read_request(fd, &client_rio, request_type, request_hostname, 
                        request_path, request_port);
    forward_request(fdw, request_type, request_path);
    close(fd);
    close(fdw);
}