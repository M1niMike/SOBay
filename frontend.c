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
    scanf("%s", cmd);
    char *token = strtok(cmd, " \n"); // ler string até encontrar espaco e, por causa da ultima palavra, ate ao /n (porque nao tem espaco, tem /n)
    fflush(stdout);
    fflush(stdin);

    // printf("\n%s", cmd);

    // char primeiraPalavra[TAM];
    // strcpy(primeiraPalavra, token);

    if (strcmp(cmd, "sell") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 6)
        {
            printf("\nPor favor insira o seguinte:  <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\n");
        }
        else if (nPalavras == 6)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "list") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "licat") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira a categoria.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "lisel") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o nome do vendedor.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "lival") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o preco maximo do artigo.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "litime") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o tempo restante (em segundos).\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "time") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "buy") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 3)
        {
            printf("\nPor favor insira o seguinte: <id> <valor>\n");
        }
        else if (nPalavras == 3)
        {
            printf("\nA ser implementado\n");
        }
    }
    else if (strcmp(cmd, "cash") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "add") == 0)
    {
        while (token != NULL)
        {
            nPalavras++;
            token = strtok(NULL, " ");
        }

        if (nPalavras < 2)
        {
            printf("\nPor favor insira o montante que deseja adicionar.\n");
        }
        else if (nPalavras == 2)
        {
            printf("\nMontante registado! A ser implementado\n");
        }
    }
    else if (strcmp(cmd, "exit") == 0)
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
    else
    {
        printf("\nComando invalido!\n");
    }
}

int main(int argc, char **argv)
{

    char cmd[50];
    char password[50];
    char username[50];
    USER *user;

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

        user->pass = atoi(argv[2]);

        printf("\nNome do user: %s\n", user->nome);

        printf("\n\nSenha registada!\n");

        printf("\n\n Username registado\n");

        while (1)
        {
            interface();
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