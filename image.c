#include "image.h"

#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void skip_comment(FILE* f) {
    int ch;
    while ((ch = fgetc(f)) != EOF) {
        if (isspace(ch)) continue;
        if (ch == '#') {
            while ((ch = fgetc(f)) != EOF && ch != '\n');
            continue;
        }
        ungetc(ch, f);
        return;
    }
}

int read_header(FILE* f, ftype* type, int* width, int* height, int* maxval) {
    char word[8];

    // Check file type
    skip_comment(f);
    fscanf(f, "%7s", word);
    if (strcmp(word, "P6") == 0) {
        *type = BINARY;
    } else if (strcmp(word, "P3") == 0) {
        *type = ASCII;
    } else {
        fprintf(stderr, "Failed to parse file type!\n");
        return 1;
    }

    skip_comment(f);
    if (!fscanf(f, "%d", width)) {
        fprintf(stderr, "Failed to read width!\n");
        return 1;
    }

    skip_comment(f);
    if (!fscanf(f, "%d", height)) {
        fprintf(stderr, "Failed to read height!\n");
        return 1;
    }

    skip_comment(f);
    if (!fscanf(f, "%d", maxval)) {
        fprintf(stderr, "Failed to read maxval!\n");
        return 1;
    }
    // Skip whitespace character
    fgetc(f);
    return 0;
}

pixel_t** bitmap_alloc(int width, int height) {
    pixel_t** image = malloc(height * sizeof(pixel_t*));
    if (image == NULL) return NULL;

    pixel_t* data = calloc(width * height, sizeof(pixel_t));
    if (data == NULL) {
        free(image);
        return NULL;
    }

    for (int x = 0; x < height; x++) {
        image[x] = data + x * width;
    }

    return image;
}

image_t* read_image(const char* path) {
    image_t* image = malloc(sizeof(image_t));
    if (image == NULL) {
        fprintf(stderr, "Failed to allocate the image");
        return NULL;
    }

    ftype type;
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        perror("Unable to open input file");
        return NULL;
    }

    if (read_header(file, &type, &image->width, &image->height, &image->max_color)) {
        fprintf(stderr, "Failed to read header\n");
        fclose(file);
        free(image);
        return NULL;
    }

    if ((image->bitmap = bitmap_alloc(image->width, image->height)) == NULL) {
        fprintf(stderr, "Failed to allocate bitmap\n");
        return NULL;
    }
    if (type == BINARY) {
        size_t read = fread(image->bitmap[0], sizeof(pixel_t), image->width * image->height, file);
        if (read != (size_t)(image->width * image->height)) {
            fprintf(stderr, "Couldn't read the whole image! Read %zu pixels instead of %d.\n", read,
                    image->width * image->height);
        }
    } else if (type == ASCII) {
        for (int i = 0; i < image->width * image->height; ++i) {
            unsigned int r, g, b;
            if (fscanf(file, " %u %u %u", &r, &g, &b) != 3) {
                fprintf(stderr, "Failed to read color\n");
            }
            image->bitmap[0][i] = (pixel_t){r, g, b};
        }
    }
    fclose(file);
    return image;
}

void free_image(image_t* image) {
    free(image->bitmap[0]);
    free(image->bitmap);
    free(image);
}
