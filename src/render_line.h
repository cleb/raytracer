#pragma once
#include "line.h"

typedef struct
{
    double a;
    double b;
    double angle;
} Render_Line;

Render_Line create_render_line(Line line);