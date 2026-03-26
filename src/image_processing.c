#include "image_processing.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ------------------------------------------------------------------ */
SDL_Surface *load_image(const char *path) {
    if (!path || path[0] == '\0') {
        fprintf(stderr, "Caminho de imagem inválido.\n");
        return NULL;
    }

    SDL_Surface *surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "Não foi possível carregar a imagem '%s': %s\n",
                path, SDL_GetError());
        return NULL;
    }

    /* Converte para formato de pixel padrão RGBA8888 para facilitar manipulação */
    SDL_Surface *converted = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_RGBA32);
    SDL_DestroySurface(surface);
    if (!converted) {
        fprintf(stderr, "Erro ao converter formato da superfície: %s\n", SDL_GetError());
        return NULL;
    }

    return converted;
}

/* ------------------------------------------------------------------ */
bool is_grayscale(SDL_Surface *surface) {
    if (!surface) return false;

    SDL_LockSurface(surface);
    Uint8 *pixels = (Uint8 *)surface->pixels;
    int pitch = surface->pitch;
    int w = surface->w;
    int h = surface->h;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint8 *p = pixels + y * pitch + x * 4;
            /* RGBA32: R=p[0], G=p[1], B=p[2], A=p[3] */
            if (p[0] != p[1] || p[1] != p[2]) {
                SDL_UnlockSurface(surface);
                return false;
            }
        }
    }

    SDL_UnlockSurface(surface);
    return true;
}

/* ------------------------------------------------------------------ */
SDL_Surface *convert_to_grayscale(SDL_Surface *surface) {
    if (!surface) return NULL;

    int w = surface->w;
    int h = surface->h;

    SDL_Surface *gray = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
    if (!gray) {
        fprintf(stderr, "Erro ao criar superfície cinza: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_LockSurface(surface);
    SDL_LockSurface(gray);

    Uint8 *src_pixels = (Uint8 *)surface->pixels;
    Uint8 *dst_pixels = (Uint8 *)gray->pixels;

    int src_pitch = surface->pitch;
    int dst_pitch = gray->pitch;

    bool already_gray = is_grayscale(surface);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint8 *src = src_pixels + y * src_pitch + x * 4;
            Uint8 *dst = dst_pixels + y * dst_pitch + x * 4;

            Uint8 luma;
            if (already_gray) {
                luma = src[0];
            } else {
                double Y = 0.2125 * src[0] + 0.7154 * src[1] + 0.0721 * src[2];
                luma = (Uint8)(Y < 0.0 ? 0 : (Y > 255.0 ? 255 : Y));
            }

            dst[0] = luma;
            dst[1] = luma;
            dst[2] = luma;
            dst[3] = 255;
        }
    }

    SDL_UnlockSurface(gray);
    SDL_UnlockSurface(surface);

    return gray;
}

/* ------------------------------------------------------------------ */
SDL_Surface *equalize_histogram_surface(SDL_Surface *gray, const int hist[256]) {
    if (!gray) return NULL;

    int w = gray->w;
    int h = gray->h;
    int total = w * h;

    /* Calcula CDF (Cumulative Distribution Function) */
    double cdf[256] = {0};
    cdf[0] = (double)hist[0] / total;
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + (double)hist[i] / total;
    }

    /* Encontra cdf_min (primeiro valor não-zero) */
    double cdf_min = 0.0;
    for (int i = 0; i < 256; i++) {
        if (hist[i] > 0) {
            cdf_min = (double)hist[i] / total;
            break;
        }
    }

    /* Tabela de mapeamento */
    Uint8 lut[256];
    for (int i = 0; i < 256; i++) {
        double val = (cdf[i] - cdf_min) / (1.0 - cdf_min) * 255.0;
        if (val < 0.0) val = 0.0;
        if (val > 255.0) val = 255.0;
        lut[i] = (Uint8)val;
    }

    SDL_Surface *eq = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA32);
    if (!eq) {
        fprintf(stderr, "Erro ao criar superfície equalizada: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_LockSurface(gray);
    SDL_LockSurface(eq);

    Uint8 *src_pixels = (Uint8 *)gray->pixels;
    Uint8 *dst_pixels = (Uint8 *)eq->pixels;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Uint8 *src = src_pixels + y * gray->pitch + x * 4;
            Uint8 *dst = dst_pixels + y * eq->pitch   + x * 4;
            Uint8 mapped = lut[src[0]];
            dst[0] = mapped;
            dst[1] = mapped;
            dst[2] = mapped;
            dst[3] = 255;
        }
    }

    SDL_UnlockSurface(eq);
    SDL_UnlockSurface(gray);

    return eq;
}

/* ------------------------------------------------------------------ */
void save_image(SDL_Surface *surface) {
    if (!surface) return;

    const char *filename = "output_image.png";
    if (!IMG_SavePNG(surface, filename)) {
        fprintf(stderr, "Erro ao salvar imagem '%s': %s\n", filename, SDL_GetError());
    } else {
        printf("Imagem salva como '%s'.\n", filename);
    }
}
