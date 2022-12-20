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
    // else if (HEARTBEAT == 0)
    // {
    //     printf("\nPor favor insira uma var de ambiente para o HEARTBEAT!\n");
    //     return;
    // }

    // printf("%d", HEARTBEAT);
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
            kill(backend->utilizadores[i].pid, SIGTERM);
        }
    }

    unlink(BACKEND_FIFO);
    unlink(SINAL_FIFO);
    kill(getpid(), SIGTERM);
    // encerrar os promotores tambem
}

void adicionaPessoa(ptrbackend backend, USER u, int maxUsers)
{
    if (backend->numUsers == maxUsers)
    {
        printf("\nNao consigo adicionar mais users. (CHEGOU AO LIMITE)\n");
        return;
    }

    for (int i = 0; i < maxUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, u.nome) == 0)
        {
            printf("\nUtilizador [%s] ja estava logado\n", backend->utilizadores[i].nome);
            kill(u.pid, SIGQUIT); // temp
            break;
        }
    }

    backend->utilizadores[backend->numUsers] = u;
    backend->numUsers++;
}

void resetDados(ptrbackend backend, ptruser user)
{
    strcpy(user->nome, "\0");
    strcpy(user->pass, "\0");
    strcpy(user->comando, "\0");
    user->saldo = 0;
    user->isLoggedIn = 0;
    user->pid = 0;
    user->tempoLogged = 0;
}

void removePessoaFromArray(ptrbackend backend, USER user)
{

    for (int i = 0; i < backend->numUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, user.nome) == 0) // se o nome for igual
        {
            if (backend->utilizadores[i].pid == user.pid) // e ele existir
            {
                printf("\n(%s) removido por inatividade\n", backend->utilizadores[i].nome);
                kill(backend->utilizadores[i].pid, SIGUSR1);
                resetDados(backend, &backend->utilizadores[i]);
                // backend->numUsers--;
                break;
            }
        }
    }
}

void CmdRemovePessoaFromArray(ptrbackend backend, char *nome)
{

    for (int i = 0; i < backend->numUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, nome) == 0) // se o nome for igual
        {
            if (backend->utilizadores[i].pid != 0) // e ele existir
            {

                kill(backend->utilizadores[i].pid, SIGUSR1);
                resetDados(backend, &backend->utilizadores[i]);
                backend->numUsers--;
                printf("Kickou %s\n", nome);
                break;
            }
        }
    }
}

void *aumentaTempo(void *dados)
{
    ptrbackend pdados = (ptrbackend)dados;

    while (1)
    {
        // para contar o tempo, mandá-lo dormir 1 segundo e incrementar a var do tempo
        sleep(1);
        pdados->time++;
        for (int i = 0; i < pdados->numUsers; i++)
        {
            if (pdados->utilizadores[i].pid != 0)
            { // o utilizador nesta posição tem um pid != de 0, ou seja, existe
                pdados->utilizadores[i].tempoLogged++;
                if (pdados->utilizadores[i].tempoLogged >= HEARTBEAT)
                {
                    removePessoaFromArray(pdados, pdados->utilizadores[i]);
                    // unlink(SELLER_BUYER_FIFO_COM);
                }
            }
        }
    }
}

void resetUserTime(ptrbackend backend, int pid)
{ // respondeu ao heartbeat, set no tempo a 0
    for (int i = 0; i < backend->numUsers; i++)
    {
        if (backend->utilizadores[i].pid == pid)
        {
            backend->utilizadores[i].tempoLogged = 0;
        }
    }
}

void cmdUsers(BACKEND backend)
{

    for (int i = 0; i < backend.numUsers; i++)
    {
        if (backend.utilizadores[i].pid != 0)
        {
            printf("\nNome: %s - PID: %d\n", backend.utilizadores[i].nome, backend.utilizadores[i].pid);
        }
    }

    //print a avisa que nao tem users
}

