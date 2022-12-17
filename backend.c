#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

int utilizador_fd, sinais_fd, backend_fd;
pid_t id = -1;
char *FUSERS;
char *FITEMS;
char *FPROMOTERS;
int HEARTBEAT;
// char FPROMOTERS[TAM];

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void getFilePaths()
{

    // no need to export from the shell
    setenv("FUSERS", "users.txt", 1);
    setenv("FITEMS", "vendas.txt", 1);
    setenv("HEARTBEAT", "20", 1);

    FITEMS = getenv("FITEMS");
    FUSERS = getenv("FUSERS");
    HEARTBEAT = atoi(getenv("HEARTBEAT"));

    if (FUSERS == NULL)
    {
        printf("\nPor favor insira uma var de ambiente FUSERS para ler o ficheiro de texto que pretende!\n");
        return;
    }
    else if (FITEMS == NULL)
    {
        printf("\nPor favor insira uma var de ambiente FITEMS para ler o ficheiro de texto que pretende!\n");
        return;
    }
    else if (FPROMOTERS == NULL)
    {
        printf("\nPor favor insira uma var de ambiente FPROMOTERS para ler o ficheiro de promotores!");
        return;
    }
    else if (HEARTBEAT == 0)
    {
        printf("\nPor favor insira uma var de ambiente para o HEARTBEAT!\n");
        return;
    }

    // printf("%d", HEARTBEAT);
}

