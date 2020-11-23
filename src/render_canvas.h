#pragma once
#include "angle.h"

typedef struct {
    int screen_w;
    int screen_h;
    double plane_dist;
    double *alpha;
    double *beta;
    Angle *precomputed_angles;
} Render_Canvas;

Angle get_precomputed_angle(Render_Canvas *canvas, double angle);