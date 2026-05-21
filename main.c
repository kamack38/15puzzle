#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

/* clang-format off */
#define OPTIONAL_ARGS\
    OPTIONAL_STRING_ARG(input, i, input, "input", "in.ppm", "Input ppm file")\
    OPTIONAL_STRING_ARG(output, o, output-file, "output", "out.ppm", "Output ppm file")\
    OPTIONAL_STRING_ARG(output_root, r, output-root, "output", "", "Output prefix of ppm file")\
    OPTIONAL_INT_ARG(gwidth, N, width, "width", 4, "Grid width")\
    OPTIONAL_INT_ARG(gheight, M, height, "height", 4, "Grid height")
/* clang-format on */

#define BOOLEAN_ARGS BOOLEAN_ARG(help, h, help, "Print help")

#include "argus.h"
#include "game.h"
#include "image.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
#include <termios.h>
#include <unistd.h>

static struct termios original = {0};

void init_io() {
    if (tcgetattr(STDIN_FILENO, &original)) perror("tcsetattr()");
    struct termios raw = original;
    raw.c_lflag &= ~(ICANON | ECHO);  // Disable buffering and echo
    raw.c_cc[VMIN] = 1;               // Minimum chars to read = 1
    raw.c_cc[VTIME] = 0;              // Timeout = 0
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &raw) < 0) perror("tcsetattr ~ICANON");
}

void clean_io() {
    original.c_lflag |= ICANON;
    original.c_lflag |= ECHO;
    if (tcsetattr(STDIN_FILENO, TCSADRAIN, &original) < 0) perror("tcsetattr()");
}

char getch() {
    char buf = 0;
    if (read(STDIN_FILENO, &buf, 1) < 0) perror("read()");
    return (buf);
}
#define READ(op) (op = getch())
#else
#define READ(op) scanf(" %c", &op) == 1
void init_io() {}
void clean_io() {}
#endif

int main(const int argc, const char* argv[]) {
    args_t args = make_default_args();

    if (parse_args(argc, argv, &args) || args.help) {
        print_help(argv[0]);
        return 1;
    }

    image_t* image = read_image(args.input);
    if (image == NULL) return 1;

    game_state_t* ctx = create_game(image, args.gwidth, args.gheight);
    if (ctx == NULL) {
        free_image(image);
        return 1;
    }

    if (write_grid(args.output, image, ctx)) {
        fprintf(stderr, "Failed to write inital grid!");
        return 1;
    }

    init_io();
    atexit(clean_io);

    int is_root = 0;
    int i = 0;
    char* filename;
    void (*game)(const char* filename, image_t* image, game_state_t* ctx, MOVE move);

    if (strcmp(args.output_root, "") != 0) {
        filename = malloc((strlen(args.output_root) + 8) * sizeof(args.output));
        is_root = 1;
        game = play_grid;
    } else {
        filename = malloc((strlen(args.output) + 3) * sizeof(args.output));
        game = play;
        strcpy(filename, args.output);
    }
    if (filename == NULL) {
        goto end_without_file;
    }

    char op;
    while (READ(op)) {
        if (is_root) {
            sprintf(filename, "%s_%d.ppm", args.output_root, i);
            ++i;
        }
        switch (toupper(op)) {
            case 'W':
                game(filename, image, ctx, UP);
                break;
            case 'A':
                game(filename, image, ctx, LEFT);
                break;
            case 'S':
                game(filename, image, ctx, DOWN);
                break;
            case 'D':
                game(filename, image, ctx, RIGHT);
                break;
            case 'Q':
                goto end;
        }
    }

end:
    free(filename);
end_without_file:
    clean_io();
    free_image(image);
    free_game(ctx);
    return 0;
}
