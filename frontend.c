#include "frontend.h"
#include "backend.h"

void help()
{

    // printf("MAXMEDICOS: 5\n");
    // printf("MAXCLIENTES: 5\n");
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
    system("clear");
}

char *interface(char cmd[50])
{

    char *token = strtok(cmd, " ");
    int nPalavras = 0;
    printf("Comando: ");
    fflush(stdout);
    scanf("%s", cmd);

    if (strcmp(cmd, "sell") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 6){
                printf("\nPor favor insira o seguinte:  <nome-item> <categoria> <preço-base> <preço-compre-já> <duração>\n");
            }
            else if (nPalavras == 6){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "list") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "licat") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira a categoria.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "lisel") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o nome do vendedor.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "lival") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o preco maximo do artigo.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "litime") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o tempo restante (em segundos).\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "time") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "buy") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 3){
                printf("\nPor favor insira o seguinte: <id> <valor>\n");
            }
            else if (nPalavras == 3){
                printf("\nA ser implementado\n");
            }
        }
    }
    else if (strcmp(cmd, "cash") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "add") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o montante que deseja adicionar.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
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

    return cmd;
}

int main(int argc, char **argv)
{

    char cmd[50];
    char password[50];
    char username[50];

    // varAmb();
    // lePipes();

    if (argc >= 3)
    {
        printf("\n\n Nome do User: %s\n", argv[1]);
        printf("\n\n Senha do User: %s\n", argv[2]);

        printf("\n\n Username registado\n");

        while (1)
        {

            interface(cmd);
        }

        return 0;
    }
    else
    {
        printf("\n\n Insira seu username e password apos './frontend'\n");
        return 1;
    }

}