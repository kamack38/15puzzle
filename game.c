#include "game.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "image.h"

int** gen_grid(int n, int m) {
    srand(time(NULL));
    int** grid = malloc(sizeof(int*) * m);
    if (grid == NULL) goto grid_fail;
    int count = n * m;
    int* tiles = malloc(count * sizeof(int));
    if (tiles == NULL) goto tiles_fail;
    for (int i = 0; i < count; ++i) {
        tiles[i] = i;
    }

    int swaps = 0;
    for (int i = count - 2; i > 0; --i) {
        int j = rand() % (i + 1);
        if (i != j) {
            int tmp = tiles[i];
            tiles[i] = tiles[j];
            tiles[j] = tmp;
            swaps++;
        }
    }
    if (swaps % 2 == 1) {
        int tmp = tiles[0];
        tiles[0] = tiles[1];
        tiles[1] = tmp;
    }

    for (int y = 0; y < m; ++y) {
        grid[y] = tiles + y * n;
    }
    return grid;

tiles_fail:
    free(grid);
grid_fail:
    return NULL;
}

void free_game(game_state_t* game) {
    free(game->grid[0]);
    free(game->grid);
    free(game);
}

game_state_t* create_game(image_t* image, int grid_width, int grid_height) {
    game_state_t* game = malloc(sizeof(game_state_t));
    if (game == NULL) goto game_fail;

    game->grid = gen_grid(grid_width, grid_height);
    if (game->grid == NULL) goto grid_fail;

    game->gridh = grid_height;
    game->gridw = grid_width;

    // Rounded up
    game->blockh = (image->height + grid_height - 1) / grid_height;
    game->blockw = (image->width + grid_width - 1) / grid_width;

    game->canvash = game->blockh * grid_height;
    game->canvasw = game->blockw * grid_width;

    game->height_rem = game->canvash - image->height;
    game->width_rem = game->canvasw - image->width;

    game->filler = FILLER;
    game->ypos = game->gridh - 1;
    game->xpos = game->gridw - 1;
    return game;

grid_fail:
    free(game);
game_fail:
    return NULL;
}

int max(int a, int b) { return a > b ? a : b; }

void get_pos_in_image(game_state_t* ctx, int tile, int* x, int* y) {
    *y = tile / ctx->gridw;
    *x = tile - (*y * ctx->gridw);
}

int get_x_offset(game_state_t* ctx, int grid_x) {
    int small_blocks = max(0, grid_x - ctx->width_rem), big_blocks = grid_x - small_blocks;
    return big_blocks * ctx->blockw + small_blocks * (ctx->blockw - 1);
}

int get_y_offset(game_state_t* ctx, int grid_y) {
    int small_blocks = max(0, grid_y - ctx->height_rem), big_blocks = grid_y - small_blocks;
    return big_blocks * ctx->blockh + small_blocks * (ctx->blockh - 1);
}

int isnarrow(game_state_t* ctx, int grid_x) { return grid_x >= ctx->gridw - ctx->width_rem; }

int isshort(game_state_t* ctx, int grid_y) { return grid_y >= ctx->gridh - ctx->height_rem; }

pixel_t* get_block(image_t* image, game_state_t* ctx, int tile, int y) {
    int grid_x, grid_y;
    get_pos_in_image(ctx, tile, &grid_x, &grid_y);
    int src_y = get_y_offset(ctx, grid_y) + y;
    int src_x = get_x_offset(ctx, grid_x);
    return image->bitmap[src_y] + src_x;
}

int write_grid(const char* filename, image_t* image, game_state_t* ctx) {
    FILE* f = fopen(filename, "wb");
    if (f == NULL) {
        perror("Unable to open output file");
        return 1;
    }
    fprintf(f, "P6\n%d %d %d\n", ctx->canvasw, ctx->canvash, image->max_color);
    for (int h = 0; h < ctx->gridh; ++h) {
        int block_height = isshort(ctx, h) ? ctx->blockh - 1 : ctx->blockh;
        for (int y = 0; y < block_height; ++y) {
            for (int w = 0; w < ctx->gridw; ++w) {
                size_t block_width = isnarrow(ctx, w) ? ctx->blockw - 1 : ctx->blockw;
                size_t wrote = fwrite(get_block(image, ctx, ctx->grid[h][w], y), sizeof(pixel_t),
                                      block_width, f);
                if (wrote != block_width) {
                    fprintf(stderr, "Failed to write %zu pixels, wrote %zu\n", block_width, wrote);
                    fclose(f);
                    return 1;
                }
                if (isnarrow(ctx, w)) {
                    fwrite(&ctx->filler, sizeof(pixel_t), 1, f);
                }
            }
        }
        if (isshort(ctx, h)) {
            for (int i = 0; i < ctx->canvasw; ++i) {
                fwrite(&ctx->filler, sizeof(pixel_t), 1, f);
            }
        }
    }
    fclose(f);
    return 0;
}

