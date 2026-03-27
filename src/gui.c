#include "gui.h"
#include "image_processing.h"
#include "histogram.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Caminho para a fonte TTF embutida via SDL_ttf fallback ou caminho do sistema */
#define FONT_PATH "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
#define FONT_SIZE 14

/* ------------------------------------------------------------------ */
static SDL_Texture *surface_to_texture(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    if (!tex) {
        fprintf(stderr, "Erro ao criar textura: %s\n", SDL_GetError());
    }
    return tex;
}

/* ------------------------------------------------------------------ */
bool create_gui(AppState *state) {
    int img_w = state->gray_surface->w;
    int img_h = state->gray_surface->h;

    /* --- Janela principal --- */
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Processamento de Imagens");
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, img_w);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, img_h);
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_CENTERED_NUMBER, true);

    state->main_window = SDL_CreateWindowWithProperties(props);
    SDL_DestroyProperties(props);

    if (!state->main_window) {
        fprintf(stderr, "Erro ao criar janela principal: %s\n", SDL_GetError());
        return false;
    }

    state->main_renderer = SDL_CreateRenderer(state->main_window, NULL);
    if (!state->main_renderer) {
        fprintf(stderr, "Erro ao criar renderer principal: %s\n", SDL_GetError());
        return false;
    }

    /* --- Janela secundária (filha) --- */
    /* Obtém posição da janela principal para posicionar a secundária ao lado */
    int main_x, main_y;
    SDL_GetWindowPosition(state->main_window, &main_x, &main_y);

    SDL_PropertiesID props2 = SDL_CreateProperties();
    SDL_SetStringProperty(props2, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Histograma");
    SDL_SetNumberProperty(props2, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, SEC_WIN_WIDTH);
    SDL_SetNumberProperty(props2, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, SEC_WIN_HEIGHT);
    SDL_SetNumberProperty(props2, SDL_PROP_WINDOW_CREATE_X_NUMBER, main_x + img_w + 10);
    SDL_SetNumberProperty(props2, SDL_PROP_WINDOW_CREATE_Y_NUMBER, main_y);
    SDL_SetPointerProperty(props2, SDL_PROP_WINDOW_CREATE_PARENT_POINTER, state->main_window);

    state->sec_window = SDL_CreateWindowWithProperties(props2);
    SDL_DestroyProperties(props2);

    if (!state->sec_window) {
        fprintf(stderr, "Aviso: não foi possível criar janela filha, criando independente: %s\n", SDL_GetError());
        /* Fallback: cria janela independente */
        state->sec_window = SDL_CreateWindow("Histograma",
                                              SEC_WIN_WIDTH, SEC_WIN_HEIGHT,
                                              0);
        if (!state->sec_window) {
            fprintf(stderr, "Erro ao criar janela secundária: %s\n", SDL_GetError());
            return false;
        }
        SDL_SetWindowPosition(state->sec_window, main_x + img_w + 10, main_y);
    }

    state->sec_renderer = SDL_CreateRenderer(state->sec_window, NULL);
    if (!state->sec_renderer) {
        fprintf(stderr, "Erro ao criar renderer secundário: %s\n", SDL_GetError());
        return false;
    }

    /* --- Fonte --- */
    state->font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!state->font) {
        /* Tenta fonte alternativa */
        state->font = TTF_OpenFont("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", FONT_SIZE);
    }
    if (!state->font) {
        fprintf(stderr, "Aviso: não foi possível carregar fonte TTF. Texto não será exibido.\n");
    }

    /* --- Textura inicial --- */
    state->main_texture = surface_to_texture(state->main_renderer, state->current_surface);
    if (!state->main_texture) return false;

    state->btn_hovered = false;
    state->btn_pressed = false;

    return true;
}

/* ------------------------------------------------------------------ */
void render_main_window(AppState *state) {
    SDL_SetRenderDrawColor(state->main_renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->main_renderer);

    if (state->main_texture) {
        SDL_RenderTexture(state->main_renderer, state->main_texture, NULL, NULL);
    }

    SDL_RenderPresent(state->main_renderer);
}

