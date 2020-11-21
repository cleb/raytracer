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
    double angle;
} Render_Wall;

typedef struct {
    double distance;
    Point point;
    Point_3 point_in_space;
    Texture *texture;
    double reflexivity;
    double angle;
} Intersection;

typedef struct {
    Render_Wall *walls;
    int num_walls;   
    Texture *floor;     
    Intersection *intersection_buffer;
    int max_bounce;
} Render_Scene;

typedef struct {
    int screen_w;
    int screen_h;
    double plane_dist;
    double *alpha;
    double *beta;
} Render_Canvas;


typedef struct {
    double angle;
    double tg;
    double sin;
    double cos;
} Angle;

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_y, Render_Canvas *canvas, Render_Scene *scene);
Render_Scene *create_render_scene(Scene *scene);
Render_Canvas *create_render_canvas(int screen_w, int screen_h);
void destroy_render_scene(Render_Scene *scene);
void destroy_render_canvas(Render_Canvas *canvas);
