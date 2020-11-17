#pragma once
#include "point.h"
#include "texture.h"
typedef struct {
    Point p1;
    Point p2;    
    double top;
    double bottom;
    Texture *texture;
} Wall;