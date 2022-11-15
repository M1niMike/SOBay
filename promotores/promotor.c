#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

#include "/home/micael/SO/SO-TP-22-23/backend.h"

int main(){

    USER *user;

    user = malloc(sizeof(USER));

    if (user == NULL){
        printf("\nFalha na alocacao de memoria!\n");
        free(user);
        return -1;
    }

    printf("\nOla bom dia\n");

    return 0;
}