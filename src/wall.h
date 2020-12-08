#pragma once
#include "line.h"
#include "texture.h"
typedef struct
{
    Line line;
    double top;
    double bottom;
    double reflexivity;
    Texture *texture;
} Wall;