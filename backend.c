#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

int utilizador_fd, sinais_fd, backend_fd, utilizador_2_fd;
pid_t id = -1;
char *FUSERS;
char *FITEMS;
char *FPROMOTERS;
int HEARTBEAT;
char *FPROMOTERS;

int max(int a, int b)
{
    return (a > b) ? a : b;
}

void getFilePaths()
{

    FITEMS = getenv("FITEMS");
    FUSERS = getenv("FUSERS");
    HEARTBEAT = atoi(getenv("HEARTBEAT"));
    FPROMOTERS = getenv("FPROMOTERS");

    if (FUSERS == NULL)
    {
        printf("\n[AVISO] - Por favor insira uma var de ambiente FUSERS para ler o ficheiro de texto que pretende!\n");
        return;
    }
    else if (FITEMS == NULL)
    {
        printf("\n[AVISO] - Por favor insira uma var de ambiente FITEMS para ler o ficheiro de texto que pretende!\n");
        return;
    }
    else if (FPROMOTERS == NULL)
    {
        printf("\n[AVISO] - Por favor insira uma var de ambiente FPROMOTERS para ler o ficheiro de promotores!");
        return;
    }
    else if (HEARTBEAT == 0)
    {
        printf("\n[AVISO] - Por favor insira uma var de ambiente para o HEARTBEAT!\n");
        return;
    }

    // printf("%d", HEARTBEAT);
}

void execPromotor(char *promoterName)
{
    int fd[2];
    char msgVolta[TAM];
    pipe(fd);
    char resposta[TAM];
    int i = 0;

    // para ler os promotores:
    // criar uma função para ler o ficheiro de texto dos promoters
    // depois ler a quantidade de promoters que existe no ficheiro de texto
    // envolver o fork() num for com a quantidade de promoters que existem
    // executar no execl(promotor[i], promotor[i], NULL)

    // strcat(nomePromotores[i].nome, "/promotores/"); //arg1 + arg2

    id = fork();

    if (id == -1)
    {
        printf("\n[ERRO] - Falha na execucao do fork()");
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

        execl(promoterName, promoterName, NULL);

        exit(-1);
    }
}

