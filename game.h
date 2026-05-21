#ifndef GAME_H
#define GAME_H
#include "image.h"

#define HEADER_SIZE 17
#define FILLER ((pixel_t){0, 0, 0})
#define CDIV(a, b) (a / b + (a % b != 0))

typedef struct {
    int gridh, gridw;
    int blockh, blockw;
    int canvash, canvasw;
    int width_rem, height_rem;
    int ypos, xpos;
    int** grid;
    pixel_t filler;
} game_state_t;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
} MOVE;

game_state_t* create_game(image_t* image, int gwidth, int gheight);
void free_game(game_state_t* game);

int write_grid(const char* filename, image_t* image, game_state_t* ctx);
int write_block(FILE* f, image_t* image, game_state_t* ctx, int tile, int grid_x, int grid_y);
void play(const char* filename, image_t* image, game_state_t* ctx, MOVE move);
void play_grid(const char* filename, image_t* image, game_state_t* ctx, MOVE move);
#endif  // GAME_H
