//
// Created by mike on 11/3/21.
//

#ifndef SO_BACKEND_H
#define SO_BACKEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#define TAM 256

typedef struct item{
    int duracao;
    int idItem;
    char nomeItem[TAM];
    char categoria[TAM];
    int valorAtual; // pode tb ser valorInicial wtv
    int valorCompreJa;
    char sellerName[TAM];
    char highestBidder[TAM];
}ITEM, *ptritem;

typedef struct user{
    char pass[TAM];
    int saldo;
    char nome[TAM];
    char mensagem[TAM];
}USER, *ptruser;

#endif //SO_BACKEND_H