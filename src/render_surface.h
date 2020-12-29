#pragma once
#include "render_polygon_2d.h"
#include "texture.h"
#include "surface.h"
typedef struct
{
    Render_Polygon_2D *polygon;
    Texture *texture;
    Surface *surface;
} Render_Surface;
