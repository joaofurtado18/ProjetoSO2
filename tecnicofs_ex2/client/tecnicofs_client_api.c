#include "tecnicofs_client_api.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
int fd_server, fd_client, id;

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {

    if (unlink(client_pipe_path) == -1 && errno == EEXIST){
        puts("client unlink error");
        return -1;
    }
    if (mkfifo(client_pipe_path, 0777) == -1){
        printf("mkfifo error\n");
        return -1;
    }
    if ((fd_server = open(server_pipe_path, O_WRONLY)) < 0){
        printf("error opening server: %s\n", strerror(errno));
        return -1;
    }
    char opc = '1';
    if (write(fd_server,  &opc, 1) < 0){
        printf("error writing opcode: %d\n");
        return -1;
    }
    if (write(fd_server, client_pipe_path, 40) < 0){
        printf("error writing client_pipe_path: %d\n");
        return -1;
    }

    if ((fd_client = open(client_pipe_path, O_RDONLY)) < 0){
        printf("error opening client: %s\n", strerror(errno));
        return -1;
    }

    int bytes_read;
    while (1){
        if ((bytes_read = read(fd_client, &id, sizeof(id))) == -1){
            printf("error reading id\n");
            break;
        } else if (bytes_read == 0){ continue; }
        else
            break;
    }
    printf("session id: %d\n", id);
    return 0;
}

int tfs_unmount() {
    /* TODO: Implement this */
    return -1;
}

int tfs_open(char const *name, int flags) {
    /* TODO: Implement this */
    char opc = '3';
    int ret_value, bytes_read;
    write(fd_server, &opc, 1);
    write(fd_server, &id, sizeof(id));
    write(fd_server, name, 40);
    write(fd_server, &flags, sizeof(flags));


    while (1){
        if ((bytes_read = read(fd_client, &ret_value, sizeof(ret_value))) == -1){
            printf("error reading id\n");
            break;
        } else if (bytes_read == 0){ continue; }
        else
            break;
    }
    

    return ret_value;
}

int tfs_close(int fhandle) {
    /* TODO: Implement this */
    return -1;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {
    /* TODO: Implement this */
    return -1;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    /* TODO: Implement this */
    return -1;
}

int tfs_shutdown_after_all_closed() {
    /* TODO: Implement this */
    return -1;
}
