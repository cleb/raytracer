#pragma once
#include "wall.h"
#include "texture.h"
#include "surface.h"
typedef struct
{
    Wall *walls;
    int num_walls;
    Texture *surface;
    Surface *surfaces;
    int num_surfaces;
} Scene;