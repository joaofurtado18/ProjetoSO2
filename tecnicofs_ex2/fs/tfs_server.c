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
    char* path;
    int session_id;
} client;

client clients[MAX_CLIENTS];

void trataMsg (char* buf) {
    printf("Recebeu: %s\n", buf);
}


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
    ssize_t n;
    char buffer[40];
    char opt;
    int fd_server, fd_client;
    if ((fd_server = open(pipename, O_RDONLY)) < 0){
        printf("%s\n", strerror(errno));
        return -1;
    }
    puts("opened");
    n = read(fd_server, buffer, 40-1);
    if (n <= 0){
        printf("n: %s\n", strerror(errno));
    }

    while(1){
        puts("paradox");
        clients[session_id].session_id = session_id;
        puts("buffer");
        puts(buffer);
        strcpy(clients[session_id].path, buffer);
        puts("opaaa");
        clients[session_id].path[39] = 0;
        session_id++;
        puts(clients[session_id-1].path);
    }

    // while (1){
    //     n = read(fd_server, opt, 1);
    //     switch(n){
    //         case '1':

    //     }
    // }

    return 0;
}