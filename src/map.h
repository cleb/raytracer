#pragma once
#include "wall.h"
#include "texture.h"
#include "surface.h"

typedef struct
{
    Wall *walls;
    int num_walls;
    Texture **textures;
    int num_textures;
    Surface *surfaces;
    int num_surfaces;
    Texture *skybox;
} Map;

Map *load_map(char *filename);
void destroy_map(Map *map);