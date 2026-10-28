#pragma once
#include <stdbool.h>

typedef struct screen Screen;
struct screen {
    void *user_data;
    bool (*load)(Screen *);   // true = terminou; chamado 1x por frame
    void (*update)(Screen *);
    void (*draw)(Screen *);
    void (*unload)(Screen *); // libera user_data e o que a tela criou
};
