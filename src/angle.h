#pragma once
#include <math.h>

#define ANGLE_90 (M_PI / 2)
#define ANGLE_180 M_PI
#define ANGLE_270 (M_PI * 3 / 2)

typedef struct
{
    double angle;
    double tg;
    double sin;
    double cos;
} Angle;

Angle create_angle(double angle);