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
    char* token = strtok(cmd, " ");
    int nPalavras = 0; // assumir que nao começamos com palavra nenhuma
    printf("Comando: ");
    scanf("%s", cmd);
    fflush(stdout);
    fflush(stdin);

    if (strcmp(cmd, "users") == 0)
    {
        printf("\nA ser implementado...\n");
    }
    else if (strcmp(cmd, "list") == 0)
    {
        printf("\nA ser implementado\n");
    }
    else if (strcmp(cmd, "kick") == 0)
    {
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o nome do utilizador a ser kickado.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
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
        while (token != NULL){
            nPalavras++;
            token = strtok(NULL, " ");
            
            if(nPalavras < 2){
                printf("\nPor favor insira o nome do executavel.\n");
            }
            else if (nPalavras == 2){
                printf("\nA ser implementado\n");
            }
        }
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


void leFicheiroVendas(char* nomeFich){
    char linha[150];
    ITEM item;
    FILE *f = fopen(nomeFich, "r");
    int i = 0;

    if (f == NULL){
        printf("\nNao foi possivel abrir ficheiro [%s]!\n", nomeFich);
        return;
    }

    printf("\nA ler info de ficheiro: [%s]\n", nomeFich);

    while (fscanf(f, "%d %s %s %d %d %d %s %s", &item.idItem, item.nomeItem, item.categoria, &item.valorAtual, &item.valorCompreJa, &item.duracao, item.sellerName, item.highestBidder) != EOF){
       
       if (i==0){
       printf("\n...............ITEM 1...............\n");
       }

       i++;

       if(i > 1){
        printf("\n...............ITEM %d...............\n", i);
       }
       
       printf("\nID do item: %d\n", item.idItem);
       printf("Nome do item: %s\n", item.nomeItem);
       printf("Categoria: %s\n", item.categoria);
       printf("Valor atual do item: %d\n", item.valorAtual);
       printf("Valor Compre Ja: %d\n", item.valorCompreJa);
       printf("Duracao do leilao: %d\n", item.duracao);
       printf("Vendedor: %s\n", item.sellerName);
       printf("Licitador mais elevado: %s\n", item.highestBidder); 
    }


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

    fclose(f);
}

int main(int argc, char **argv)
{
    char cmd[50];
    // varAmb();
    // lePipes();

    // Comandos

    leFicheiroVendas("vendas.txt");

    while (1){
        interface(cmd);
    }

    return 0;
}