#pragma once
#include <math.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "angle.h"
#include "render_canvas.h"
#include "render_line.h"
#include "render_surface.h"

typedef struct
{
    Wall *wall;
    Render_Line line;
} Render_Wall;

typedef struct
{
    double distance_squared;
    Point point;
    Point_3 point_in_space;
    Texture *texture;
    double reflexivity;
    double angle;
    Color *color;
} Intersection;

typedef struct
{
    Intersection *buffer;
    int size;
    int top;
} Intersection_Buffer;

typedef struct
{
    Render_Wall *walls;
    int num_walls;
    int max_bounce;
    int num_surfaces;
    Render_Surface *surfaces;
    Intersection_Buffer **intersection_buffers;
    int num_intersection_buffers;
    Scene *scene;
} Render_Scene;

typedef struct
{
    Intersection_Buffer *buffer;
    Intersection *current;
    int items;
} Intersection_Buffer_Iterator;

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_y, Render_Canvas *canvas, Render_Scene *scene);
Render_Scene *create_render_scene(Scene *scene);
Render_Canvas *create_render_canvas(int screen_w, int screen_h);
void destroy_render_scene(Render_Scene *scene);
void destroy_render_canvas(Render_Canvas *canvas);

Intersection_Buffer *create_intersection_buffer(int size);
void destroy_intersection_buffer(Intersection_Buffer *buffer);
void add_to_intersection_buffer(Intersection_Buffer *buffer, Intersection *intersection);
Intersection_Buffer_Iterator get_intersection_buffer_iterator(Intersection_Buffer *buffer);
Intersection *intersection_buffer_iterator_get_next(Intersection_Buffer_Iterator *iterator);
void reset_intersection_buffer(Intersection_Buffer *buffer);