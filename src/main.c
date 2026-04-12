/*
 * Grupo:
 *   - Leonardo Patriani Cardoso - 1041788
 *   - João Victor Martins - 10417928
 * Descrição: Software de processamento de imagens usando SDL3 e SDL_image.
 */

#include "image_processing.h"
#include "histogram.h"
#include "gui.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <caminho_da_imagem>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    /* Inicializa SDL */
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Erro ao inicializar SDL: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    /* SDL_image 3.x não requer IMG_Init/IMG_Quit */

    /* Inicializa SDL_ttf */
    if (!TTF_Init()) {
        fprintf(stderr, "Erro ao inicializar SDL_ttf: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Carrega a imagem */
    AppState state;
    memset(&state, 0, sizeof(AppState));

    state.original_surface = load_image(image_path);
    if (!state.original_surface) {
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Converte para escala de cinza */
    state.gray_surface = convert_to_grayscale(state.original_surface);
    if (!state.gray_surface) {
        fprintf(stderr, "Erro ao converter imagem para escala de cinza.\n");
        SDL_DestroySurface(state.original_surface);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* A imagem exibida começa como a cinza */
    state.current_surface = state.gray_surface;
    state.equalized = false;

    /* Calcula histograma inicial */
    compute_histogram(state.gray_surface, state.histogram);

    /* Analisa histograma */
    analyze_histogram(state.histogram, &state.mean, &state.stddev,
                      state.brightness_label, state.contrast_label);

    /* Cria GUI */
    if (!create_gui(&state)) {
        SDL_DestroySurface(state.gray_surface);
        SDL_DestroySurface(state.original_surface);
        TTF_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    /* Loop principal */
    run_event_loop(&state);

    /* Limpeza */
    destroy_gui(&state);

    if (state.equalized_surface) {
        SDL_DestroySurface(state.equalized_surface);
    }
    SDL_DestroySurface(state.gray_surface);
    SDL_DestroySurface(state.original_surface);

    TTF_Quit();
    SDL_Quit();

    return EXIT_SUCCESS;
}
