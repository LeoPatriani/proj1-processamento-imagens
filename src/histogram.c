#include "histogram.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
void compute_histogram(SDL_Surface *gray, int hist[256]) {
    memset(hist, 0, 256 * sizeof(int));
    if (!gray) return;

    SDL_LockSurface(gray);
    Uint8 *pixels = (Uint8 *)gray->pixels;
    int pitch = gray->pitch;
    int w = gray->w;
    int h = gray->h;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint8 *p = pixels + y * pitch + x * 4;
            hist[p[0]]++;
        }
    }

    SDL_UnlockSurface(gray);
}

/* ------------------------------------------------------------------ */
void analyze_histogram(const int hist[256], double *mean, double *stddev,
                       char *brightness_label, char *contrast_label) {
    long long total = 0;
    long long sum   = 0;

    for (int i = 0; i < 256; i++) {
        total += hist[i];
        sum   += (long long)i * hist[i];
    }

    if (total == 0) {
        *mean   = 0.0;
        *stddev = 0.0;
        snprintf(brightness_label, LABEL_MAX, "indefinido");
        snprintf(contrast_label,   LABEL_MAX, "indefinido");
        return;
    }

    *mean = (double)sum / (double)total;

    /* Desvio padrão */
    double variance = 0.0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] == 0) continue;
        double diff = (double)i - *mean;
        variance += diff * diff * hist[i];
    }
    variance /= (double)total;
    *stddev = sqrt(variance);

    /* Classificação de brilho */
    if (*mean < 85.0) {
        snprintf(brightness_label, LABEL_MAX, "escura");
    } else if (*mean < 170.0) {
        snprintf(brightness_label, LABEL_MAX, "media");
    } else {
        snprintf(brightness_label, LABEL_MAX, "clara");
    }

    /* Classificação de contraste */
    if (*stddev < 40.0) {
        snprintf(contrast_label, LABEL_MAX, "baixo");
    } else if (*stddev < 80.0) {
        snprintf(contrast_label, LABEL_MAX, "medio");
    } else {
        snprintf(contrast_label, LABEL_MAX, "alto");
    }
}