void readPromotersFile(char *fileName)
{

    int maxPromoters = 0;
    ptrpromotores nomePromotores;

    nomePromotores = malloc(10 * sizeof(PROMOTORES));

    FILE *f = fopen(fileName, "rt");

    if (f == NULL)
    {
        printf("\nErro na abertura do ficheiro de texto PROMOTORES!\n");
        fclose(f);
        return;
    }

    if (nomePromotores == NULL)
    {
        perror("\nErro na alocação de memória para os promotores.\n");
        free(nomePromotores);
        return;
    }

    while (fscanf(f, "%s", nomePromotores[maxPromoters].nome) != EOF && maxPromoters <= 10)
    {
        // printf("\nNome do promotor: %s", nomePromotores[maxPromoters].nome);
        execPromotor(nomePromotores[maxPromoters].nome);
        maxPromoters++;
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
        printf("\n[AVISO] - Nao consigo adicionar mais users. (CHEGOU AO LIMITE)\n");
        return;
    }

    for (int i = 0; i < maxUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, u.nome) == 0)
        {
            printf("\n[AVISO] - Utilizador [%s] ja estava logado\n", backend->utilizadores[i].nome);
            kill(u.pid, SIGQUIT); // temp
            break;
        }
    }

    backend->utilizadores[backend->numUsers] = u;
    backend->utilizadores[backend->numUsers].saldo = getUserBalance(backend->utilizadores[backend->numUsers].nome);
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
                printf("\n[AVISO] - %s removido por inatividade\n", backend->utilizadores[i].nome);
                resetDados(backend, &backend->utilizadores[i]);
                // backend->numUsers--;
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
        // para maxPromotersar o tempo, mandá-lo dormir 1 segundo e incrementar a var do tempo
        sleep(1);
        pdados->time++;
        for (int i = 0; i < pdados->numUsers; i++)
        {
            if (pdados->utilizadores[i].pid != 0)
            { // o utilizador nesta posição tem um pid != de 0, ou seja, existe
                pdados->utilizadores[i].tempoLogged++;
                if (pdados->utilizadores[i].tempoLogged >= HEARTBEAT + 1)
                {
                    removePessoaFromArray(pdados, pdados->utilizadores[i]);
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

void cmdSell(char *nomeItem, char *categoria, int precoBase, int compreJa, int duracao, ptrbackend backend, USER user, int maxItens)
{

    if (backend->numItens == maxItens)
    {
        printf("\nNao foi possivel adicionar mais itens à estrutura!");
        return;
    } // verifica que nao chegou ao maxItens que é 30, se chegou nem faz o for

    backend->itens[backend->numItens].idItem = backend->numItens + 1; // igualar o id a posicao do loop
    strcpy(backend->itens[backend->numItens].nomeItem, nomeItem);
    strcpy(backend->itens[backend->numItens].categoria, categoria);
    backend->itens[backend->numItens].valorAtual = precoBase;
    backend->itens[backend->numItens].valorCompreJa = compreJa;
    backend->itens[backend->numItens].duracao = duracao;
    strcpy(backend->itens[backend->numItens].sellerName, user.nome);

    backend->numItens++;

} // testar pode nao estar certo e probs nao esta

void cmdList(BACKEND backend)
{
    if (backend.numItens == 0)
    {
        printf("\n[AVISO] - nao ha itens na plataforma neste momento!\n");
    }

    for (int i = 0; i < backend.numItens; i++)
    {
        printf("\nItem Id: %d", backend.itens[i].idItem);
        printf("\nNome item: %s", backend.itens[i].nomeItem);
        printf("\nNome do vendedor: %s", backend.itens[i].sellerName);
        printf("\nCategoria item: %s", backend.itens[i].categoria);
        printf("\nPreco item: %d", backend.itens[i].valorAtual);
        printf("\nPreco compre ja: %d", backend.itens[i].valorCompreJa);
        printf("\nDuracao: %d\n", backend.itens[i].duracao);
    }

    /*Apos a remoção*/

    // for (int i = 0; i < backend.numItens; i++)
    // {
    //     if (backend.utilizadores[i].pid != 0)
    //     {
    //         printf("\n[UTILIZADORES] - Nome: %s - Pass: %s - Saldo: %d - PID: %d\n", backend.utilizadores[i].nome, backend.utilizadores[i].pass, backend.utilizadores[i].saldo, backend.utilizadores[i].pid);
    //     }
    //     else
    //     {
    //         if (backend.utilizadores[i].nome == " ")
    //         {
    //             printf("\n[AVISO] - Nao ha users logados na plataforma neste momento.\n");
    //         }
    //         else
    //         {
    //             continue;
    //         }
    //     }
    // }
}

void cmdKickBackend(ptrbackend backend, char *nome)
{

    for (int i = 0; i < backend->numUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, nome) == 0) // se o nome for igual
        {
            if (backend->utilizadores[i].pid != 0) // e ele existir
            {

                kill(backend->utilizadores[i].pid, SIGUSR1);
                resetDados(backend, &backend->utilizadores[i]);
                // backend->numUsers--;
                printf("[AVISO] - %s foi removido", nome);
                break;
            }
        }
    }
}

void cmdUsersBackend(BACKEND backend)
{

    if (backend.numUsers == 0)
    {
        printf("\n[AVISO] - nao ha users logados na plataforma neste momento!\n");
    }

    for (int i = 0; i < backend.numUsers; i++)
    {
        if (backend.utilizadores[i].pid != 0)
        {
            printf("\nNome: %s", backend.utilizadores[i].nome);
            printf("\nPass: %s", backend.utilizadores[i].pass);
            printf("\nSaldo: %d\n", backend.utilizadores[i].saldo);
        }
        else
        {
            if (backend.utilizadores[i].nome == " ")
            {
                printf("\n[AVISO] - Nao ha users logados na plataforma neste momento.\n");
            }
            else
            {
                continue;
            }
        }
    }
    // nao esquecer de corrigir a cena do backend->numUsers-- quando acabarmos o trabalho. o espaço de memoria fica la, tratar disso!
    // print a avisa que nao tem users.
}

// void cmdTime(BACKEND backend){
//     backend.time

//     write(utilizador_fd, &backend, sizeof(BACKEND));
// }

void interface(BACKEND backend, USER user)
{
    char cmd[TAM];
    char *token;
    char *arg[2];

    fgets(cmd, TAM, stdin);

    token = strtok(cmd, " \n");

    while (token != NULL)
    {
        if (strcmp(token, "users") == 0)
        {
            cmdUsersBackend(backend);
        }
        else if (strcmp(token, "list") == 0)
        {
            cmdList(backend);
        }
        else if (strcmp(token, "kick") == 0)
        {
            arg[1] = strtok(NULL, " \n");

            if (arg[1] != NULL)
            {
                cmdKickBackend(&backend, arg[1]);
            }
            else
            {
                printf("\n[AVISO] - Insira apenas [kick] [nomeUser]\n");
            }
        }
        else if (strcmp(token, "prom") == 0)
        {
            printf("\nA ser implementado...\n");
        }
        else if (strcmp(token, "reprom") == 0)
        {
            printf("\nA ser implementado...\n");
        }
        else if (strcmp(token, "cancel") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO] - Insira apenas [cancel] [nomePromotor]\n");
            }
        }
        else if (strcmp(token, "close") == 0)
        {
            encerra(&backend, backend.numUsers);
        }
        else if (strcmp(token, "help") == 0)
        {
            help();
        }
        else if (strcmp(token, "clear") == 0)
        {
            clear();
        }
        else
        {
            printf("\n[AVISO] - Comando invalido!\n");
        }
        token = strtok(NULL, " ");
    }
}