/* ------------------------------------------------------------------ */
static void draw_text(SDL_Renderer *renderer, TTF_Font *font,
                      const char *text, int x, int y, SDL_Color color) {
    if (!font || !text) return;

    SDL_Surface *surf = TTF_RenderText_Blended(font, text, 0, color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    if (!tex) return;

    float tw, th;
    SDL_GetTextureSize(tex, &tw, &th);
    SDL_FRect dst = { (float)x, (float)y, tw, th };
    SDL_RenderTexture(renderer, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
}

/* ------------------------------------------------------------------ */
void render_secondary_window(AppState *state) {
    SDL_SetRenderDrawColor(state->sec_renderer, 30, 30, 30, 255);
    SDL_RenderClear(state->sec_renderer);

    /* --- Título do histograma --- */
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 220, 50, 255};
    SDL_Color cyan   = {100, 220, 255, 255};

    draw_text(state->sec_renderer, state->font, "Histograma", HIST_X, 8, white);

    /* --- Desenha área de fundo do histograma --- */
    SDL_SetRenderDrawColor(state->sec_renderer, 15, 15, 15, 255);
    SDL_FRect hist_bg = {HIST_X - 2, HIST_Y - 2, HIST_W + 4, HIST_H + 4};
    SDL_RenderFillRect(state->sec_renderer, &hist_bg);

    /* --- Desenha barras do histograma --- */
    int max_val = 0;
    for (int i = 0; i < 256; i++) {
        if (state->histogram[i] > max_val) max_val = state->histogram[i];
    }

    if (max_val > 0) {
        float bar_w = (float)HIST_W / 256.0f;

        for (int i = 0; i < 256; i++) {
            float bar_h = (float)HIST_H * ((float)state->histogram[i] / (float)max_val);
            SDL_FRect bar = {
                HIST_X + i * bar_w,
                HIST_Y + HIST_H - bar_h,
                bar_w > 1.0f ? bar_w : 1.0f,
                bar_h
            };
            /* Coloração gradiente branco -> azul */
            Uint8 intensity = (Uint8)i;
            SDL_SetRenderDrawColor(state->sec_renderer, intensity, intensity, 255, 255);
            SDL_RenderFillRect(state->sec_renderer, &bar);
        }
    }

    /* --- Borda do histograma --- */
    SDL_SetRenderDrawColor(state->sec_renderer, 100, 100, 100, 255);
    SDL_FRect hist_border = {HIST_X - 2, HIST_Y - 2, HIST_W + 4, HIST_H + 4};
    SDL_RenderRect(state->sec_renderer, &hist_border);

    /* --- Informações de análise --- */
    int info_y = HIST_Y + HIST_H + 15;
    char buf[128];

    snprintf(buf, sizeof(buf), "Media de intensidade: %.1f  (%s)",
             state->mean, state->brightness_label);
    draw_text(state->sec_renderer, state->font, buf, HIST_X, info_y, yellow);

    snprintf(buf, sizeof(buf), "Desvio padrao: %.1f  (contraste %s)",
             state->stddev, state->contrast_label);
    draw_text(state->sec_renderer, state->font, buf, HIST_X, info_y + 22, cyan);

    /* --- Linha separadora --- */
    SDL_SetRenderDrawColor(state->sec_renderer, 80, 80, 80, 255);
    SDL_RenderLine(state->sec_renderer, HIST_X, info_y + 50,
                   HIST_X + HIST_W, info_y + 50);

    /* --- Botão --- */
    /* Cor do botão conforme estado */
    SDL_Color btn_text_color = {255, 255, 255, 255};
    if (state->btn_pressed) {
        /* Azul escuro – estado "clicado" */
        SDL_SetRenderDrawColor(state->sec_renderer, 0, 50, 150, 255);
    } else if (state->btn_hovered) {
        /* Azul claro – mouse em cima */
        SDL_SetRenderDrawColor(state->sec_renderer, 80, 160, 255, 255);
    } else {
        /* Azul neutro */
        SDL_SetRenderDrawColor(state->sec_renderer, 30, 100, 220, 255);
    }

    SDL_FRect btn_rect = {BTN_X, BTN_Y, BTN_W, BTN_H};
    SDL_RenderFillRect(state->sec_renderer, &btn_rect);

    /* Borda do botão */
    SDL_SetRenderDrawColor(state->sec_renderer, 150, 200, 255, 255);
    SDL_RenderRect(state->sec_renderer, &btn_rect);

    /* Texto do botão */
    const char *btn_label = state->equalized ? "Ver original" : "Equalizar";
    /* Centraliza texto no botão (aproximado) */
    int text_x = BTN_X + (BTN_W / 2) - (int)(strlen(btn_label) * 4);
    int text_y = BTN_Y + (BTN_H / 2) - 7;
    draw_text(state->sec_renderer, state->font, btn_label, text_x, text_y, btn_text_color);

    SDL_RenderPresent(state->sec_renderer);
}

/* ------------------------------------------------------------------ */
static bool point_in_button(float x, float y) {
    return (x >= BTN_X && x <= BTN_X + BTN_W &&
            y >= BTN_Y && y <= BTN_Y + BTN_H);
}

/* ------------------------------------------------------------------ */
static void update_main_texture(AppState *state) {
    if (state->main_texture) {
        SDL_DestroyTexture(state->main_texture);
    }
    state->main_texture = SDL_CreateTextureFromSurface(state->main_renderer,
                                                        state->current_surface);
}

/* ------------------------------------------------------------------ */
static void toggle_equalization(AppState *state) {
    if (!state->equalized) {
        /* Equaliza */
        if (state->equalized_surface) {
            SDL_DestroySurface(state->equalized_surface);
        }
        state->equalized_surface = equalize_histogram_surface(state->gray_surface,
                                                               state->histogram);
        if (!state->equalized_surface) return;

        state->current_surface = state->equalized_surface;
        state->equalized = true;

        /* Recalcula histograma da imagem equalizada */
        compute_histogram(state->equalized_surface, state->histogram);
        analyze_histogram(state->histogram, &state->mean, &state->stddev,
                          state->brightness_label, state->contrast_label);
    } else {
        /* Reverte para cinza original */
        state->current_surface = state->gray_surface;
        state->equalized = false;

        /* Restaura histograma da imagem cinza */
        compute_histogram(state->gray_surface, state->histogram);
        analyze_histogram(state->histogram, &state->mean, &state->stddev,
                          state->brightness_label, state->contrast_label);
    }

    update_main_texture(state);
}

/* ------------------------------------------------------------------ */
void run_event_loop(AppState *state) {
    bool running = true;
    SDL_Event event;

    SDL_WindowID main_id = SDL_GetWindowID(state->main_window);
    SDL_WindowID sec_id  = SDL_GetWindowID(state->sec_window);

    /* Renderiza estado inicial */
    render_main_window(state);
    render_secondary_window(state);

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;

                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                    running = false;
                    break;

                /* --- Teclado: salvar com S --- */
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.scancode == SDL_SCANCODE_S) {
                        save_image(state->current_surface);
                        printf("\n[Aviso] Imagem salva com sucesso como 'output_image.png'!\n");
                    }
                    if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                        running = false;
                    }
                    break;

                /* --- Mouse na janela secundária --- */
                case SDL_EVENT_MOUSE_MOTION:
                    if (event.motion.windowID == sec_id) {
                        bool was_hovered = state->btn_hovered;
                        state->btn_hovered = point_in_button(event.motion.x, event.motion.y);
                        if (was_hovered != state->btn_hovered) {
                            render_secondary_window(state);
                        }
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    if (event.button.windowID == sec_id && event.button.button == SDL_BUTTON_LEFT) {
                        if (point_in_button(event.button.x, event.button.y)) {
                            state->btn_pressed = true;
                            render_secondary_window(state);
                        }
                    }
                    break;

                case SDL_EVENT_MOUSE_BUTTON_UP:
                    if (event.button.windowID == sec_id && event.button.button == SDL_BUTTON_LEFT) {
                        if (state->btn_pressed && point_in_button(event.button.x, event.button.y)) {
                            state->btn_pressed = false;
                            toggle_equalization(state);
                            render_main_window(state);
                            render_secondary_window(state);
                        } else {
                            state->btn_pressed = false;
                            render_secondary_window(state);
                        }
                    }
                    break;

                case SDL_EVENT_WINDOW_EXPOSED:
                case SDL_EVENT_WINDOW_RESTORED:
                    if (event.window.windowID == main_id) {
                        render_main_window(state);
                    } else if (event.window.windowID == sec_id) {
                        render_secondary_window(state);
                    }
                    break;

                default:
                    break;
            }
        }

        SDL_Delay(16); /* ~60 FPS */
    }
}

/* ------------------------------------------------------------------ */
void destroy_gui(AppState *state) {
    if (state->font) {
        TTF_CloseFont(state->font);
        state->font = NULL;
    }
    if (state->main_texture) {
        SDL_DestroyTexture(state->main_texture);
        state->main_texture = NULL;
    }
    if (state->sec_renderer) {
        SDL_DestroyRenderer(state->sec_renderer);
        state->sec_renderer = NULL;
    }
    if (state->sec_window) {
        SDL_DestroyWindow(state->sec_window);
        state->sec_window = NULL;
    }
    if (state->main_renderer) {
        SDL_DestroyRenderer(state->main_renderer);
        state->main_renderer = NULL;
    }
    if (state->main_window) {
        SDL_DestroyWindow(state->main_window);
        state->main_window = NULL;
    }
}
