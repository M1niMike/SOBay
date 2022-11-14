#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

int saldo = 10;

void execPromotor()
{
    USER user;
    int fd[2];
    char msgVolta[TAM];
    pipe(fd);
    char resposta[TAM];

    int id = fork();

    if (id == -1)
    {
        printf("\nFalha na execucao do fork()");
        return;
    }

    if(id > 0){
        read(fd[0], resposta, sizeof(resposta));
        close(fd[1]);
        printf("%s", resposta);
    }
    else if(id == 0){
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        execl("./promotores/promotor", "./promotor", NULL);
    }
}

// quando tivermos o ficheiro dos users:

void testaUsers(USER *user)
{ // um set para o saldo do utilizador

    user = malloc(sizeof(USER));

    if(user == NULL){
        printf("Erro na alocacao de memoria\n");
        free(user);
        return;
    }

    saldo--;
    user->saldo = saldo;
}

void help()
{
    printf("---------------------\n");
    printf("COMANDOS\n");
    printf("[users]  - \n");
    printf("[list] - \n");
    printf("[kick <username>] - \n");
    printf("[prom] - \n");
    printf("[reprom]  - \n");
    printf("[cancel <nome-do-executavel-do-promotor>]  - \n");
    printf("[close] - \n");
    printf("[help] - mostra os comandos disponiveis no sistema.\n");
    printf("[clear] - \n");
    printf("---------------------\n");
}

void sair()
{
    printf("Fim do programa!\n");
    exit(EXIT_SUCCESS);
}

void clear()
{
    for(int i = 0; i < 30; i++){
        printf("\n");
    }
    
}

void interface()
{
    char cmd[TAM];
    char primeiraPalavra[TAM];

    int nPalavras = 0; // assumir que nao começamos com palavra nenhuma

    fflush(stdin);
    printf("Comando: \n");
    fgets(cmd, TAM, stdin);

    char *tokenfw = strtok(cmd, " \n"); // ate ao espaco e /n por causa da ultima palavra
                                      // fflush(stdout);

    strcpy(primeiraPalavra, tokenfw);
    while (tokenfw != NULL)
    {
        nPalavras++;
        tokenfw = strtok(NULL, " ");
        
    }

    if (strcmp(primeiraPalavra, "users") == 0)
    {
        if(nPalavras == 1){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [users]\n");
        }
        
    }
    else if (strcmp(primeiraPalavra, "list") == 0)
    {
        if(nPalavras == 1){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [list]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "kick") == 0)
    {
        if(nPalavras == 2){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [kick] [nomeUser]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "prom") == 0)
    {
        if(nPalavras == 1){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [prom]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "reprom") == 0)
    {
        if(nPalavras == 1){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [reprom]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "cancel") == 0)
    {
        if(nPalavras == 2){
            printf("\nA ser implementado...\n");
        }else{
            printf("\nInsira apenas [cancel] [nomePromotor]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "close") == 0)
    {
        if(nPalavras == 1 ){
             sair();
        }else{
            printf("\nInsira apenas [close]\n");
        }
       
    }
    else if (strcmp(primeiraPalavra, "help") == 0)
    {
        if(nPalavras == 1 ){
             help();
        }else{
            printf("\nInsira apenas [help]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "clear") == 0)
    {
        if(nPalavras == 1 ){
             clear();
        }else{
            printf("\nInsira apenas [clear]\n");
        }
    }
    else
    {
        printf("\nComando invalido!\n");
    }
}

ptritem leFicheiroVendas(char *nomeFich)
{
    char linha[150];
    ITEM *item;
    FILE *f = fopen(nomeFich, "r");

    item = malloc(sizeof(ITEM));
    if(item == NULL){
        printf("Erro na alocacao de memoria\n");
        free(item);
        return NULL;
    }

    if (f == NULL)
    {
        printf("\nNao foi possivel abrir ficheiro [%s]!\n", nomeFich);
        return NULL;
    }

    printf("\nA ler info de ficheiro: [%s]\n", nomeFich);

    while (fscanf(f, "%d %s %s %d %d %d %s %s", &item->idItem, item->nomeItem, item->categoria, &item->valorAtual, &item->valorCompreJa, &item->duracao, item->sellerName, item->highestBidder) != EOF)
    {

        printf("\n...............ITEM %d...............\n", item->idItem);

        printf("\nID do item: %d\n", item->idItem);
        printf("Nome do item: %s\n", item->nomeItem);
        printf("Categoria: %s\n", item->categoria);
        printf("Valor atual do item: %d\n", item->valorAtual);
        printf("Valor Compre Ja: %d\n", item->valorCompreJa);
        printf("Duracao do leilao: %d\n", item->duracao);
        printf("Vendedor: %s\n", item->sellerName);
        printf("Licitador mais elevado: %s\n", item->highestBidder);
    }

    fclose(f);

    return item;
}

int main(int argc, char **argv)
{

    char cmd[TAM];
    char ms[TAM];
    
    char nomeFich[TAM];

    ptritem item;

    item = malloc(sizeof(ITEM));
    if(item == NULL){
        printf("Erro na alocacao de memoria\n");
        free(item);
        return -1;
    }
    // execPromotor();

     printf("\n---Bem vindo Administrador---\n");

    while (1)
    {
        printf("Deseja testar que funcionalidade?: <comandos> <promotor> <utilizadores> <itens> <sair>\n");
        fgets(ms, TAM, stdin);
        ms[strcspn(ms, "\n")] = 0; // strcspn conta os caracteres de uma string evitando o \n - set \n to 0

        if (strcmp(ms, "comandos") == 0)
        {
            interface();
        }
        else if (strcmp(ms, "itens") == 0){
        
            printf("\nInsira o nome do ficheiro: \n");
            fgets(nomeFich, TAM, stdin);
            nomeFich[strcspn(nomeFich, "\n")] = 0;

           item = leFicheiroVendas(nomeFich);
        }
        else if (strcmp(ms, "utilizadores") == 0)
        {

            // users_lib.h -- teste funçoes
            loadUsersFile("users.txt");

            int i = getUserBalance("mike");

            printf("\n[%d]\n", i);

            updateUserBalance("mike", 500);

            saveUsersFile("users.txt");

            printf("\n[%d]\n", i);
        }
        else if (strcmp(ms, "promotor") == 0)
        {
            execPromotor();
        }
        else if (strcmp(ms, "sair") == 0)
        {

            printf("\nA sair...\n");
            exit(EXIT_SUCCESS);
        }
        else
        {

            printf("\nFuncionalidade indisponivel\n");
        }
    }

    return 0;
}

// PARA RELEMBRAR, TAM DE FICHEIROS

/*fseek(f, 0, SEEK_END); // ir até ao fim do ficheiro

    int tam = ftell(f); // associar o tamanho do ficheiro (que esta no fim) a uma var

    fseek(f, 0, SEEK_SET); // voltar a meter o ficheiro no inicio

    char *valorFich = malloc(sizeof(int) * (tam+1)); // alocar espaco para a variavel que vai ter todos os valores para tam+1 por causa do /n

    while (fscanf(f, "%s", linha) != EOF){
        puts(linha);
    }

    /*for (int i = 0; i < 15; i++){
        valorFich[i] = atoi(linha);
    }*/