void cmdAddUtilizador(COMUNICA comunica, USER user, ptrbackend backend, int saldoToAdd)
{
    for (int i = 0; i < backend->numUsers; i++)
    {
        if (strcmp(backend->utilizadores[i].nome, user.nome) == 0)
        {
            backend->utilizadores[i].saldo += saldoToAdd;
            comunica.saldo = backend->utilizadores[i].saldo;
            updateUserBalance(backend->utilizadores[i].nome, backend->utilizadores[i].saldo);
            saveUsersFile(FUSERS);
            break;
        }
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
        printf("\n[AVISO] - Erro na alocacao de memoria\n");
        free(item);
        return NULL;
    }

    if (f == NULL)
    {
        printf("\n[AVISO] - Nao foi possivel abrir ficheiro [%s]!\n", FITEMS);
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
        printf("[AVISO] - ja tem o servidor ativo]\n");
        exit(EXIT_FAILURE);
    }
}

void utilizadorCmd(ptrbackend backend, USER u, ITEM it, COMUNICA comunica)
{
    char *token;
    char *arg[5];

    token = strtok(u.comando, " \n");

    while (token != NULL)
    {
        if (strcmp(token, "sell") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");
            arg[3] = strtok(NULL, " \n");
            arg[4] = strtok(NULL, " \n");
            arg[5] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL && arg[3] != NULL && arg[4] != NULL && arg[5] != NULL)
            {
                cmdSell(arg[1], arg[2], atoi(arg[3]), atoi(arg[4]), atoi(arg[5]), backend, u, 30);
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "list") == 0)
        {
            printf("\nRecebi list\n");
        }
        else if (strcmp(token, "licat") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "lisel") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "lival") == 0)
        {

            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "litime") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "time") == 0)
        {
            comunica.timeBackend = backend->time;
            write(utilizador_fd, &comunica, sizeof(comunica));
            close(utilizador_fd);
        }
        else if (strcmp(token, "buy") == 0)
        {
            arg[1] = strtok(NULL, " \n");
            arg[2] = strtok(NULL, " \n");
            arg[3] = strtok(NULL, " \n");

            if (arg[1] != NULL && arg[2] != NULL && arg[3] != NULL)
            {
                printf("\nA ser implementado...\n");
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
            }
        }
        else if (strcmp(token, "cash") == 0)
        {

            for (int i = 0; i < backend->numUsers; i++)
            {
                if (strcmp(u.nome, backend->utilizadores[i].nome) == 0)
                {
                    comunica.saldo = backend->utilizadores[i].saldo;
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }

                // close(utilizador_fd);
            }
        }
        else if (strcmp(token, "add") == 0)
        {

            arg[1] = strtok(NULL, " \n");

            if (arg[1] != NULL)
            {
                cmdAddUtilizador(comunica, u, backend, atoi(arg[1]));
                strcpy(comunica.mensagem, "Saldo atualizado com sucesso");
                write(utilizador_fd, &comunica, sizeof(comunica));
                close(utilizador_fd);
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
                strcpy(comunica.mensagem, "Insira [add] [valor]");
                write(utilizador_fd, &comunica, sizeof(comunica));
                close(utilizador_fd);
            }
        }
        else if (strcmp(token, "exit") == 0)
        {
            printf("\n%s encerrou o programa\n", u.nome);
        }
        token = strtok(NULL, " ");
    }
}

