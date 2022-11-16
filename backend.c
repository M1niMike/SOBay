#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

pid_t id = -1;


void execPromotor()
{
    int fd[2];
    char msgVolta[TAM];
    pipe(fd);
    char resposta[TAM];
    
    id = fork();

    if (id == -1)
    {
        printf("\nFalha na execucao do fork()");
        return;
    }

    if(id > 0){
        
        read(fd[0], resposta, sizeof(resposta));
        close(fd[1]);
        printf("\n%s\n", resposta);

        //Encerra o promotor
        signal(SIGCHLD, SIG_IGN); //to ignore the signal sent to the parent when a child dies
        kill(id, SIGKILL);

    }
    else if(id == 0){
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        execl("./promotores/promotor_oficial", "./promotor_oficial" , NULL);
          
    }
    
    
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
    ptritem item;
    FILE *f = fopen(nomeFich, "r");
    int i = 0;

    item = malloc(30 * sizeof(ITEM));
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

    while (fscanf(f, "%d %s %s %d %d %d %s %s", &(item[i].idItem), item[i].nomeItem, item[i].categoria, &(item[i].valorAtual), &(item[i].valorCompreJa), &(item[i].duracao), item[i].sellerName, item[i].highestBidder) != EOF)
    {

        printf("\n...............ITEM %d...............\n", item[i].idItem);

        printf("\nID do item: %d\n", item[i].idItem);
        printf("Nome do item: %s\n", item[i].nomeItem);
        printf("Categoria: %s\n", item[i].categoria);
        printf("Valor atual do item: %d\n", item[i].valorAtual);
        printf("Valor Compre Ja: %d\n", item[i].valorCompreJa);
        printf("Duracao do leilao: %d\n", item[i].duracao);
        printf("Vendedor: %s\n", item[i].sellerName);
        printf("Licitador mais elevado: %s\n", item[i].highestBidder);

        i++;

    }

    

    fclose(f);

    return item;
}

ptruser leFicheiroUsers(ptruser user ,char * nomeFicheiro, int quant){

    int i = 0;
    FILE *f = fopen(nomeFicheiro, "r");

    user = malloc(quant*sizeof(USER));
    if(user == NULL){
        printf("Erro na alocacao de memoria\n");
        free(user);
        return NULL;
    }

    if (f == NULL)
    {
        printf("\nNao foi possivel abrir ficheiro [%s]!\n", nomeFicheiro);
        return NULL;
    }
    printf("\nA ler info de ficheiro: [%s]\n", nomeFicheiro);

    while (fscanf(f, "%s %s %d", user[i].nome, user[i].pass, &(user[i].saldo)) != EOF)
    {

        printf("\n...............User %s...............\n", user[i].nome);

        printf("Nome do user: %s\n", user[i].nome);
        printf("Password do user: %s\n", user[i].pass);
        printf("Saldo do user: %d\n", user[i].saldo);

        i++;
    }

  

    fclose(f);

    return user;

}

int main(int argc, char **argv)
{

    char cmd[TAM];
    char ms[TAM];
    
    char nomeFich[TAM];

    ptritem item;
    ptruser user;

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

            //printf("\n[%s]\n",item->nomeItem);
        }
        else if (strcmp(ms, "utilizadores") == 0)
        {

            int quantUsers = loadUsersFile("users.txt");
            printf("\n[%d]\n", quantUsers);

            user = leFicheiroUsers(user,"users.txt", quantUsers);
            for(int i=0; i<quantUsers;i++){
                updateUserBalance(user[i].nome, user[i].saldo-=1);

            }
            saveUsersFile("users.txt");

        
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