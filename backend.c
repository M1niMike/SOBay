#include "frontend.h"
#include "backend.h"

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

    if (id > 0)
    {
        printf("\nEntrei no pai!");
        close(0);     // fecha stdin
        dup(fd[0]);   // duplica o fd para conseguir ler o que esta no pipe
        close(fd[0]); // fecha porque nao estamos a usar
        close(fd[1]); // fecha o de escrita tambem
        close(1);     // fecha o stdout
        wait(&id);    // espera para que o filho acabe
        execl("./promotor", "promotor", NULL);
    }
    else if (id == 0)
    {

        printf("\nEntrei no filho!");
        printf("\nInsira a sua mensagem:\n");
        fgets(resposta, TAM, stdin);
        printf("\nSua resposta: {%s}", resposta);
        close(1);                    // mesmo conceito aqui, fecha o stdout
        dup(fd[1]);                  // duplica para a escrita
        write(fd[1], resposta, TAM); // escreve para a ponta de leitura
        close(fd[1]);                // fecha que nao estamos a usar
        read(fd[0], msgVolta, TAM);  // le no 0 pq é a ponta para ler
        close(fd[0]);                // fecha porque já não estamos a usar

        printf("\nRecebi do promotor: ", msgVolta);
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
    system("clear");
}

void interface()
{
    char cmd[TAM];

    int nPalavras = 0; // assumir que nao começamos com palavra nenhuma

    fflush(stdin);
    printf("Comando: \n");
    scanf("%s", cmd);

    char *token = strtok(cmd, " \n"); // ate ao espaco e /n por causa da ultima palavra
                                      // fflush(stdout);

    while (token != NULL)
    {
        nPalavras++;
        token = strtok(NULL, " ");
    }

    if (strcmp(cmd, "users") == 0)
    {
        printf("\nA ser implementado...\n");
    }
    else if (strcmp(cmd, "list") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "kick") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o nome do utilizador a ser kickado.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "prom") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "reprom") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "cancel") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o nome do executavel.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "close") == 0)
    {
        sair();
    }
    else if (strcmp(cmd, "help") == 0)
    {
        help();
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        clear();
    }
    else if (strcmp(cmd, "comandos") == 0)
    {
        printf("\nEntrei");
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
    fflush(stdout);
    
    char cmd[50];
    char ms[50];
    
    char nomeFich[TAM];

    ptritem item;

    item = malloc(sizeof(ITEM));
    if(item == NULL){
        printf("Erro na alocacao de memoria\n");
        free(item);
        return -1;
    }
    // execPromotor();

    while (1)
    {

        printf("\n---Bem vindo!---\n");
        printf("Desejar testar que funcionalidade?: comandos, execução do promotor, utilizadores, itens, sair\n");
        scanf("%s", ms);

        if (strcmp(ms, "comandos") == 0)
        {

            printf("%s", ms);
            interface();
        }
        else if (strcmp(ms, "itens") == 0){
        
            printf("\nInsira o nome do ficheiro: \n");
            scanf("%s", nomeFich);

           item = leFicheiroVendas(nomeFich);
        }
        else if (strcmp(ms, "utilizadores") == 0)
        {

            printf("\nA ser implementado\n");
        }
        else if (strcmp(ms, "promotor") == 0)
        {

            printf("\nA ser implementado\n");
            // execPromotor();
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