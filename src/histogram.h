#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "types.h"

/**
 * Calcula o histograma de uma superfície em escala de cinza.
 * hist deve ter 256 posições.
 */
void compute_histogram(SDL_Surface *gray, int hist[256]);

/**
 * Analisa o histograma e preenche média, desvio padrão e
 * os rótulos de brilho e contraste.
 */
void analyze_histogram(const int hist[256], double *mean, double *stddev,
                       char *brightness_label, char *contrast_label);

#endif /* HISTOGRAM_H */
