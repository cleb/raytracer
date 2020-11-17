#pragma once
#include "./color.h"
#include "./texture.h"

Texture *load_texture(const char* filename);

Texture *destroy_texture(Texture *texture);