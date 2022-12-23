#include "frontend.h"
#include "backend.h"

int utilizador_fd, backend_fd, sinal_fd;

void sigQuit_handler()
{
    printf("\n[AVISO] - Usuario já logado\n");
    unlink(SELLER_BUYER_FIFO_COM);
    exit(EXIT_SUCCESS);
}

void sigTerm_handler()
{
    printf("\n[AVISO] - O servidor foi encerrado\n");
    unlink(SELLER_BUYER_FIFO_COM);
    exit(EXIT_SUCCESS);
}

void sigUser1_handler()
{
    printf("\n[AVISO] - Foi kickado\n");
    unlink(SELLER_BUYER_FIFO_COM);
    exit(EXIT_SUCCESS);
}

void *mandaSinal(void *dados)
{
    ptruser pdados = (ptruser)dados;

    int pid = pdados->pid;

    int heartBeatTime = atoi(getenv("HEARTBEAT"));
    while (1)
    {
        sleep(heartBeatTime);
        sinal_fd = open(SINAL_FIFO, O_RDWR | O_NONBLOCK);

        if (sinal_fd == -1)
        {
            perror("\nErro na abertura do fifo do sinal.\n");
            unlink(SINAL_FIFO);
            unlink(SELLER_BUYER_FIFO_COM);
            exit(EXIT_FAILURE);
        }
        if (write(sinal_fd, &pid, sizeof(pid)) < 0)
        {
            perror("\nErro write heartbeat message to thread\n");
        }

        close(sinal_fd);
    }
}


