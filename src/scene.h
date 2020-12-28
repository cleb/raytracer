#pragma once
#include "wall.h"
#include "texture.h"
#include "floor.h"
typedef struct
{
    Wall *walls;
    int num_walls;
    Texture *floor;
    Floor *floors;
    int num_floors;
} Scene;