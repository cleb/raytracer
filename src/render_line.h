#pragma once
#include "line.h"

typedef struct
{
    double a;
    double b;
    double angle;
    Line *line;
} Render_Line;

Render_Line create_render_line(Line line);