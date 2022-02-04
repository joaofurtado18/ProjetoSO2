#include "operations.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS (1)
#define MAX_NAME (40)
#define PERMISSION_FOR_ALL (0777)

typedef struct client_s {
    int fd;
    int session_id;
} client;

client clients[MAX_CLIENTS];

int FREE_SESSION_ID_TABLE[MAX_CLIENTS];
int find_session_id() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (FREE_SESSION_ID_TABLE[i] == FREE) {
            FREE_SESSION_ID_TABLE[i] = TAKEN;
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }

    char *pipename = argv[1];

    if (unlink(pipename) == -1 && errno == EEXIST) {
        printf("server unlink error\n");
        return -1;
    }
    if (mkfifo(pipename, PERMISSION_FOR_ALL) == -1) {
        printf("[ERR] error creating server pipe: %s\n", strerror(errno));
        return -1;
    }

    if (tfs_init() == -1) {
        printf("error initializing tfs\n");
        return -1;
    }

    printf("Starting TecnicoFS server with pipe called %s\n", pipename);
    memset(FREE_SESSION_ID_TABLE, FREE, sizeof(FREE_SESSION_ID_TABLE));

    ssize_t bytes_read, string_read, written, len_read;
    size_t len;
    char buffer[MAX_NAME], buffer_write[BLOCK_SIZE];
    char opt;
    int fd_server, fd_client, int_read, id, flags, return_value, fhandle;
    if ((fd_server = open(pipename, O_RDONLY)) < 0) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    while (1) {
        bytes_read = (int)read(fd_server, &opt, 1);
        if (bytes_read == -1) {
            printf("\nbytes read: %ld\n", bytes_read);
            printf("error reading OPCODE: %s\n", strerror(errno));
            break;
        } else if (bytes_read == 0) {
            continue;
        }
        switch (opt) {
        case TFS_OP_CODE_MOUNT:

            string_read = (int)read(fd_server, buffer, MAX_NAME);
            if (string_read == -1) {
                printf("error reading client pipe path");
            }

            id = find_session_id();
            clients[id].session_id = id;

            if ((fd_client = open(buffer, O_WRONLY)) < 0) {
                printf("error opening server -> client path: %s\n",
                       strerror(errno));
                return -1;
            }
            clients[id].fd = fd_client;

            printf("s_id: %d\n", id);

            if ((written = write(clients[id].fd, &id, sizeof(id))) < 0) {
                printf("error writing id: %ld\n", written);
                return -1;
            }

            break;
        case TFS_OP_CODE_UNMOUNT:
            int_read = (int)read(fd_server, &id, sizeof(int));
            if (int_read == -1) {
                printf("error reading id");
            }
            FREE_SESSION_ID_TABLE[id] = FREE;
            if (close(clients[id].fd) == -1) {
                printf("error closing client pipe\n");
                return -1;
            }
            break;

        case TFS_OP_CODE_OPEN:

            int_read = (int)read(fd_server, &id, sizeof(int));
            if (int_read == -1) {
                printf("error reading id");
            }

            string_read = (int)read(fd_server, buffer, MAX_NAME);
            if (string_read == -1) {
                printf("error reading name");
            }

            int_read = (int)read(fd_server, &flags, sizeof(int));
            if (int_read == -1) {
                printf("error reading flag");
            }

            return_value = tfs_open(buffer, flags);
            if ((written = write(clients[id].fd, &return_value,
                                 sizeof(return_value))) < 0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            break;

        case TFS_OP_CODE_CLOSE:
            int_read = (int)read(fd_server, &id, sizeof(int));
            if (int_read == -1) {
                printf("error reading id");
            }

            int_read = (int)read(fd_server, &fhandle, sizeof(int));
            if (int_read == -1) {
                printf("error reading fhandle");
            }

            return_value = tfs_close(fhandle);
            if ((written = write(clients[id].fd, &return_value,
                                 sizeof(return_value))) < 0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            break;

        case TFS_OP_CODE_WRITE:
            int_read = (int)read(fd_server, &id, sizeof(int));
            if (int_read == -1) {
                printf("error reading id");
            }

            int_read = (int)read(fd_server, &fhandle, sizeof(int));
            if (int_read == -1) {
                printf("error reading fhandle");
            }

            string_read = (int)read(fd_server, buffer_write, BLOCK_SIZE);
            if (string_read == -1) {
                printf("error reading buffer");
            }

            len_read = (int)read(fd_server, &len, sizeof(size_t));
            if (len_read == -1) {
                printf("error reading len");
            }

            return_value = (int)tfs_write(fhandle, buffer_write, len);
            if ((written = write(clients[id].fd, &return_value, sizeof(int))) <
                0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            break;

        case TFS_OP_CODE_READ:

            int_read = (int)read(fd_server, &id, sizeof(int));
            if (int_read == -1) {
                printf("error reading id");
            }

            int_read = (int)read(fd_server, &fhandle, sizeof(int));
            if (int_read == -1) {
                printf("error reading fh");
            }

            len_read = (int)read(fd_server, &len, sizeof(size_t));
            if (len_read == -1) {
                printf("error reading len");
            }
            memset(buffer_write, 0, BLOCK_SIZE);
            return_value = (int)tfs_read(fhandle, buffer_write, len);
            if ((written = write(fd_client, &return_value, sizeof(int))) < 0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            if ((written = write(clients[id].fd, &buffer_write, BLOCK_SIZE)) <
                0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            break;
        case TFS_OP_CODE_SHUTDOWN_AFTER_ALL_CLOSED:
            return_value = tfs_destroy_after_all_closed();
            if ((written = write(clients[id].fd, &return_value,
                                 sizeof(return_value))) < 0) {
                printf("error writing ret val: %ld\n", written);
                return -1;
            }
            exit(EXIT_SUCCESS);
        default:
            printf("switch case didnt match, opt: %c\n", opt);
            break;
        }
    }

    return 0;
}