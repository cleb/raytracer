#include <stdlib.h>
#include "render_polygon_2d.h"
#include "render_line.h"

Render_Polygon_2D *create_render_polygon_2d(Polygon_2D *polygon)
{
    Render_Polygon_2D *ret = (Render_Polygon_2D *)malloc(sizeof(Render_Polygon_2D));
    ret->lines = (Render_Line *)malloc(polygon->numLines * sizeof(Render_Line));
    ret->numlines = polygon->numLines;
    for (int i = 0; i < polygon->numLines; i++)
    {
        ret->lines[i] = create_render_line(&polygon->lines[i]);
    }
    return ret;
}

void destroy_render_polygon_2d(Render_Polygon_2D *polygon)
{
    free(polygon->lines);
    free(polygon);
}