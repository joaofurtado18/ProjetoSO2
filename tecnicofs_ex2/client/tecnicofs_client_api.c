#include "tecnicofs_client_api.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {
    int fd_server, fd_client;

    if (mkfifo(client_pipe_path, 0777) == -1){
        if (errno != EEXIST){
            return -1;
        }
    }
    if ((fd_server = open(server_pipe_path, O_WRONLY)) < 0){
        printf("error opening server: %s\n", strerror(errno));
        return -1;
    }
    char opc = '1';
    write(fd_server,  &opc, 1);
    write(fd_server, client_pipe_path, 40-1);
    printf("size: %ld\n", sizeof(client_pipe_path));


    if ((fd_client = open(client_pipe_path, O_RDONLY)) < 0){
        printf("error opening client: %s\n", strerror(errno));
        fflush(stdout);
        return -1;
    }

    return -1;
}

int tfs_unmount() {
    /* TODO: Implement this */
    return -1;
}

int tfs_open(char const *name, int flags) {
    /* TODO: Implement this */

    return -1;
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
