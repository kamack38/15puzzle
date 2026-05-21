#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>

typedef struct {
    unsigned char r, g, b;
} pixel_t;

typedef struct {
    int width;
    int height;
    int max_color;
    pixel_t** bitmap;
} image_t;

typedef enum {
    BINARY,
    ASCII,
} ftype;

image_t* read_image(const char* path);
void free_image(image_t* image);
#endif  // IMAGE_H
