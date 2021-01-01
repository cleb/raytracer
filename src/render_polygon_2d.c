#include <stdlib.h>
#include "render_polygon_2d.h"
#include "render_line.h"

void adjust_minmax(Point *min, Point *max, Point *src)
{
    if (src->x < min->x)
    {
        min->x = src->x;
    }
    if (src->y < min->y)
    {
        min->y = src->y;
    }
    if (src->x > max->x)
    {
        max->x = src->x;
    }
    if (src->y > max->y)
    {
        max->y = src->y;
    }
}

Render_Polygon_2D *create_render_polygon_2d(Polygon_2D *polygon)
{
    Render_Polygon_2D *ret = (Render_Polygon_2D *)malloc(sizeof(Render_Polygon_2D));
    ret->lines = (Render_Line *)malloc(polygon->numLines * sizeof(Render_Line));
    ret->numlines = polygon->numLines;
    ret->bound_min.x = INFINITY;
    ret->bound_min.y = INFINITY;
    ret->bound_max.x = -INFINITY;
    ret->bound_max.y = -INFINITY;
    for (int i = 0; i < polygon->numLines; i++)
    {
        ret->lines[i] = create_render_line(&polygon->lines[i]);
        adjust_minmax(&ret->bound_min, &ret->bound_max, &polygon->lines[i].start);
        adjust_minmax(&ret->bound_min, &ret->bound_max, &polygon->lines[i].end);
    }
    return ret;
}

void destroy_render_polygon_2d(Render_Polygon_2D *polygon)
{
    free(polygon->lines);
    free(polygon);
}