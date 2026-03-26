#ifndef UTILS_H
#define UTILS_H

#include "types.h"

/* Funções utilitárias diversas */

/** Retorna o máximo entre dois inteiros. */
static inline int max_int(int a, int b) { return a > b ? a : b; }

/** Clamp de um double para [0, 255] e converte para Uint8. */
static inline Uint8 clamp_to_byte(double v) {
    if (v < 0.0)   return 0;
    if (v > 255.0) return 255;
    return (Uint8)v;
}

#endif /* UTILS_H */
