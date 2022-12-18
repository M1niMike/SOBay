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
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#define TAM 256
#define BACKEND_FIFO "backend_fifo"
#define SINAL_FIFO "sinal_fifo"
#define SELLER_BUYER_FIFO "seller_buyer_fifo%d"

char SELLER_BUYER_FIFO_COM[TAM];


// estrutura para definir os utilizadores (sejam compradores ou vendedores)
typedef struct item{
    int duracao;
    int idItem;
    char nomeItem[TAM];
    char categoria[TAM];
    int valorAtual; // pode tb ser valorInicial
    int valorCompreJa;
    char sellerName[TAM];
    char highestBidder[TAM];
}ITEM, *ptritem;

typedef struct user{
    pid_t pid;
    char pass[TAM];
    int saldo;
    char nome[TAM];
    char mensagem[TAM];
    int isLoggedIn;
    char comando[TAM];
}USER, *ptruser;

typedef struct backend{
    int numItens;
    int numUsers;
    int numPromoters;
    int time;
    ptruser utilizadores;
    ptritem itens;
}BACKEND, *ptrbackend;


#endif //SO_BACKEND_H