void verificaUser(USER user, BACKEND backend, int quantUser)
{
    // chamar a funcao do stor para validar os dados introduzidos no argv (passados por parametro)
    // se não for valido, imprime login sem exito
    // se for então vai dar loop por todos os users do backend (os que estao armazenados na estrutura)
    // se o nome do user inserido no argv ja estiver contido no backend, printf("Ja existe, logging in.");
    // se nao estiver

    if (isUserValid(user.nome, user.pass) == 1)
    {
        printf("\nUser is valid.");

        for (int i = 0; i < quantUser; i++)
        {
            if (strcmp(backend.utilizadores[i].nome, user.nome) != 0)
            { // o nome não está contido no backend.
                if (backend.numUsers == 20)
                {
                    printf("\nNao consigo inserir mais utilizadores! (MAX = 20)");
                    unlink(SELLER_BUYER_FIFO_COM);
                    kill(backend.utilizadores->pid, SIGQUIT);
                    break;
                }
                backend.numUsers++;
                printf("\nUser added to backend.\n");
            }
            else
            { // o nome está contido no backend
                printf("\nUser is valid. Logging in...\n");
            }
        }
    }
    else if (isUserValid(user.nome, user.pass) == 0)
    {
        printf("\nLogin incorreto. Login sem exito...\n");
    }
    else
    {
        printf("\nErro wtf\n");
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

        exit(-1);
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

void clear()
{
    for (int i = 0; i < 30; i++)
    {
        printf("\n");
    }
}

void encerra(ptrbackend backend, int numUsers)
{
    for (int i = 0; i < numUsers; i++)
    {
        if (backend->utilizadores[i].pid != 0)
        {
            // printf("Encerrando Users(%d)...\n" );
            printf("[%s]", backend->utilizadores[i].nome);
            //  sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, backend->utilizadores[i].pid);
            unlink(SELLER_BUYER_FIFO_COM);
            kill(backend->utilizadores[i].pid, SIGTERM);
        }
    }

    unlink(BACKEND_FIFO);
    kill(getpid(), SIGTERM);
    // encerrar os promotores tambem
}

void interface(BACKEND backend)
{
    char cmd[TAM];
    char primeiraPalavra[TAM];

    int nPalavras = 0; // assumir que nao começamos com palavra nenhuma

    fgets(cmd, sizeof(cmd), stdin);

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
            encerra(&backend, backend.numUsers);
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

    while (fscanf(f, "%d %s %s %d %d %d %s %s",
                  &(item[i].idItem), item[i].nomeItem, item[i].categoria, &(item[i].valorAtual), &(item[i].valorCompreJa), &(item[i].duracao), item[i].sellerName, item[i].highestBidder) != EOF)
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

void verificaServidor()
{ // verifica se já existe um balcao ativo

    if (access(BACKEND_FIFO, F_OK) == 0)
    {
        printf("Aviso [já tem o servidor ativo]\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char **argv)
{

    char cmd[TAM];
    int i;
    BACKEND backend;
    struct timeval tv;
    fd_set read_fds;
    USER u;
    VOLTA v;
    fflush(stdout);

    verificaServidor();

    getFilePaths();
    backend.numUsers = loadUsersFile(FUSERS);

    backend.utilizadores = malloc(backend.numUsers * sizeof(*backend.utilizadores));
    backend.itens = malloc(atoi(FITEMS) * sizeof(*backend.itens));
    // backend.numPromoters = malloc(10 * sizeof(*backend.));

    if (mkfifo(BACKEND_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Erro [Criacao - FIFO BACKEND]\n");
            exit(EXIT_FAILURE);
        }
    }

    backend_fd = open(BACKEND_FIFO, O_RDWR);

    if (backend_fd == -1)
    {
        perror("\nNao foi possivel abrir o fifo do BACKEND!\n");
        return -1;
    }

    printf("\nServidor do backend a correr...\n");

    printf("\n---Bem vindo Administrador---\n");

    while (1)
    {
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(backend_fd, &read_fds);
        // FD_SET(sinais_fd, &read_fds);

        int nfd = select(max(backend_fd, sinais_fd) + 1, &read_fds, NULL, NULL, &tv);

        if (nfd == 0)
        {
            printf("\nEstou a espera de utilizadores...\n");
        }

        if (nfd == -1)
        {
            perror("\nErro no select!\n");
        }

        if (FD_ISSET(0, &read_fds))
        {
            interface(backend);
        }

        if (FD_ISSET(backend_fd, &read_fds))
        {

            printf("Entrei no Backend\n");

            if (read(backend_fd, &u, sizeof(USER)) == -1)
            {
                printf("[ERRO] Read - FIFO Backend\n");
                unlink(BACKEND_FIFO);
                exit(EXIT_FAILURE);
            }

            for (int i = 0; i < backend.numUsers; i++)
            {
                strcpy(backend.utilizadores[i].nome, u.nome);
                strcpy(backend.utilizadores[i].pass, u.pass);
                backend.utilizadores[i].pid = u.pid;
                
            }

            // sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);
            // utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);

            // if (utilizador_fd == -1)
            // {
            //     perror("\nNao foi possivel abrir o fifo do UTILIZADOR!\n");
            //     return -2;
            // }

            //verificaUser(u, backend, backend.numUsers);

        for(int i = 0; i < backend.numUsers; i++){
            if (isUserValid(backend.utilizadores[i].nome, backend.utilizadores[i].pass) == 1)
            {
                printf("User is valid\n");
                if (read(backend_fd, &v, sizeof(strlen(v.comando))) == -1)
                {
                    printf("[ERRO] Read - FIFO Backend\n");
                    unlink(BACKEND_FIFO);
                    exit(EXIT_FAILURE);
                }

                 printf("Recebeu do User: %s\n", v.comando);
            }else if(isUserValid(backend.utilizadores[i].nome, backend.utilizadores[i].pass) == 0){
                printf("User is not valid\n");
            }else if(isUserValid(backend.utilizadores[i].nome, backend.utilizadores[i].pass) == -1){
                printf("Erro\n");
            }
        }
            

            // printf("Recebeu do User\n");

            // if (read(backend_fd, &v, sizeof(v.comando)) == -1)
            // {
            //     printf("[ERRO] Read - FIFO Backend\n");
            //     // unlink(BACKEND_FIFO);
            //     // exit(EXIT_FAILURE);
            // }

            // do{

            // }while(isUserValid(u.nome, u.pass) == 1)

           

            // close(backend_fd);
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

//  for (i = 0; i < quantUser; i++)
//         {

//             printf("\nInsira o nome do utilizador: ");
//             fgets(user[i].nome, TAM, stdin);
//             user[i].nome[strcspn(user[i].nome, "\n")] = 0;

//             printf("\nInsira a password do utilizador: ");
//             fgets(user[i].pass, TAM, stdin);
//             user[i].pass[strcspn(user[i].pass, "\n")] = 0;

//             if (quantUser <= 0)
//             {
//                 printf("\nErro na leitura de Ficheiro\n");
//             }

//             if (isUserValid(user[i].nome, user[i].pass) == 1)
//             {
//                 printf("\nLogin com exito\n");
//             }
//             else if (isUserValid(user[i].nome, user[i].pass) == 0)
//             {
//                 printf("\nPassword incorreta | user inexistente\n");
//                 break;
//             }
//             else if (isUserValid(user[i].nome, user[i].pass) == -1)
//             {
//                 printf("\nErro\n");
//             }

//             user[i].saldo = getUserBalance(user[i].nome);

//             user[i].saldo -= 1;

//             updateUserBalance(user[i].nome, user[i].saldo);

//             printf("\nUser %s tem %d saldo depois da atualizaçao\n", user[i].nome, user[i].saldo);

//         }

//         printf("\nSaldos de %d utilizadores foram atualizados!\n", i);

//         saveUsersFile(FUSERS);

// strcmp(ms, "sair") == 0)
//     {
//         unlink(BACKEND_FIFO);
//         printf("\nA sair...\n");
//         exit(EXIT_SUCCESS);

// if (u.isLoggedIn == 0)
// {
//     if (isUserValid(u.nome, u.pass) == 1)
//     {
//         u.isLoggedIn = 1;

//         int size = write(utilizador_fd, &u, sizeof(USER));
//         if (size < 0)
//         {
//             perror("\nErro no write. No bytes ");
//         }
//         printf("User is valid\n");
//         // read(utilizador_fd, &u, sizeof(USER)); // le os comandos do user
//         // parse aos comandos
//     }

//     else if (isUserValid(u.nome, u.pass) == 0)
//     {
//         u.isLoggedIn = 0;

//         int size = write(utilizador_fd, &u, sizeof(USER));
//         if (size < 0)
//         {
//             perror("\nErro no write. No bytes ");
//         }

//     }
// }
// else
// {
//     continue;
// }

// close(backend_fd);