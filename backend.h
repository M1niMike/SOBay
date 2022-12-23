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
    int isLoggedIn;
    int tempoLogged;
    char comando[TAM];
    char userStatus[TAM];
    int timeBackend;
}USER, *ptruser;

typedef struct promotores{
    char nome[TAM];
    int contaPromotores;
    char msg[TAM];
}PROMOTORES, *ptrpromotores;

typedef struct backend{
    int numItens;
    int numUsers;
    int time;
    int numPromoters;
    ptruser utilizadores;
    ptritem itens;
    ptrpromotores promotores;
}BACKEND, *ptrbackend;

typedef struct comunica{
    int timeBackend;
    char mensagem[TAM];
    char comando[TAM];
    int saldo;
    int retorno;
    ptritem itens;
}COMUNICA, *ptrcomunica;

void execPromotor(char* name);
void readPromotersFile(ptrbackend backend);
void encerra(ptrbackend backend, int numUsers, int numItens, ptritem itens);
void leFicheiroVendas(ptrbackend backend);
void adicionaPessoa(ptrbackend backend, USER u, int maxUsers);
void resetDados(ptrbackend backend, ptruser user);
void resetDadosItens(ptrbackend backend, ptritem item);
void removeItensFromArray(ptrbackend backend, ITEM item);
void removePessoaFromArray(ptrbackend backend, USER user);
void *aumentaTempo(void *dados);
void *aumentaTempoItem(void *dados);
void resetUserTime(ptrbackend backend, int pid);
void cmdListBackend(ptrbackend backend);
void cmdKickBackend(ptrbackend backend, char *nome);
void cmdUsersBackend(BACKEND backend);
void interface(BACKEND backend, USER user, ITEM item);
void cmdSell(char *nomeItem, char *categoria, int precoBase, int compreJa, int duracao, ptrbackend backend, USER user, int maxItens);
void cmdBuy(USER user, int id, int valor, ptrbackend backend, ptrcomunica comunica);
void cmdAdd(COMUNICA comunica, USER user, ptrbackend backend, int saldoToAdd);
void giveMoneyToSeller(ptrbackend backend);
void giveMoneyToSellerCompreJa(ptrbackend backend);
void *promocoes(void *dados);


#endif //SO_BACKEND_H