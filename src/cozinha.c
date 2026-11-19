#define _DEFAULT_SOURCE

#include "comum.h"
#include "semaphore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>

Pedido *fila;
int entrada = 0;
int saida = 0;
int capacidade;

int livres;
int ocupados;
int fogoes;
int msg_id;

FILE *ficheiro;
int num_cozinheiros;
int mutex;


void colocar(Pedido pedido) {
    

    down(livres); //inicializar no main 

    down(mutex);

    fila[entrada] = pedido;
    entrada = (entrada + 1) % capacidade;

    up(mutex);

    up(ocupados);
}

Pedido retirar(void){
    Pedido pedido;

    down(ocupados); //incializar no main

    down(mutex);

    pedido = fila[saida];
    saida = (saida + 1) % capacidade;

    up(mutex);

    up(livres);

    return pedido;
}

void *rececionista(void *ptr)
{
    (void)ptr;

    char linha[200];
    Pedido pedido;
    int i;

    while (fgets(linha, sizeof(linha), ficheiro) != NULL) {

       if (sscanf(linha, "%d;%99[^;];%d",&pedido.id, pedido.prato, &pedido.tempo_ms) != 3) {

            printf("Linha de pedido invalida.\n");
        }
        else if (pedido.id <= 0 || pedido.tempo_ms < 0) {

            printf("Numero ou tempo do pedido invalido.\n");
        }
        else {

            colocar(pedido);
        }
    }

    if (ferror(ficheiro)) {
        perror("Erro a ler o ficheiro");
    }

    pedido.id = -1;

    for (i = 0; i < num_cozinheiros; i++) {
        colocar(pedido);
    }

    return NULL;
}

void *cozinheiro(void *ptr)
{
    int id = *(int *)ptr;
    Pedido pedido;
    Mensagem msg = {0};

    while (1) {
        pedido = retirar();

        if (pedido.id == -1) {
            break;
        }

        down(fogoes);

        printf("Cozinheiro %d reservou um fogao.\n", id);

        printf("Cozinheiro %d vai preparar o pedido %d: %s\n", id, pedido.id, pedido.prato);

        usleep(pedido.tempo_ms * 1000);

        printf("Cozinheiro %d terminou o pedido %d.\n", id, pedido.id);

        up(fogoes);

        msg.mtype = 1;
        msg.fim = 0;
        msg.id = pedido.id;
        strcpy(msg.prato, pedido.prato);
        msg.conclusao = time(NULL);

        if (msgsnd(msg_id, &msg, sizeof(msg) - sizeof(msg.mtype), 0) == -1) {
            perror("Erro ao enviar o prato");
            exit(1);
        }

        printf("Pedido %d enviado para o painel.\n", pedido.id);

    }

    return NULL;
}

int main(int argc, char *argv[]){

    int i;
    int num_fogoes;

    if (argc != 5){

            printf("Uso: %s ficheiro cozinheiros capacidade fogoes\n", argv[0]);

        return 1;
    }

    num_cozinheiros = atoi(argv[2]);
    capacidade = atoi(argv[3]);
    num_fogoes = atoi(argv[4]);

    if(num_cozinheiros <= 0 || capacidade <= 0 || num_fogoes <= 0){

        printf("Os numeros devem ser positivos.\n");

        return 1;
    }

    ficheiro = fopen(argv[1], "r");

    if (ficheiro == NULL) {
        perror("Erro ao abrir o ficheiro");
        return 1;
    }

    fila = malloc(capacidade * sizeof(Pedido));

    if (fila == NULL) {
        perror("Erro ao reservar memoria");
        fclose(ficheiro);
        return 1;
    }

    livres = create_semaphore(IPC_PRIVATE);
    ocupados = create_semaphore(IPC_PRIVATE);
    fogoes = create_semaphore(IPC_PRIVATE);
    mutex = create_mutex(IPC_PRIVATE);

    for (i = 0; i < capacidade; i++) {
        up(livres);
    }

    for (i = 0; i < num_fogoes; i++) {
        up(fogoes);
    }

    destroy_semaphore(livres);
    destroy_semaphore(ocupados);
    destroy_semaphore(fogoes);
    destroy_mutex(mutex);

    free(fila);
    fclose(ficheiro);
    return 0;
}