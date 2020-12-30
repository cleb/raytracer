#pragma once
#include "polygon_2d.h"
#include "texture.h"
typedef struct
{
    Polygon_2D *polygon;
    Texture *texture;
    double z;
} Surface;