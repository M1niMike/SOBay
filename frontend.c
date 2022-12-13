#include "frontend.h"
#include "backend.h"

int utilizador_fd, backend_fd;

void help()
{
    printf("---------------------\n");
    printf("COMANDOS\n");
    printf("[sell <nome-item> <categoria> <preco-base> <preco-compre-ja> <duracao>]  - \n");
    printf("[list] - \n");
    printf("[licat <nome-categoria>] - \n");
    printf("[lisel <username do vendedor] - \n");
    printf("[lival <preco-maximo>]  - \n");
    printf("[litime <hora-em-segundos>]  - \n");
    printf("[time] - \n");
    printf("[buy <id> <valor>]  - \n");
    printf("[cash] - \n");
    printf("[add <valor>] - \n");
    printf("[exit] - \n");
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

    // system("clear");
}

void interface()
{
    int nPalavras = 0;
    char cmd[TAM];
    printf("Comando: ");
    fgets(cmd, TAM, stdin);

    char *token = strtok(cmd, " \n"); // ler string até encontrar espaco e, por causa da ultima palavra, ate ao /n (porque nao tem espaco, tem /n)
    fflush(stdout);

    // fflush(stdin);

    // printf("\n%s", cmd);

    char primeiraPalavra[TAM];
    strcpy(primeiraPalavra, token);

    while (token != NULL)
    {
        nPalavras++;
        token = strtok(NULL, " ");
    }

    if (strcmp(primeiraPalavra, "sell") == 0)
    {
        if (nPalavras == 6)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [sell nomeItem categoria precoBase precoCompreJa duracao]\n");
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
    else if (strcmp(primeiraPalavra, "licat") == 0)
    {

        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [licat nomeCategoria]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "lisel") == 0)
    {

        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [lisel nomeVendedor]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "lival") == 0)
    {

        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [lival precoMaximo]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "litime") == 0)
    {

        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [litime prazo]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "time") == 0)
    {

        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [time]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "buy") == 0)
    {

        if (nPalavras == 3)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [buy id valor]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "cash") == 0)
    {

        if (nPalavras == 1)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [cash]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "add") == 0)
    {

        if (nPalavras == 2)
        {
            printf("\nA ser implementado...\n");
        }
        else
        {
            printf("\nInsira apenas [add valor]\n");
        }
    }
    else if (strcmp(primeiraPalavra, "exit") == 0)
    {

        if (nPalavras == 1)
        {
            sair();
        }
        else
        {
            printf("\nInsira apenas [exit]\n");
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

int main(int argc, char **argv)
{

    char cmd[TAM];
    char password[50];
    char username[50];
    ptruser user;
    fd_set read_fds;
    int nfd; // para o return do select
    struct timeval tv;

    user = malloc(sizeof(USER));
    if (user == NULL)
    {
        printf("\nErro na alocacao de memoria [USER] \n");
        free(user);
        return -1;
    }

    user->pid = getpid();

    if (argc == 3)
    {

        strcpy(user->nome, argv[1]);

        strcpy(user->pass, argv[2]);

        printf("\nNome do user: %s\n", user->nome);

        printf("\n\nSenha registada!\n");

        printf("\n\n Username registado\n");

        sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, user->pid);

        if (mkfifo(SELLER_BUYER_FIFO_COM, 0666) == -1){
            perror("\nFifo do utilizador nao aberto!\n");
            exit(EXIT_FAILURE);
        }


        utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);

        if (utilizador_fd == -1){
            perror("\n[ERRO] Na abertura do fifo do utilizador!\n");
            exit(EXIT_FAILURE);
        }

        backend_fd = open(BACKEND_FIFO, O_RDWR | O_NONBLOCK);

        if (backend_fd == -1){
            fprintf(stderr, "\nO server nao esta a correr!\n");
            unlink(SELLER_BUYER_FIFO_COM);
            exit(EXIT_FAILURE);
        }

        printf("\nUTILIZADOR: [%d] configurado!\n", getpid());

        printf("\n[%s]\n", user->nome);

        while (1)
        {
            tv.tv_sec = 50; //segundos
            tv.tv_usec = 0; // microsegundos. Isto significa que o timeout será de 50 segundos e 0 milisegundos. (50,0)
            
            FD_ZERO(&read_fds); // inicializar o set
            FD_SET(0, &read_fds); // adicionar o file descriptor ao "set"
            FD_SET(utilizador_fd, &read_fds); // adicionar o utilizador_fd ao "set"

            // ir buscar o return do select e validar 

            nfd = select(utilizador_fd + 1, &read_fds, NULL, NULL, &tv);

            if (nfd == -1){
                perror("\nErro no select! Nao tenho nada para ler...\n");
            }
            if (nfd == 0){
                printf("\nA escuta...\n");
            }

            //depois do return do select, verificar se os fd ainda estao dentro do set

            if (FD_ISSET(0, &read_fds)){
                //TODO
            }
            
            //interface(cmd);
        }
    }
    else if (argc < 3)
    {
        printf("\n\n Insira seu username e password apos './frontend'\n");
    }
    else
    {
        printf("\n\n Demasiados argumentos, insira apenas 3 [./frontend nome pass]\n");
        return -2;
    }

    return 0;

    // Comandos
}