#pragma once
#include "wall.h"
#include "texture.h"
typedef struct {
    Wall *walls;
    int num_walls; 
    Texture *floor;       
} Scene;