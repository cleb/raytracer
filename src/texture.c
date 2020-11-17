#include "texture.h"
#include <math.h>
#include <stdlib.h>

Color *get_color(Texture *texture, int x, int y) {
    int t_x = (x >= 0 ? x : abs(texture->w + x)) % texture->w;
    int t_y = (y >= 0 ? y : abs(texture->h + y)) % texture->h;

    return &texture->pixels[t_y * texture->w + t_x];
}