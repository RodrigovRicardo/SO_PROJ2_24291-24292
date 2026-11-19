#ifndef __COMUM_H__
#define __COMUM_H__

#include <time.h>

#define CHAVE 1000

typedef struct {
    int id;
    char prato [100];
    int tempo_ms;
} Pedido;

typedef struct {
    long mtype;
    int fim;
    int id;
    char prato [100];
    time_t conclusao;
} Mensagem;

#endif