int write_block(FILE* f, image_t* image, game_state_t* ctx, int tile, int grid_x, int grid_y) {
    long offset = HEADER_SIZE + (long)grid_y * ctx->blockh * ctx->canvasw * sizeof(pixel_t) +
                  (long)grid_x * ctx->blockw * sizeof(pixel_t);
    if (fseek(f, offset, SEEK_SET)) {
        fprintf(stderr, "Failed to seek in file\n");
        return 1;
    }

    int bytes_to_skip = (ctx->canvasw - ctx->blockw) * sizeof(pixel_t);

    int block_height = isshort(ctx, grid_y) ? ctx->blockh - 1 : ctx->blockh;
    size_t block_width = isnarrow(ctx, grid_x) ? ctx->blockw - 1 : ctx->blockw;
    for (int y = 0; y < block_height; ++y) {
        if (y != 0) fseek(f, bytes_to_skip, SEEK_CUR);

        size_t wrote = fwrite(get_block(image, ctx, tile, y), sizeof(pixel_t), block_width, f);
        if (wrote != block_width) {
            fprintf(stderr, "Failed to write %zu. Wrote only %zu\n", block_width, wrote);
            return 1;
        }
        if (isnarrow(ctx, grid_x)) {
            fwrite(&ctx->filler, sizeof(pixel_t), 1, f);
        }
    }
    if (isshort(ctx, grid_y)) {
        for (int i = 0; i < ctx->blockw; ++i) {
            fwrite(&ctx->filler, sizeof(pixel_t), 1, f);
        }
    }
    return 0;
}

void play(const char* filename, image_t* image, game_state_t* ctx, MOVE move) {
    int game_tile = ctx->gridh * ctx->gridw - 1;
    int x = ctx->xpos, y = ctx->ypos;

    int new_x = x, new_y = y;
    switch (move) {
        case UP:
            new_y -= 1;
            if (new_y < 0) {
                fprintf(stderr, "Failed to move UP\n");
                return;
            }
            break;
        case LEFT:
            new_x -= 1;
            if (new_x < 0) {
                fprintf(stderr, "Failed to move LEFT\n");
                return;
            }
            break;
        case DOWN:
            new_y += 1;
            if (new_y >= ctx->gridh) {
                fprintf(stderr, "Failed to move DOWN\n");
                return;
            }
            break;
        case RIGHT:
            new_x += 1;
            if (new_x >= ctx->gridw) {
                fprintf(stderr, "Failed to move RIGHT\n");
                return;
            }
            break;
    }
    int replaced_tile = ctx->grid[new_y][new_x];

    FILE* f = fopen(filename, "rb+");
    if (f == NULL) {
        perror("Unable to open output file");
        return;
    }
    write_block(f, image, ctx, game_tile, new_x, new_y);
    write_block(f, image, ctx, replaced_tile, x, y);
    ctx->grid[y][x] = replaced_tile;
    ctx->grid[new_y][new_x] = game_tile;
    ctx->ypos = new_y, ctx->xpos = new_x;
    fclose(f);
}

void play_grid(const char* filename, image_t* image, game_state_t* ctx, MOVE move) {
    int game_tile = ctx->gridh * ctx->gridw - 1;
    int x = ctx->xpos, y = ctx->ypos;

    int new_x = x, new_y = y;
    switch (move) {
        case UP:
            new_y -= 1;
            if (new_y < 0) {
                fprintf(stderr, "Failed to move UP\n");
                return;
            }
            break;
        case LEFT:
            new_x -= 1;
            if (new_x < 0) {
                fprintf(stderr, "Failed to move LEFT\n");
                return;
            }
            break;
        case DOWN:
            new_y += 1;
            if (new_y >= ctx->gridh) {
                fprintf(stderr, "Failed to move DOWN\n");
                return;
            }
            break;
        case RIGHT:
            new_x += 1;
            if (new_x >= ctx->gridw) {
                fprintf(stderr, "Failed to move RIGHT\n");
                return;
            }
            break;
    }
    int replaced_tile = ctx->grid[new_y][new_x];

    ctx->grid[y][x] = replaced_tile;
    ctx->grid[new_y][new_x] = game_tile;
    ctx->ypos = new_y, ctx->xpos = new_x;
    write_grid(filename, image, ctx);
}
