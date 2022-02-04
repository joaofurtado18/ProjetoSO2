#include "operations.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS (1)

typedef struct client_s{
    char path[40];
    int session_id;
} client;

client clients[MAX_CLIENTS];

int FREE_SESSION_ID_TABLE[MAX_CLIENTS];
int find_session_id(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (FREE_SESSION_ID_TABLE[i] == 0){
            FREE_SESSION_ID_TABLE[i] = 1;
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

    if (unlink(pipename) == -1 && errno == EEXIST){
        printf("server unlink error\n");
        return -1;
    }
    if(mkfifo(pipename, 0777) == -1){
        printf("[ERR] error creating server pipe: %s\n", strerror(errno));
        return -1;
    }
    
    if (tfs_init() == -1){
        printf("error initializing tfs\n");
        return -1;
    }

    printf("Starting TecnicoFS server with pipe called %s\n", pipename);
    memset(FREE_SESSION_ID_TABLE, 0, sizeof(FREE_SESSION_ID_TABLE));

    
    /* TO DO */
    ssize_t bytes_read, string_read;
    char buffer[40];
    char opt;
    int fd_server, fd_client, int_read, id, flags, return_value;
    ssize_t written;
    if ((fd_server = open(pipename, O_RDONLY)) < 0){
        printf("%s\n", strerror(errno));
        return -1;
    }

    while (1){
        bytes_read = read(fd_server, &opt, 1);
        if (bytes_read == -1){
            printf("\nbytes read: %ld\n", bytes_read);
            printf("error reading OPCODE: %s\n", strerror(errno));
            break;
        } else if (bytes_read == 0){
            continue;
        }
        switch(opt){
            case '1':

                string_read = read(fd_server, buffer, 40);
                if (string_read == -1){
                    printf("error reading client pipe path");
                }

                id = find_session_id();
                clients[id].session_id = id;
                memcpy(clients[id].path, buffer, sizeof(buffer));

                printf("%s\n", clients[id].path);

                if ((fd_client = open(clients[id].path, O_WRONLY)) < 0){
                    printf("error opening server -> client path: %s\n", strerror(errno));
                    return -1;
                }

                printf("s_id: %d\n", id);


                if ((written = write(fd_client, &id , sizeof(id))) < 0){
                    printf("error writing id: %ld\n", written);
                    return -1;
                }

                break;

            
            case '3':
                
                printf("%d\n", fd_server);
                int_read = read(fd_server, &id, sizeof(int));
                if (int_read == -1){
                    printf("error reading id");
                }

                string_read = read(fd_server, buffer, 40);
                if (string_read == -1){
                    printf("error reading name");
                }

                printf("%d\n", fd_server);
                int_read = read(fd_server, &flags, sizeof(int));
                if (int_read == -1){
                    printf("error reading flag");
                }
                puts("saçv2222eee");


                return_value = tfs_open(buffer, flags);
                puts("saçveee");
                if ((written = write(fd_client, &return_value , sizeof(return_value))) < 0){
                    printf("error writing ret val: %ld\n", written);
                    return -1;
                }
                break;
            case '6':
                
                int_read = read(fd_server, &id, sizeof(id));
                if (int_read == -1){
                    printf("error reading id");
                }

                int_read = read(fd_server, &flags, sizeof(flags));
                if (int_read == -1){
                    printf("error reading fh");
                }

                break;


            default:
                printf("switch case didnt match, opt: %c\n", opt);
                break;
        }
    }

    return 0;
}