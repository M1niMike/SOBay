#include "frontend.h"
#include "backend.h"
#include "users_lib.h"

int utilizador_fd, sinais_fd, backend_fd, utilizador_2_fd;
pid_t id = -1;
char *FUSERS;
char *FITEMS;
char *FPROMOTERS;
int HEARTBEAT;

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

char *execPromotor(char *name)
{
    int fd[2];
    pipe(fd);
    int i = 0;
    static char mensagem[TAM];

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
        return NULL;
    }

    if (id > 0)
    {
        read(fd[0], mensagem, sizeof(mensagem));
        close(fd[1]);
        printf("\n%s", mensagem);

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

        execl(name, name, NULL);

        exit(-1);
    }

    return mensagem;
}

void *promocoes(void *dados)
{ // thread para estar continuamente a executar promotor para os users verem promocoes
    ptrbackend backend = (ptrbackend)dados;

    printf("%d\n", backend->numPromoters);
    while (1)
    {
        for (int i = 0; i < backend->numPromoters; i++)
        {
            printf("%s", backend->promotores[i].nome);
            strcpy(backend->promotores[i].msg, execPromotor(backend->promotores[i].nome));
            printf("MENSAGEM DO PROMOTOR %s\n", backend->promotores[i].msg);
            sleep(30);
            continue;
        }
    }
}

void readPromotersFile(ptrbackend backend)
{

    FILE *f = fopen(FPROMOTERS, "rt");

    if (f == NULL)
    {
        printf("\nErro na abertura do ficheiro de texto PROMOTORES!\n");
        fclose(f);
        return;
    }

    while (fscanf(f, "%s", backend->promotores[backend->numPromoters].nome) != EOF)
    {
        backend->numPromoters++;
    }

    fclose(f);
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

void encerra(ptrbackend backend, int numUsers, int numItens, ptritem itens)
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

    FILE *f = fopen(FITEMS, "wt");

    if (!f)
    {
        printf("\nErro na abertura do ficheiro!\n");
        fclose(f);
        return;
    }

    for (int i = 0; i < backend->numItens; i++)
    {
        if (backend->itens[i].idItem != 0)
        {
            if (strcmp(backend->itens[i].highestBidder, "\0") == 0)
            {
                strcpy(backend->itens[i].highestBidder, "-");

                fprintf(f, "%d %s %s %d %d %d %s %s\n",
                        backend->itens[i].idItem,
                        backend->itens[i].nomeItem,
                        backend->itens[i].categoria,
                        backend->itens[i].valorAtual,
                        backend->itens[i].valorCompreJa,
                        backend->itens[i].duracao,
                        backend->itens[i].sellerName,
                        backend->itens[i].highestBidder);
            }
            else
            {
                fprintf(f, "%d %s %s %d %d %d %s %s\n",
                        backend->itens[i].idItem,
                        backend->itens[i].nomeItem,
                        backend->itens[i].categoria,
                        backend->itens[i].valorAtual,
                        backend->itens[i].valorCompreJa,
                        backend->itens[i].duracao,
                        backend->itens[i].sellerName,
                        backend->itens[i].highestBidder);
            }
        }
    }

    fclose(f);
    unlink(BACKEND_FIFO);
    unlink(SINAL_FIFO);
    kill(getpid(), SIGTERM);
    // encerrar os promotores tambem
}

