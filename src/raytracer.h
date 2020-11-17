#pragma once
#include <math.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#define ANGLE_90 (M_PI / 2)
#define ANGLE_180 M_PI
#define ANGLE_270 (M_PI * 3 / 2)

typedef struct {
    Wall *wall;
    double a;
    double b;
} Render_Wall;

typedef struct {
    double distance;
    Point point;
    Render_Wall *wall;
} Intersection;

typedef struct {
    Render_Wall *walls;
    int num_walls;        
    Intersection *intersection_buffer;
} Render_Scene;

Color render_pixel(double player_x, double player_y, double player_alpha, int pixel_x, int pixel_y, int screen_w, int screen_h, Render_Scene *scene);
Render_Scene *create_render_scene(Scene *scene);
void destroy_render_scene(Render_Scene *scene);
