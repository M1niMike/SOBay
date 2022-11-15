#include "frontend.h"
#include "backend.h"

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

    user = malloc(sizeof(USER));
    if (user == NULL)
    {
        printf("\nErro na alocacao de memoria [USER] \n");
        free(user);
        return -1;
    }

    // varAmb();
    // lePipes();

    if (argc == 3)
    {

        strcpy(user->nome, argv[1]);

        strcpy(user->pass, argv[2]);

        printf("\nNome do user: %s\n", user->nome);

        printf("\n\nSenha registada!\n");

        printf("\n\n Username registado\n");

        while (1)
        {
            interface(cmd);
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