#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "./color.h"
#include "./texture.h"

Texture *load_texture(const char* filename) {
    SDL_Surface *image = IMG_Load(filename);
    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32);
    SDL_Surface *image_rgba = SDL_ConvertSurface(image, format, 0);
    SDL_FreeFormat(format);

    Texture * ret = (Texture *)malloc(sizeof(Texture));
    ret->w = image_rgba->w;
    ret->h = image_rgba->h;
    ret->pixels = (Color *)malloc(ret->h * ret->w * sizeof(Color));
    unsigned char * pixels = image_rgba->pixels;
    for(int y = 0; y < ret->h; y ++) {
        for(int x = 0; x < ret->w; x ++) {
            ret->pixels[y * ret->w + x].r = pixels[(y * ret->w + x) * 4];
            ret->pixels[y * ret->w + x].g = pixels[(y * ret->w + x) * 4 + 1];
            ret->pixels[y * ret->w + x].b = pixels[(y * ret->w + x) * 4 + 2];
            ret->pixels[y * ret->w + x].alpha = pixels[(y * ret->w + x) * 4 + 3];
        }
    }
    return ret;        
}

Texture *destroy_texture(Texture *texture){
    free(texture);
}