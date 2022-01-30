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
            puts("[ERR] error creating server pipe");
            return -1;
        }
    }

    printf("Starting TecnicoFS server with pipe called %s\n", pipename);
    
    /* TO DO */
    ssize_t n;
    char buffer[1024];
    while (1){
        n = read(pipename, buffer, 1024);
        if (n <= 0) { break; }
        trataMsg(buffer);
        client c;
        c.session_id = session_id;
        session_id++;
        strcpy(c.path, buffer);
        n = write(c.path, "connection established\n", 25);
    }

    return 0;
}