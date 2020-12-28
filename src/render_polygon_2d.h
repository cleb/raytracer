#pragma once
#include "render_line.h"
#include "polygon_2d.h"

typedef struct
{
    Render_Line *lines;
    int numlines;
} Render_Polygon_2D;

Render_Polygon_2D *create_render_polygon_2d(Polygon_2D *polygon);
void destroy_render_polygon_2d(Render_Polygon_2D *polygon);