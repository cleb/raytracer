#pragma once
#include "color.h"
typedef struct {
    Color *pixels;
    int w;
    int h;
} Texture;

Color *get_color(Texture *texture, int x, int y);