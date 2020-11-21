#pragma once
#include <math.h>

typedef struct {
    double angle;
    double tg;
    double sin;
    double cos;
} Angle;

Angle create_angle(double angle);