void help()
{
    printf("\n---------------------\n");
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
    unlink(SELLER_BUYER_FIFO_COM);
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

int main(int argc, char **argv)
{
    char password[50];
    char username[50];
    fd_set read_fds;
    int nfd;           // para o return do select
    struct timeval tv; // timeout do selects
    fflush(stdout);
    char mensagem[TAM];
    int res;
    int reader = 0;
    pthread_t heartbeat_thread; // mandar o sinal para o backend

    USER user;
    ITEM item;
    COMUNICA comunica;
    int cont = 0;

    user.isLoggedIn = 0;
    user.tempoLogged = 0;

    signal(SIGQUIT, sigQuit_handler);
    signal(SIGTERM, sigTerm_handler);
    signal(SIGUSR1, sigUser1_handler);

    user.pid = getpid();

    sprintf(SELLER_BUYER_FIFO_COM, SELLER_BUYER_FIFO, user.pid);

    if (mkfifo(SELLER_BUYER_FIFO_COM, 0666) == -1)
    {
        perror("\nFifo do utilizador nao criado!\n");
        exit(EXIT_FAILURE);
    }

    utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);

    if (utilizador_fd == -1)
    {
        perror("\n[ERRO] Na abertura do fifo do utilizador!\n");
        exit(EXIT_FAILURE);
    }

    backend_fd = open(BACKEND_FIFO, O_RDWR | O_NONBLOCK);

    if (backend_fd == -1)
    {
        fprintf(stderr, "\nO server nao esta a correr!\n");
        unlink(SELLER_BUYER_FIFO_COM);
        exit(EXIT_FAILURE);
    }

    if (argc == 3)
    {
        strcpy(user.nome, argv[1]);

        strcpy(user.pass, argv[2]);

        printf("\nNome do user: %s\n", user.nome);

        printf("\nSenha registada!\n");

        printf("\nUsername registado\n");

        if (write(backend_fd, &user, sizeof(user)) == -1)
        {
            printf("[ERRO] Write - FIFO Backend\n");
            unlink(SELLER_BUYER_FIFO_COM);
            exit(EXIT_FAILURE);
        } // envia os detalhes do user

        // receber se o login correu bem ou nao
        if (pthread_create(&heartbeat_thread, NULL, &mandaSinal, &user) != 0)
        {
            perror("\nErro na thread\n");
        }

        while (1)
        {
            tv.tv_sec = 20; // segundos
            tv.tv_usec = 0; // microsegundos. Isto significa que o timeout será de 50 segundos e 0 milisegundos. (50,0)

            FD_ZERO(&read_fds);               // inicializar o set
            FD_SET(0, &read_fds);             // adicionar o file descriptor do teclado (stdin) ao "set"
            FD_SET(utilizador_fd, &read_fds); // adicionar o utilizador_fd ao "set"

            // ir buscar o return do select e validar

            nfd = select(utilizador_fd + 1, &read_fds, NULL, NULL, &tv);

            if (nfd == -1)
            {
                perror("\nErro no select! Nao tenho nada para ler...\n");
            }
            if (nfd == 0)
            {
                printf("\nEspera de comandos ou de resposta do backend\n");
            }

            // depois do return do select, verificar se os fd ainda estao dentro do set

            if (FD_ISSET(0, &read_fds)) // Teclado
            {
                backend_fd = open(BACKEND_FIFO, O_WRONLY | O_NONBLOCK);
                fgets(user.comando, sizeof(user.comando), stdin);
                user.comando[strcspn(user.comando, "\n")] = 0;
                write(backend_fd, &user, sizeof(user));

                if (strcmp(user.comando, "exit") == 0)
                {
                    sair();
                }

                close(backend_fd);
            }
            if (FD_ISSET(utilizador_fd, &read_fds)) // user fd
            {
                if (cont == 0)
                {
                    printf("Voltei antes do user.isLoggedIn\n");
                    printf("\nBem vindo [%s]\n", user.nome);
                    read(utilizador_fd, &user, sizeof(user));

                    if (user.isLoggedIn == 0)
                    {
                        utilizador_fd = open(SELLER_BUYER_FIFO_COM, O_RDWR | O_NONBLOCK);
                        printf("\nInsira outra vez o nome: ");
                        fgets(user.nome, sizeof(user.nome), stdin);
                        user.nome[strcspn(user.nome, "\n")] = 0;

                        printf("\nInsira novamente a passe: ");
                        fgets(user.pass, sizeof(user.pass), stdin);
                        user.pass[strcspn(user.pass, "\n")] = 0;

                        write(backend_fd, &user, sizeof(user)); // volta a enviar os detalhes para o backend
                    }
                    cont = 1;
                }
                else if (cont == 1 && user.isLoggedIn == 1)
                {
                    close(backend_fd);

                    char *token; // ler string até encontrar espaco e, por causa da ultima palavra, ate ao /n (porque nao tem espaco, tem /n)
                    char *arg[5];
                    token = strtok(user.comando, " \n");

                    while (token != NULL)
                    {

                        if (strcmp(token, "time") == 0)
                        {
                            read(utilizador_fd, &comunica, sizeof(comunica));
                            printf("\nTempo da plataforma atual: (%d) segundos\n", comunica.timeBackend);
                        }
                        else if (strcmp(token, "cash") == 0)
                        {
                            read(utilizador_fd, &comunica, sizeof(comunica));
                            user.saldo = comunica.saldo;
                            printf("\nSeu saldo atual: (%d)\n", comunica.saldo);
                        }
                        else if (strcmp(token, "add") == 0)
                        {
                            arg[1] = strtok(NULL, " \n");

                            if (arg[1] != NULL)
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                            else
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                        }
                        else if (strcmp(token, "buy") == 0)
                        {
                            arg[1] = strtok(NULL, " \n");
                            arg[2] = strtok(NULL, " \n");

                            if (arg[1] != NULL && arg[2] != NULL)
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                            else
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                        }
                        else if (strcmp(token, "sell") == 0)
                        {
                            arg[1] = strtok(NULL, " \n");
                            arg[2] = strtok(NULL, " \n");
                            arg[3] = strtok(NULL, " \n");
                            arg[4] = strtok(NULL, " \n");
                            arg[5] = strtok(NULL, " \n");

                            if (arg[1] != NULL && arg[2] != NULL && arg[3] != NULL && arg[4] != NULL && arg[5] != NULL)
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                            else
                            {
                                read(utilizador_fd, &comunica, sizeof(comunica));
                                printf("\n%s\n", comunica.mensagem);
                            }
                        }
                        else if (strcmp(token, "list") == 0)
                        {
                            read(utilizador_fd, &comunica.itens, sizeof(comunica.itens));
                            
                        }
                        else
                        {
                            printf("\nComando invalido! Tente novamente!\n");
                        }

                        token = strtok(NULL, " ");
                    }
                }
            }
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
}

/*OLD INTERFACE*/

// void interface(USER user, ITEM item)
// {

//     char *token; // ler string até encontrar espaco e, por causa da ultima palavra, ate ao /n (porque nao tem espaco, tem /n)
//     char *arg[5];
//     token = strtok(user.comando, " \n");

//     while (token != NULL)
//     {
//         if (strcmp(token, "sell") == 0)
//         {
//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");
//             arg[3] = strtok(NULL, " \n");
//             arg[4] = strtok(NULL, " \n");
//             arg[5] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL && arg[3] != NULL && arg[4] != NULL && arg[5] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [sell nomeItem categoria precoBase precoCompreJa duracao]\n");
//             }
//         }
//         else if (strcmp(token, "list") == 0)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else if (strcmp(token, "licat") == 0)
//         {
//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [licat nomeCategoria]\n");
//             }
//         }
//         else if (strcmp(token, "lisel") == 0)
//         {
//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [lisel nomeVendedor]\n");
//             }
//         }
//         else if (strcmp(token, "lival") == 0)
//         {

//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [lival precoMaximo]\n");
//             }
//         }
//         else if (strcmp(token, "litime") == 0)
//         {
//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [litime prazo]\n");
//             }
//         }
//         else if (strcmp(token, "time") == 0)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else if (strcmp(token, "buy") == 0)
//         {
//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");
//             arg[3] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2] != NULL && arg[3] != NULL)
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [buy id valor]\n");
//             }
//         }
//         else if (strcmp(token, "cash") == 0)
//         {
//             printf("\nA ser implementado...\n");
//         }
//         else if (strcmp(token, "add") == 0)
//         {

//             arg[1] = strtok(NULL, " \n");
//             arg[2] = strtok(NULL, " \n");

//             if (arg[1] != NULL && arg[2])
//             {
//                 printf("\nA ser implementado...\n");
//             }
//             else
//             {
//                 printf("\nInsira apenas [add valor]\n");
//             }
//         }
//         else if (strcmp(token, "exit") == 0)
//         {
//             sair();
//         }
//         else if (strcmp(token, "help") == 0)
//         {
//             help();
//         }
//         else if (strcmp(token, "clear") == 0)
//         {
//             clear();
//         }
//         else
//         {
//             printf("\nComando invalido!\n");
//         }
//         token = strtok(NULL, " ");
//     }
// }