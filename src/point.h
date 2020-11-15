#include <math.h>

#pragma once
typedef struct {
    double x;
    double y;
} Point;

Point point_null = {.x = INFINITY, .y = INFINITY};