#ifndef TYPES_H
#define TYPES_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>
#include <string.h>

/* Dimensões da janela secundária */
#define SEC_WIN_WIDTH  400
#define SEC_WIN_HEIGHT 550

/* Área do histograma dentro da janela secundária */
#define HIST_X       20
#define HIST_Y       30
#define HIST_W       360
#define HIST_H       200

/* Botão na janela secundária */
#define BTN_X        100
#define BTN_Y        470
#define BTN_W        200
#define BTN_H        50

#define LABEL_MAX 32

typedef struct {
    /* Superfícies */
    SDL_Surface *original_surface;   /* original colorida ou cinza original */
    SDL_Surface *gray_surface;       /* cinza (entrada para operações) */
    SDL_Surface *equalized_surface;  /* cinza equalizada */
    SDL_Surface *current_surface;    /* ponteiro para a exibida no momento */

    /* Histograma */
    int histogram[256];              /* histograma atual (cinza ou equalizado) */
    double mean;
    double stddev;
    char brightness_label[LABEL_MAX];
    char contrast_label[LABEL_MAX];

    /* Estado */
    bool equalized;

    /* SDL – janela principal */
    SDL_Window   *main_window;
    SDL_Renderer *main_renderer;
    SDL_Texture  *main_texture;

    /* SDL – janela secundária */
    SDL_Window   *sec_window;
    SDL_Renderer *sec_renderer;

    /* Fonte */
    TTF_Font *font;

    /* Estado do botão */
    bool btn_hovered;
    bool btn_pressed;
} AppState;

#endif /* TYPES_H */
