#include "tecnicofs_client_api.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NAME (40)
#define PERMISSION_FOR_ALL (0777)
#define BLOCK_SIZE (1024)

int fd_server, fd_client, id;

int tfs_mount(char const *client_pipe_path, char const *server_pipe_path) {

    if (unlink(client_pipe_path) == -1 && errno == EEXIST) {
        printf("client unlink error: %s\n", strerror(errno));
        return -1;
    }
    if (mkfifo(client_pipe_path, PERMISSION_FOR_ALL) == -1) {
        printf("mkfifo error: %s\n", strerror(errno));
        return -1;
    }
    if ((fd_server = open(server_pipe_path, O_WRONLY)) < 0) {
        printf("error opening server: %s\n", strerror(errno));
        return -1;
    }
    char opc = '1';
    if (write(fd_server, &opc, 1) < 0) {
        printf("error writing opcode");
        return -1;
    }
    if (write(fd_server, client_pipe_path, MAX_NAME) < 0) {
        printf("error writing client_pipe_path");
        return -1;
    }

    if ((fd_client = open(client_pipe_path, O_RDONLY)) < 0) {
        printf("error opening client: %s\n", strerror(errno));
        return -1;
    }

    int bytes_read;
    while (1) {
        if ((bytes_read = (int)read(fd_client, &id, sizeof(id))) == -1) {
            printf("error reading id\n");
            break;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }
    return 0;
}

int tfs_unmount() {
    /* TODO: Implement this */
    char opc = '2';
    if (write(fd_server, &opc, 1) == -1){
        return -1;
    }
    if (write(fd_server, &id, sizeof(int)) == -1)
        return -1;
    if (close(fd_client) == -1) {
        printf("error closing client pipe\n");
        return -1;
    }
    return 0;
}

int tfs_open(char const *name, int flags) {
    /* TODO: Implement this */
    char opc = '3';
    int ret_value, bytes_read;
    if (write(fd_server, &opc, 1) == -1)
        return -1;
    if (write(fd_server, &id, sizeof(id)) == -1)
        return -1;
    if (write(fd_server, name, MAX_NAME) == -1)
        return -1;
    if (write(fd_server, &flags, sizeof(flags)) == -1)
        return -1;

    while (1) {
        if ((bytes_read =
                 (int)read(fd_client, &ret_value, sizeof(ret_value))) == -1) {
            printf("error reading id\n");
            break;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }

    return ret_value;
}

int tfs_close(int fhandle) {
    char opc = '4';
    int ret_value, bytes_read;
    if (write(fd_server, &opc, 1) == -1)
        return -1;
    if (write(fd_server, &id, sizeof(id)) == -1)
        return -1;
    if (write(fd_server, &fhandle, sizeof(fhandle)) == -1)
        return -1;

    while (1) {
        if ((bytes_read =
                 (int)read(fd_client, &ret_value, sizeof(ret_value))) == -1) {
            printf("error reading id\n");
            break;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }

    return ret_value;
}

ssize_t tfs_write(int fhandle, void const *buffer, size_t len) {

    /* TODO: Implement this */
    char opc = '5';
    int ret_value, bytes_read;
    if (write(fd_server, &opc, 1) == -1)
        return -1;
    if (write(fd_server, &id, sizeof(id)) == -1)
        return -1;
    if (write(fd_server, &fhandle, sizeof(fhandle)) == -1)
        return -1;
    if (write(fd_server, buffer, BLOCK_SIZE) == -1)
        return -1;
    if (write(fd_server, &len, sizeof(len)) == -1)
        return -1;

    while (1) {
        if ((bytes_read =
                 (int)read(fd_client, &ret_value, sizeof(ret_value))) == -1) {
            printf("error reading id: %s\n", strerror(errno));
            break;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }
    return ret_value;
}

ssize_t tfs_read(int fhandle, void *buffer, size_t len) {
    /* TODO: Implement this */
    char opc = '6';
    int ret_value, bytes_read;
    if (write(fd_server, &opc, 1) == -1)
        return -1;
    if (write(fd_server, &id, sizeof(int)) == -1)
        return -1;
    if (write(fd_server, &fhandle, sizeof(int)) == -1)
        return -1;
    if (write(fd_server, &len, sizeof(size_t)) == -1)
        return -1;

    while (1) {
        if ((bytes_read =
                 (int)read(fd_client, &ret_value, sizeof(ret_value))) == -1) {
            printf("error reading bytes\n");
            return -1;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }
    char buffer_write[BLOCK_SIZE];
    while (1) {
        if ((bytes_read = (int)read(fd_client, &buffer_write, BLOCK_SIZE)) ==
            -1) {
            printf("error reading id: %s\n", strerror(errno));
            break;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }

    memcpy(buffer, buffer_write, (size_t)ret_value);
    return ret_value;
}

int tfs_shutdown_after_all_closed() {
    /* TODO: Implement this */
    char opc = '6';
    int ret_value, bytes_read;
    if (write(fd_server, &opc, 1) == -1)
        return -1;
    if (write(fd_server, &id, sizeof(int)) == -1)
        return -1;

    while (1) {
        if ((bytes_read =
                 (int)read(fd_client, &ret_value, sizeof(ret_value))) == -1) {
            printf("error reading bytes: %s\n", strerror(errno));
            return -1;
        } else if (bytes_read == 0) {
            continue;
        } else
            break;
    }
    return -1;
}