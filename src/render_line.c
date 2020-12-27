#include "render_line.h"
#include "line.h"
#include "angle.h"

Render_Line create_render_line(Line *line)
{
    Render_Line ret;
    ret.line = line;
    if (line->start.x != line->end.x)
    {
        double a = (line->end.y - line->start.y) / (line->end.x - line->start.x);
        double b = line->start.y - a * line->start.x;
        ret.a = a;
        ret.b = b;
        ret.angle = atan(a);
    }
    else
    {
        ret.a = INFINITY;
        ret.b = INFINITY;
        ret.angle = ANGLE_90;
    }
    return ret;
}