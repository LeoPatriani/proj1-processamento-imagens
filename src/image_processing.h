#ifndef IMAGE_PROCESSING_H
#define IMAGE_PROCESSING_H

#include "types.h"

/**
 * Carrega uma imagem a partir do caminho fornecido.
 * Retorna NULL em caso de erro (arquivo não encontrado, formato inválido, etc.)
 */
SDL_Surface *load_image(const char *path);

/**
 * Verifica se a superfície está em escala de cinza.
 * Retorna true se todos os pixels tiverem R == G == B.
 */
bool is_grayscale(SDL_Surface *surface);

/**
 * Converte uma superfície colorida para escala de cinza usando:
 *   Y = 0.2125*R + 0.7154*G + 0.0721*B
 * Se a imagem já for em escala de cinza, retorna uma cópia dela.
 * O chamador é responsável por destruir a superfície retornada.
 */
SDL_Surface *convert_to_grayscale(SDL_Surface *surface);

/**
 * Equaliza o histograma de uma superfície em escala de cinza.
 * Retorna uma nova superfície equalizada.
 * O chamador é responsável por destruir a superfície retornada.
 */
SDL_Surface *equalize_histogram_surface(SDL_Surface *gray, const int hist[256]);

/**
 * Salva a superfície atual como "output_image.png".
 * Sobrescreve se já existir.
 */
void save_image(SDL_Surface *surface);

#endif /* IMAGE_PROCESSING_H */
