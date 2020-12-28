#pragma once
#include "render_polygon_2d.h"
#include "texture.h"
#include "floor.h"
typedef struct
{
    Render_Polygon_2D *polygon;
    Texture *texture;
    Floor *floor;
} Render_Floor;
