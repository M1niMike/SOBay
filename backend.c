#include "frontend.h"
#include "backend.h"

// Unamed Pipes - Promotor
//  void lePipes(){

//     int FD_BC[2];
//     int FD_CB[2];

//     char str[50];
//     char volta[50];

//     pipe(FD_BC);
//     pipe(FD_CB);

//     int id = fork();

//     if (id > 0){
//         close(FD_BC[0]);

//         printf("\nIndique os seus sintomas\n");
//         fgets(str, 49, stdin);

//         write(FD_BC[1], str, 49);
//         close(FD_CB[1]);
//         read(FD_CB[0], volta , 50);
//         close(FD_CB[0]);

//         printf("recebeu do classificador: %s\n", volta);

//     }

//     else if (id == 0){
//         close(STDIN_FILENO);

//         dup(FD_BC[0]); // para duplicar o file descriptor, ligando-o ao primeiro fd vazio

//         close(FD_BC[0]);
//         close(FD_BC[1]);
//         close(STDOUT_FILENO);

//         dup(FD_CB[1]);

//         close(FD_CB[1]);
//         close(FD_CB[0]);

//         execl("./classificador", "classificador", NULL);

//     }
//     else{
//         printf("Falha na criacao do fork()");

//         exit(EXIT_FAILURE);
//     }

// }

void help()
{

    // printf("MAXMEDICOS: 5\n");
    // printf("MAXCLIENTES: 5\n");
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
    system("clear");
}

char *interface(char cmd[50])
{

    printf("Comando: ");
    fflush(stdout);
    scanf("%s", cmd);

    if (strcmp(cmd, "users") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "list") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "kick") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "prom") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "reprom") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "cancel") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "close") == 0)
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

    // varAmb();
    // lePipes();

    // Comandos
    while (1)
    {

        interface(cmd);
    }

    return 0;
}