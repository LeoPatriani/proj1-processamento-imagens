#ifndef GUI_H
#define GUI_H

#include "types.h"

/**
 * Cria as janelas principal e secundária, renderers e carrega a fonte.
 * Retorna true em caso de sucesso, false caso contrário.
 */
bool create_gui(AppState *state);

/**
 * Executa o loop principal de eventos até o usuário fechar a aplicação.
 */
void run_event_loop(AppState *state);

/**
 * Libera todos os recursos de GUI alocados.
 */
void destroy_gui(AppState *state);

/* Funções auxiliares de renderização */
void render_main_window(AppState *state);
void render_secondary_window(AppState *state);

#endif /* GUI_H */