int main(int argc, char **argv)
{
    int i;
    // ptrcomunica comunica = malloc(sizeof(COMUNICA));
    COMUNICA comunica;
    BACKEND backend;
    USER u;
    ITEM it;
    struct timeval tv;
    fd_set read_fds;
    fflush(stdout);
    int maxUsers = 20;
    int numUsersInTextFile;
    int maxItens = 30;
    int status = 0;
    backend.numUsers = 0;
    backend.time = 0;
    pthread_t incrementaTempo;
    pthread_t recebeSinal;

    u.isLoggedIn = 0;

    verificaServidor();

    getFilePaths();

    numUsersInTextFile = loadUsersFile(FUSERS); // maxPromotersa os users do ficheiro de texto.

    backend.utilizadores = malloc(maxUsers * sizeof(*backend.utilizadores));
    backend.itens = malloc(maxItens * sizeof(*backend.itens));
    // backend.numPromoters = malloc(10 * sizeof(*backend.));

    if (mkfifo(BACKEND_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("[ERRO] - Criacao - FIFO BACKEND\n");
            exit(EXIT_FAILURE);
        }
    }

    if (mkfifo(SINAL_FIFO, 0666) == -1)
    {
        if (errno != EEXIST)
        {
            printf("[ERRO] - Criacao - FIFO SINAIS\n");
            exit(EXIT_FAILURE);
        }
    }

    backend_fd = open(BACKEND_FIFO, O_RDWR | O_NONBLOCK);

    if (backend_fd == -1)
    {
        perror("\n[ERRO] - Nao foi possivel abrir o fifo do BACKEND!\n");
        return -1;
    }

    sinais_fd = open(SINAL_FIFO, O_RDWR | O_NONBLOCK);

    if (sinais_fd == -1)
    {
        perror("\n[ERRO] - Nao foi possivel abrir o fifo dos SINAIS!\n");
        return -1;
    }

    printf("\nBACKEND A CORRER...\n");

    printf("\n---Bem vindo Administrador---\n");

    if (pthread_create(&incrementaTempo, NULL, &aumentaTempo, &backend) != 0)
    {
        printf("\n[ERRO] - Falha na criacao da thread.\n");
    }

    while (1)
    {

        tv.tv_sec = 20;
        tv.tv_usec = 0;

        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(backend_fd, &read_fds);
        FD_SET(sinais_fd, &read_fds);

        int nfd = select(max(backend_fd, sinais_fd) + 1, &read_fds, NULL, NULL, &tv);

        if (nfd == 0)
        {
            printf("\n[AVISO] - Estou a espera de utilizadores\n"); // corrigir o num users
        }

        if (nfd == -1)
        {
            perror("\n[ERRO] - Erro no select!\n");
        }

        if (FD_ISSET(0, &read_fds))
        {
            interface(backend, u);
        }

        if (FD_ISSET(backend_fd, &read_fds))
        {

            if (read(backend_fd, &u, sizeof(u)) == -1)
            {
                printf("[ERRO] - Read - FIFO Backend(2)\n");
                unlink(BACKEND_FIFO);
                unlink(SINAL_FIFO);
                exit(EXIT_FAILURE);
            } // ler os detalhes do utilizador

            // sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);

            // utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);

            if (u.isLoggedIn == 0)
            {

                // if (utilizador_fd == -1)
                // {
                //     perror("\n[ERRO] - Na abertura do fifo do utilizador!\n");
                //     unlink(BACKEND_FIFO);
                //     unlink(SINAL_FIFO);
                //     exit(EXIT_FAILURE);
                // }

                if (isUserValid(u.nome, u.pass) == 1)
                {
                    printf("\n[LOGIN] - Utilizador [%s] e valido, a verificar...\n", u.nome);
                    u.isLoggedIn = 1;
                    sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);
                    utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_WRONLY | O_NONBLOCK);
                    write(utilizador_fd, &u, sizeof(u));
                    adicionaPessoa(&backend, u, maxUsers);
                    close(utilizador_fd);
                }
                else if (isUserValid(u.nome, u.pass) == 0)
                {
                    printf("\n[LOGIN] - Utilizador [%s] nao e valido\n", u.nome);
                    u.isLoggedIn = 0;
                    sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);
                    utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_WRONLY | O_NONBLOCK);
                    write(utilizador_fd, &u, sizeof(u));
                    close(utilizador_fd);
                }
                else
                {
                    perror("\nErro\n");
                }
            }
            else if (u.isLoggedIn == 1)
            {
                utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_WRONLY | O_NONBLOCK);
                printf("\n[%s] enviou o comando: %s\n", u.nome, u.comando);

                utilizadorCmd(&backend, u, it, comunica);
                close(utilizador_fd);
            }
        }
        if (FD_ISSET(sinais_fd, &read_fds))
        {
            printf("\n[AVISO] - Entrei no SINAIS_FD\n");
            int aux;
            int size = read(sinais_fd, &aux, sizeof(aux));

            if (size == sizeof(aux))
            {
                printf("HEARTBEAT %s\n", u.nome); // printf corrigir, so esta printando o ultimo
                resetUserTime(&backend, aux);
            }
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

/*OLD INTERFACE*/

// void interface(BACKEND backend, USER user)
// {
//     char cmd[TAM];
//     char primeiraPalavra[TAM];
//     char arg1[TAM];

//     int nPalavras = 0; // assumir que nao começamos com palavra nenhuma

//     fgets(cmd, sizeof(cmd), stdin);
//     cmd[strcspn(cmd, "\n")] = '\0';

//     char *tokenfw = strtok(cmd, " \n"); // ate ao espaco e /n por causa da ultima palavra
//                                         // fflush(stdout);

//     strcpy(primeiraPalavra, tokenfw);
//     while (tokenfw != NULL)
//     {
//         nPalavras++;
//         tokenfw = strtok(NULL, " ");
//     }

//     //

//     if (strcmp(primeiraPalavra, "users") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             cmdUsers(backend);
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [users]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "list") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [list]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "kick") == 0)
//     {

//         if (nPalavras == 2)
//         {
//             CmdRemovePessoaFromArray(&backend, arg1);
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [kick] [nomeUser]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "prom") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [prom]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "reprom") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [reprom]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "cancel") == 0)
//     {
//         if (nPalavras == 2)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [cancel] [nomePromotor]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "close") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             encerra(&backend, backend.numUsers);
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [close]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "help") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             help();
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [help]\n");
//         }
//     }
//     else if (strcmp(primeiraPalavra, "clear") == 0)
//     {
//         if (nPalavras == 1)
//         {
//             clear();
//         }
//         else
//         {
//             printf("\n[AVISO] - Insira apenas [clear]\n");
//         }
//     }
//     else
//     {
//         printf("\n[AVISO] - Comando invalido!\n");
//     }
// }