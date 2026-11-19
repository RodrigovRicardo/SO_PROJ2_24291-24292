#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/msg.h>

#include "headers/comum.h"
#include "headers/semaphore.h"

Pedido *fila;
int entrada = 0;
int saida = 0;
int capacidade;

int livres;
int ocupados;
int fogoes;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

FILE *ficheiro;
int num_conzinheiros;

void colocar(Pedido pedido) {

    down(livres);

    pthread_mutex_lock(&mutex);

    fila[entrada] = pedido;
    entrada = (entrada + 1) % capacidade;

    pthread_mutex_unlock(&mutex);

    up(ocupados);
}

Pedido retirar(void){
    Pedido pedido;

    down(ocupados);

    pthread_mutex_lock(&mutex);

    pedido = fila[saida];
    saida = (saida + 1) % capacidade;;

    pthread_mutex_unlock(&mutex);

    up(livres);

    return pedido;
}

void *rececionista(void *ptr){
    char *linha[200];
    Pedido pedido;
    int i;

    while(fgets(linha, sizeof(linha), ficheiro) != NULL){

        if(sscanf(linha, "%d;%99[^;]; %d", &pedido.id, pedido.prato, &pedido.tempo_ms) != 3){
            printf("Linha de pedido invalida \n");
        }else if(pedido.id <= 0 || pedido.tempo_ms < 0) {

            printf("Numero ou tempo o pedido invalido");
        }
        else {
            
            colocar(pedido);
        }

    }
}

void *cozinheiro(void *ptr)
{
    int id = *(int *)ptr;
    Pedido pedido;

    while (1) {
        pedido = retirar();

        if (pedido.id == -1) {
            break;
        }

        down(fogoes);

        printf("Cozinheiro %d reservou um fogao.\n", id);

        printf("Cozinheiro %d vai preparar o pedido %d: %s\n",
               id, pedido.id, pedido.prato);

        usleep(pedido.tempo_ms * 1000);

        printf("Cozinheiro %d terminou o pedido %d.\n",
               id, pedido.id);

        up(fogoes);
    }

    return NULL;
}