void leFicheiroVendas(ptrbackend backend)
{
    FILE *f = fopen(FITEMS, "rt");

    if (f == NULL)
    {
        printf("\n[AVISO] - Nao foi possivel abrir ficheiro [%s]!\n", FITEMS);
        return;
    }

    while (fscanf(f, "%d %s %s %d %d %d %s %s",
                  &(backend->itens[backend->numItens].idItem),
                  backend->itens[backend->numItens].nomeItem,
                  backend->itens[backend->numItens].categoria,
                  &(backend->itens[backend->numItens].valorAtual),
                  &(backend->itens[backend->numItens].valorCompreJa),
                  &(backend->itens[backend->numItens].duracao),
                  backend->itens[backend->numItens].sellerName,
                  backend->itens[backend->numItens].highestBidder) != EOF)
    {
        backend->numItens++;
        // if
        // printf("\n%d\n", backend->numItens);
    }

    fclose(f);
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

void resetDadosItens(ptrbackend backend, ptritem item)
{
    item->idItem = 0;
    strcpy(item->nomeItem, "\0");
    strcpy(item->categoria, "\0");
    item->valorAtual = 0;
    item->valorCompreJa = 0;
    item->duracao = 0;
    strcpy(item->sellerName, "\0");
    // backend->numItens--;
}

void removeItensFromArray(ptrbackend backend, ITEM item)
{

    for (int i = 0; i < backend->numItens; i++)
    {
        if (backend->itens[i].idItem != 0) // e ele existir
        {
            printf("\n[AVISO] - Item %s removido do leilao!\n", backend->itens[i].nomeItem);
            resetDadosItens(backend, &backend->itens[i]);
            break;
        }
    }
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

void *aumentaTempoItem(void *dados)
{
    ptrbackend backend = (ptrbackend)dados; // dizer ao void *dados que ele é do tipo backend para deposi conseguir aceder ao backend

    while (1)
    {
        sleep(1);

        for (int i = 0; i < backend->numItens; i++)
        {
            if (backend->itens[i].idItem != 0 && backend->itens[i].duracao > 0)
            {
                backend->itens[i].duracao--;
                if (backend->itens[i].duracao == 0)
                {
                    if ((strcmp(backend->itens[i].highestBidder, "\0") == 0) || strcmp(backend->itens[i].highestBidder, "-") == 0)
                    {
                        removeItensFromArray(backend, backend->itens[i]);
                        printf("\nNinguem licitou neste item.\n");
                        break;
                    }
                    printf("\n%s foi o maior licitador do item %s!\n", backend->itens[i].highestBidder, backend->itens[i].nomeItem);

                    for (int j = 0; j < backend->numUsers; j++)
                    {
                        if (strcmp(backend->utilizadores[j].nome, backend->itens[i].highestBidder) == 0)
                        {
                            backend->utilizadores[j].saldo -= backend->itens[i].valorAtual;
                            updateUserBalance(backend->utilizadores[j].nome, backend->utilizadores[j].saldo);
                            saveUsersFile(FUSERS);
                            giveMoneyToSeller(backend);
                            break;
                        }
                    }
                }
            }
            else
            {
                continue;
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

void cmdListBackend(ptrbackend backend)
{
    int counter = 0;

    if (backend->numItens == 0)
    {
        printf("\n[AVISO] - nao ha itens na plataforma neste momento!\n");
    }

    if (counter == 0)
    {
        printf("\n----ITENS DO LEILAO----\n");
        counter = 1;
    }

    for (int i = 0; i < backend->numItens; i++)
    {
        if (backend->itens[i].idItem != 0) // se existir
        {
            printf("\nItem Id: %d", backend->itens[i].idItem);
            printf("\nNome item: %s", backend->itens[i].nomeItem);
            printf("\nNome do vendedor: %s", backend->itens[i].sellerName);
            printf("\nCategoria item: %s", backend->itens[i].categoria);
            printf("\nPreco item: %d", backend->itens[i].valorAtual);
            printf("\nPreco compre ja: %d", backend->itens[i].valorCompreJa);
            printf("\nDuracao: %d", backend->itens[i].duracao);
            printf("\nMaior licitador: %s\n", backend->itens[i].highestBidder);
        }
        else
        {
            if ((strcmp(backend->itens[i].nomeItem, "\0") == 0) && i + 1 == backend->numItens)
            {
                printf("\n[AVISO] - Nao ha itens na plataforma neste momento.\n");
            }
            else
            {
                continue;
            }
        }
    }
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
    int counter = 0;
    if (backend.numUsers == 0)
    {
        printf("\n[AVISO] - nao ha users logados na plataforma neste momento!\n");
    }

    if (counter == 0)
    {
        printf("\n----UTILIZADORES DO LEILAO----\n");
        counter = 1;
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
            if ((strcmp(backend.utilizadores[i].nome, "\0") == 0) && i + 1 == backend.numUsers)
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

void interface(BACKEND backend, USER user, ITEM item)
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
            cmdListBackend(&backend);
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
            encerra(&backend, backend.numUsers, backend.numItens, &item);
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

void verificaServidor()
{ // verifica se já existe um balcao ativo

    if (access(BACKEND_FIFO, F_OK) == 0)
    {
        printf("[AVISO] - ja tem o servidor ativo]\n");
        exit(EXIT_FAILURE);
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

void cmdAdd(COMUNICA comunica, USER user, ptrbackend backend, int saldoToAdd)
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

void giveMoneyToSeller(ptrbackend backend)
{
    for (int j = 0; j < backend->numUsers; j++)
    {
        for (int i = 0; i < backend->numItens; i++)
        {
            if (strcmp(backend->utilizadores[j].nome, backend->itens[i].sellerName) == 0)
            {
                backend->utilizadores[j].saldo += backend->itens[i].valorAtual;
                updateUserBalance(backend->utilizadores[j].nome, backend->utilizadores[j].saldo);
                saveUsersFile(FUSERS);
                removeItensFromArray(backend, backend->itens[i]);
                break;
            }
        }
    }
}
void giveMoneyToSellerCompreJa(ptrbackend backend)
{
    for (int j = 0; j < backend->numUsers; j++)
    {
        for (int i = 0; i < backend->numItens; i++)
        {
            if (strcmp(backend->utilizadores[j].nome, backend->itens[i].sellerName) == 0)
            {
                backend->utilizadores[j].saldo += backend->itens[i].valorCompreJa;
                updateUserBalance(backend->utilizadores[j].nome, backend->utilizadores[j].saldo);
                saveUsersFile(FUSERS);
                removeItensFromArray(backend, backend->itens[i]);
                break;
            }
        }
    }
}

void cmdBuy(USER user, int id, int valor, ptrbackend backend, ptrcomunica comunica)
{
    if (id <= 0 || id > 30)
    {
        printf("ID inexistente");
        comunica->retorno = -1;
        return;
    }

    for (int i = 0; i < backend->numItens; i++)
    {
        for (int j = 0; j < backend->numUsers; j++)
        {
            if ((strcmp(backend->utilizadores[j].nome, user.nome) == 0) && backend->utilizadores[j].saldo <= 0)
            {
                comunica->retorno = 3;
                break;
            }
            else if ((strcmp(backend->utilizadores[j].nome, user.nome) == 0) && backend->utilizadores[j].saldo > 0)
            {
                if (backend->itens[i].idItem == id) // se ele existir
                {
                    if (backend->itens[i].valorAtual <= valor && backend->itens[i].valorCompreJa != valor) // se o valor inserido for maior ou igual ao valor atual do item ele faz
                    {
                        backend->itens[i].valorAtual = valor;
                        strcpy(backend->itens[i].highestBidder, user.nome);
                        comunica->retorno = 2;
                        break;
                    }
                    else if (backend->itens[i].valorCompreJa > 0 && backend->itens[i].valorCompreJa == valor) // se o valor inserido for igual ao compreja e se o compre ja for mair que zero
                    {

                        if (strcmp(backend->utilizadores[j].nome, user.nome) == 0)
                        {

                            backend->utilizadores[j].saldo -= backend->itens[i].valorCompreJa;
                            updateUserBalance(backend->utilizadores[j].nome, backend->utilizadores[j].saldo);
                            saveUsersFile(FUSERS);
                            printf("%s foi comprado com o compreja por %s\n", backend->itens[i].nomeItem, backend->utilizadores[j].nome);
                            comunica->retorno = 1;
                            break;
                        }
                    }

                    else if (backend->itens[i].valorAtual > valor) // se for menor
                    {
                        comunica->retorno = 0; // codigo de retorno se correr mal (dinheiro insuficiente)
                        break;
                    }
                }
            }
        }
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
                strcpy(comunica.mensagem, "Item colocado a venda!");
                write(utilizador_fd, &comunica, sizeof(comunica));
                close(utilizador_fd);
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
                strcpy(comunica.mensagem, "Insira [sell] [nomeItem] [categoria] [precoAtual] [precoCompreJa | 0 se nao tiver] [duracao]]!");
                write(utilizador_fd, &comunica, sizeof(comunica));
                close(utilizador_fd);
            }
        }
        else if (strcmp(token, "list") == 0)
        {
            for (int i = 0; i < backend->numItens; i++)
            {
                comunica.itens[i] = backend->itens[i];
            }

            if (write(utilizador_fd, &comunica.itens, sizeof(comunica.itens)) < 0)
            {
                perror("Erro no write do list\n");
            }

            close(utilizador_fd);
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

            if (arg[1] != NULL && arg[2] != NULL)
            {
                comunica.retorno = 0;
                cmdBuy(u, atoi(arg[1]), atoi(arg[2]), backend, &comunica);

                if (comunica.retorno == 1)
                {
                    giveMoneyToSellerCompreJa(backend);
                    strcpy(comunica.mensagem, "Item comprado com sucesso!");
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }
                else if (comunica.retorno == 0)
                {
                    strcpy(comunica.mensagem, "Dinheiro insuficiente | Item ja vendido!");
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }
                else if (comunica.retorno == -1)
                {
                    strcpy(comunica.mensagem, "ID nao existe!");
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }
                else if (comunica.retorno == 2)
                {
                    strcpy(comunica.mensagem, "Licitacao feita!");
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }
                else if (comunica.retorno == 3)
                {
                    strcpy(comunica.mensagem, "Saldo negativo - use o comando add!");
                    write(utilizador_fd, &comunica, sizeof(comunica));
                    close(utilizador_fd);
                }
            }
            else
            {
                printf("\n[AVISO]- %s enviou um comando incompleto\n", u.nome);
                strcpy(comunica.mensagem, "Insira [buy] [id] [valor]");
                write(utilizador_fd, &comunica, sizeof(comunica));
                close(utilizador_fd);
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
                cmdAdd(comunica, u, backend, atoi(arg[1]));
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
    int maxPromotores = 10;
    backend.numUsers = 0;
    backend.time = 0;
    backend.numItens = 0;
    pthread_t incrementaTempo;
    pthread_t recebeSinal;
    pthread_t tempoItem;
    pthread_t promocoesThread;

    u.isLoggedIn = 0;

    verificaServidor();

    getFilePaths();

    numUsersInTextFile = loadUsersFile(FUSERS); // maxPromotersa os users do ficheiro de texto.

    backend.utilizadores = malloc(maxUsers * sizeof(*backend.utilizadores));
    backend.itens = malloc(maxItens * sizeof(*backend.itens));
    backend.promotores = malloc(maxPromotores * sizeof(*backend.promotores));
    comunica.itens = malloc(maxItens * sizeof(*comunica.itens));

    leFicheiroVendas(&backend);

    readPromotersFile(&backend);

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
        printf("\n[ERRO] - Falha na criacao da thread AUMENTA TIME\n");
    }

    if (pthread_create(&tempoItem, NULL, &aumentaTempoItem, &backend) != 0)
    {
        printf("\n[ERRO] - Falha na criacao da thread TEMPO ITEM.\n");
    }

    if (pthread_create(&promocoesThread, NULL, &promocoes, &backend) != 0)
    {
        printf("\n[ERRO] - Falha na criacao da thread PROMOTORES");
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
            interface(backend, u, it);
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

            if (u.isLoggedIn == 0)
            {

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
                sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, u.pid);
                utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_WRONLY | O_NONBLOCK);
                printf("\n[%s] enviou o comando: %s\n", u.nome, u.comando);

                utilizadorCmd(&backend, u, it, comunica);
                close(utilizador_fd);
            }
        }
        if (FD_ISSET(sinais_fd, &read_fds))
        {
            int aux;
            int size = read(sinais_fd, &aux, sizeof(aux));

            if (size == sizeof(aux))
            {
                for (int i = 0; i < backend.numUsers; i++)
                {
                    if (aux == backend.utilizadores[i].pid)
                    {
                        printf("HEARTBEAT do %s\n", backend.utilizadores[i].nome);
                    }
                }
                resetUserTime(&backend, aux);
            }
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
