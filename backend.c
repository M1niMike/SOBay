#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

pid_t id = -1;
char* FUSERS;
char* FITEMS;
char* FPROMOTERS;
int HEARTBEAT;
//char FPROMOTERS[TAM];

void getFilePaths(){
    
    FITEMS = getenv("FITEMS");
    FUSERS = getenv("FUSERS");
    HEARTBEAT = getenv("HEARTBEAT");

    if (FUSERS == NULL){
        printf("\nPor favor insira uma var de ambiente FUSERS para ler o ficheiro de texto que pretende!\n");
        return;
    } else if(FITEMS == NULL){
        printf("\nPor favor insira uma var de ambiente FITEMS para ler o ficheiro de texto que pretende!\n");
        return;
    } else if(FPROMOTERS == NULL){
        printf("\nPor favor insira uma var de ambiente FPROMOTERS para ler o ficheiro de promotores!");
        return;
    } else if(HEARTBEAT == NULL){
        printf("\nPor favor insira uma var de ambiente para o HEARTBEAT!\n");
        return;
    }
}

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

    if (id > 0)
    {
        read(fd[0], resposta, sizeof(resposta));
        close(fd[1]);
        printf("\n%s\n", resposta);

        // Encerra o promotor
        union sigval val;
        sigqueue(id, SIGUSR1, val);
        wait(&id);
    }
    else if (id == 0)
    {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        execl("./promotores/promotor_oficial", "./promotor_oficial", NULL);
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
    for (int i = 0; i < 30; i++)
    {
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
        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [users]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "list") == 0)
    {
        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [list]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "kick") == 0)
    {
        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [kick] [nomeUser]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "prom") == 0)
    {
        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [prom]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "reprom") == 0)
    {
        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [reprom]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "cancel") == 0)
    {
        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [cancel] [nomePromotor]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "close") == 0)
    {
        if (nPalavras == 1)
        {
            sair();
        }
        else
        {
            printf("\nInsira apenas [close]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "help") == 0)
    {
        if (nPalavras == 1)
        {
            help();
        }
        else
        {
            printf("\nInsira apenas [help]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "clear") == 0)
    {
        if (nPalavras == 1)
        {
            clear();
        }
        else
        {
            printf("\nInsira apenas [clear]\n");
        }
    }
    else
    {
        printf("\nComando invalido!\n");
    }
}

ptritem leFicheiroVendas()
{
    ptritem item;
    FILE *f = fopen(FITEMS, "r");
    int i = 0;

    item = malloc(30 * sizeof(ITEM));
    if (item == NULL)
    {
        printf("\nErro na alocacao de memoria\n");
        free(item);
        return NULL;
    }

    if (f == NULL)
    {
        printf("\nNao foi possivel abrir ficheiro [%s]!\n", FITEMS);
        return NULL;
    }

    printf("\nA ler info de ficheiro: [%s]\n", FITEMS);

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



int main(int argc, char **argv)
{

    char cmd[TAM];
    char ms[TAM];
    int i;

    ptritem item;
    ptruser user;

    getFilePaths();

    printf("%s", FUSERS);

    int quantUser = loadUsersFile(FUSERS);

    user = malloc(quantUser * sizeof(USER));
    if (user == NULL)
    {
        printf("\nErro na alocacao de memoria 1230\n");
        free(user);
        return -1;
    }


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
        else if (strcmp(ms, "itens") == 0)
        {
            //nomeFich[strcspn(FITEMS, "\n")] = 0;
            item = leFicheiroVendas(FITEMS);
        }
        else if (strcmp(ms, "utilizadores") == 0)
        {

            for (i = 0; i < quantUser; i++)
            {

                printf("\nInsira o nome do utilizador: ");
                fgets(user[i].nome, TAM, stdin);
                user[i].nome[strcspn(user[i].nome, "\n")] = 0;

                printf("\nInsira a password do utilizador: ");
                fgets(user[i].pass, TAM, stdin);
                user[i].pass[strcspn(user[i].pass, "\n")] = 0;

                if (quantUser <= 0)
                {
                    printf("\nErro na leitura de Ficheiro\n");
                }

                if (isUserValid(user[i].nome, user[i].pass) == 1)
                {
                    printf("\nLogin com exito\n");
                }
                else if (isUserValid(user[i].nome, user[i].pass) == 0)
                {
                    printf("\nPassword incorreta | user inexistente\n");
                    break;
                }
                else if (isUserValid(user[i].nome, user[i].pass) == -1)
                {
                    printf("\nErro\n");
                }

                user[i].saldo = getUserBalance(user[i].nome);

                user[i].saldo -= 1;

                updateUserBalance(user[i].nome, user[i].saldo);

                printf("\nUser %s tem %d saldo depois da atualizaçao\n", user[i].nome, user[i].saldo);

            }

            printf("\nSaldos de %d utilizadores foram atualizados!\n", i);

            saveUsersFile(FUSERS);

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