#include "operations.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define MAX_CLIENTS (1)

int session_id = 0;
typedef struct client_s{
    char path[40];
    int session_id;
} client;

client clients[MAX_CLIENTS];

int main(int argc, char **argv) {

    if (argc < 2) {
        printf("Please specify the pathname of the server's pipe.\n");
        return 1;
    }

    char *pipename = argv[1];

    if(mkfifo(pipename, 0777) == -1){
        if (errno != EEXIST){
            printf("[ERR] error creating server pipe: %s\n", strerror(errno));
            return -1;
        }
    }

    printf("Starting TecnicoFS server with pipe called %s\n", pipename);
    
    /* TO DO */
    ssize_t opt_read, string_read;
    char buffer[40];
    char opt;
    int fd_server, fd_client;
    if ((fd_server = open(pipename, O_RDONLY)) < 0){
        printf("%s\n", strerror(errno));
        return -1;
    }

    while (1){
        opt_read = read(fd_server, &opt, 1);
        if (opt_read <= 0 && errno != EEXIST){
            printf("opt: %ld\n", opt_read);
            printf("error reading OPCODE: %s\n", strerror(errno));
            break;
        }
        printf("opt: %c\n", opt);
        switch(opt){
            case '1':
                puts("antes do read");

                string_read = read(fd_server, buffer, 40-1);

                buffer[39] = 0; //enforce null termination
                printf("size: %ld\n", sizeof(buffer));
                clients[session_id].session_id = session_id;
                strcpy(clients[session_id].path, buffer);

                puts("copied.");
                puts(clients[session_id].path);

                if ((fd_client = open(clients[session_id].path, O_WRONLY)) < 0){
                    printf("error opening server -> client path: %s\n", strerror(errno));
                    fflush(stdout);
                    return -1;
                }
                session_id++;
                printf("s_id: %d\n", session_id);
                break;
            default:
                puts("switch case didnt match");
                break;
        }
        // opt = '0';
    }

    return 0;
}