void interface(BACKEND backend, USER user)
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
            cmdUsers(backend);
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
    int i;
    BACKEND backend;
    struct timeval tv;
    fd_set read_fds;
    USER u;
    fflush(stdout);
    int maxUsers = 20;
    int numUsersInTextFile;
    int maxItens = 30;
    backend.numUsers = 0;
    pthread_t incrementaTempo;
    pthread_t recebeSinal;

    u.isLoggedIn = 0;

    verificaServidor();

    getFilePaths();
    numUsersInTextFile = loadUsersFile(FUSERS); // conta os users do ficheiro de texto.

    backend.utilizadores = malloc(maxUsers * sizeof(*backend.utilizadores));
    backend.itens = malloc(maxItens * sizeof(*backend.itens));
    // backend.numPromoters = malloc(10 * sizeof(*backend.));

    if (mkfifo(BACKEND_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Erro [Criacao - FIFO BACKEND]\n");
            exit(EXIT_FAILURE);
        }
    }

    if (mkfifo(SINAL_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("Erro [Criacao - FIFO SINAIS]\n");
            exit(EXIT_FAILURE);
        }
    }

    backend_fd = open(BACKEND_FIFO, O_RDWR | O_NONBLOCK);

    if (backend_fd == -1)
    {
        perror("\nNao foi possivel abrir o fifo do BACKEND!\n");
        return -1;
    }

    sinais_fd = open(SINAL_FIFO, O_RDWR | O_NONBLOCK);

    if (sinais_fd == -1)
    {
        perror("\nNao foi possivel abrir o fifo dos SINAIS!\n");
        return -1;
    }

    printf("\nServidor do backend a correr...\n");

    printf("\n---Bem vindo Administrador---\n");

    if (pthread_create(&incrementaTempo, NULL, &aumentaTempo, &backend) != 0)
    {
        printf("\nFalha na criacao da thread.\n");
    }

    while (1)
    {
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(backend_fd, &read_fds);
        FD_SET(sinais_fd, &read_fds);

        int nfd = select(max(backend_fd, sinais_fd) + 1, &read_fds, NULL, NULL, &tv);

        if (nfd == 0)
        {
            printf("\n[AVISO] - Estou a espera de utilizadores...\n");
        }

        if (nfd == -1)
        {
            perror("\nErro no select!\n");
        }

        if (FD_ISSET(0, &read_fds))
        {
            interface(backend, u);
        }

        if (FD_ISSET(backend_fd, &read_fds))
        {

            if (read(backend_fd, &u, sizeof(u)) == -1)
            {
                printf("[ERRO] - Read - FIFO Backend\n");
                unlink(BACKEND_FIFO);
                unlink(SINAL_FIFO);
                exit(EXIT_FAILURE);
            } // ler os detalhes do utilizador

            if (u.isLoggedIn == 0)
            {
                sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);

                utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);

                if (utilizador_fd == -1)
                {
                    perror("\n[ERRO] Na abertura do fifo do utilizador!\n");
                    unlink(BACKEND_FIFO);
                    unlink(SINAL_FIFO);
                    exit(EXIT_FAILURE);
                }

                if (isUserValid(u.nome, u.pass) == 1)
                {
                    printf("\n[Login] - Utilizador [%s] e valido, a verificar...\n", u.nome);
                    u.isLoggedIn = 1;
                    write(utilizador_fd, &u, sizeof(u));
                    adicionaPessoa(&backend, u, maxUsers);
                }
                else if (isUserValid(u.nome, u.pass) == 0)
                {
                    printf("\n[Login] - Utilizador [%s] nao e valido\n", u.nome);
                    u.isLoggedIn = 0;
                    write(utilizador_fd, &u, sizeof(u));
                }
                else
                {
                    perror("\nErro\n");
                }
            }
            else if (u.isLoggedIn == 1)
            {
                read(backend_fd, &u, sizeof(u)); // apanha o comando

                if (strcmp(u.comando, " ") != 0)
                {
                    printf("\ncomando enviado pelo user [%s]: %s\n", u.nome, u.comando);
                    resetUserTime(&backend, u.pid);
                }

                // if (strcmp(u.comando, "reset") == 0)
                // {
                //     printf("\nDei Reset\n");
                //     //resetUserTime(&backend, u.pid);
                // }
                // printf("\nKEKW\n");
            }
        }
        if (FD_ISSET(sinais_fd, &read_fds))
        {
            printf("\nEntrei no SINAIS_FD\n");
            int aux;
            int size = read(sinais_fd, &aux, sizeof(aux));

            // if (size == sizeof(aux))
            // {
            //     printf("Entrei no Reset\n");
            //     resetUserTime(&backend, aux);
            // }
        }

        // okay o tempo de heartbeat ja passou?
        //  se sim, remove user
        //  se não caga
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
