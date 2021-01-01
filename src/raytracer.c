#include <math.h>
#include <stdlib.h>
#include <omp.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "raytracer.h"
#include "texture.h"
#include "render_polygon_2d.h"
#include "render_surface.h"

Point point_null = {.x = INFINITY, .y = INFINITY};
Intersection intersection_null = {.point = {.x = INFINITY, .y = INFINITY}, .distance_squared = INFINITY, .reflexivity = INFINITY};
Color ret_black = {.r = 0, .g = 0, .b = 0};

Color trace_ray(double player_x, double player_y, double player_z, double alpha, double beta, Render_Canvas *canvas, Render_Scene *scene, int max_bounce);
Intersection_Buffer *create_intersection_buffer(int size);
void destroy_intersection_buffer(Intersection_Buffer *buffer);

void copy_point(Point *src, Point *dest)
{
    *dest = *src;
}

void copy_intersection(Intersection *src, Intersection *dest)
{
    *dest = *src;
}

void add_color(Color *c1, Color *c2)
{
    double alpha = c2->alpha_double;
    double inverse_alpha = 1 - alpha;
    c1->r = inverse_alpha * c1->r + alpha * c2->r;
    c1->g = inverse_alpha * c1->g + alpha * c2->g;
    c1->b = inverse_alpha * c1->b + alpha * c2->b;
}

int point_equals(Point p1, Point p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

int intersection_equals(Intersection *i1, Intersection *i2)
{
    return i1->distance_squared == i2->distance_squared && point_equals(i1->point, i2->point);
}

int compare_intersections(const void *b, const void *a)
{
    Intersection *a_intersection = (Intersection *)a;
    Intersection *b_intersection = (Intersection *)b;
    return a_intersection->distance_squared > b_intersection->distance_squared ? 1 : -1;
}

Point get_intersection_straight(Render_Wall *wall, double pa, double pb)
{
    double intersection_x = wall->wall->line.start.x;
    double intersection_y = pa * intersection_x + pb;
    if (intersection_y < wall->wall->line.start.y || intersection_y > wall->wall->line.end.y)
    {
        return point_null;
    }
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
}

Point get_intersection_90(Render_Wall *wall, double x, double y)
{

    if (wall->wall->line.start.x > x || wall->wall->line.end.x < x)
    {
        Point null_ret;
        copy_point(&point_null, &null_ret);
        return null_ret;
    }

    double intersection_x = x;
    double intersection_y = wall->line.a * intersection_x + wall->line.b;
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
}

Point get_intersection_point(Render_Wall *wall, double x, double y, double a, double b, Angle alpha)
{

    if (alpha.angle == M_PI_2 || alpha.angle == M_PI_2 * 3)
    {
        return get_intersection_90(wall, x, y);
    }

    double tg_alpha = alpha.tg;
    double pa = tg_alpha;
    double pb = y - x * tg_alpha;

    if (wall->wall->line.start.x == wall->wall->line.end.x)
    {
        return get_intersection_straight(wall, pa, pb);
    }

    double intersection_x = (b - pb) / (pa - a);
    if (intersection_x < wall->wall->line.start.x || intersection_x > wall->wall->line.end.x)
    {
        return point_null;
    }

    double intersection_y = a * intersection_x + b;
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
}

int division_would_be_less_than_0(double a, double b)
{
    return a > 0 && b < 0 || a < 0 && b > 0;
}

int intersection_is_impossible(double x, double y, double z, Angle alpha, Render_Wall *wall)
{
    if (alpha.sin > 0 && wall->line.line->start.y < y && wall->line.line->end.y < y)
    {
        return 1;
    }
    if (alpha.sin < 0 && wall->line.line->start.y > y && wall->line.line->end.y > y)
    {
        return 1;
    }
    if (alpha.cos > 0 && wall->line.line->end.x < x)
    {
        return 1;
    }
    if (alpha.cos < 0 && wall->line.line->start.x > x)
    {
        return 1;
    }
    return 0;
}

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Intersection intersects(double x, double y, double z, Angle alpha, Angle beta, Render_Wall *wall)
{
    if (intersection_is_impossible(x, y, z, alpha, wall))
    {
        return intersection_null;
    }
    double a = wall->line.a;
    double b = wall->line.b;

    Point intersection = get_intersection_point(wall, x, y, a, b, alpha);

    if (point_equals(intersection, point_null))
    {
        return intersection_null;
    }

    double intersection_x = intersection.x;
    double intersection_y = intersection.y;

    if (division_would_be_less_than_0(intersection_x - x, alpha.cos) || division_would_be_less_than_0(intersection_y - y, alpha.sin))
    {
        //intersetion happened behind the viewer
        return intersection_null;
    }

    double dist_from_wall = sqrt(pow((intersection_y - y), 2) + pow((intersection_x - x), 2));
    double wall_y = dist_from_wall * beta.tg + z;

    if (wall_y > wall->wall->top || wall_y < wall->wall->bottom)
    {
        return intersection_null;
    }

    double wall_x = sqrt(pow((intersection_y - wall->wall->line.start.y), 2) + pow((intersection_x - wall->wall->line.start.x), 2));

    double ray_dist = (pow(z - wall_y, 2) + pow(dist_from_wall, 2));

    Intersection ret = {
        .point = {.x = wall_x, .y = wall_y},
        .distance_squared = ray_dist,
        .texture = wall->wall->texture,
        .reflexivity = wall->wall->reflexivity,
        .point_in_space = {.x = intersection_x, .y = intersection_y, .z = wall_y},
        .angle = wall->line.angle};

    ret.color = get_color(ret.texture,
                          ret.point.x,
                          ret.point.y);
    return ret;
}

int intersects_polygon_2d(Render_Polygon_2D *polygon, Point point)
{
    if (polygon->bound_min.x > point.x || polygon->bound_min.y > point.y || polygon->bound_max.x < point.x || polygon->bound_max.y < point.y)
    {
        return 0;
    }
    int has_above = 0;
    int has_below = 0;
    for (int i = 0; i < polygon->numlines; i++)
    {
        Render_Line *current_line = &polygon->lines[i];
        if (current_line->line->start.x == current_line->line->end.x)
        {
            continue;
        }
        if (current_line->line->start.x < point.x && current_line->line->end.x > point.x)
        {
            if (point.y < current_line->a * point.x + current_line->b)
            {
                has_below = 1;
            }
            else
            {
                has_above = 1;
            }
            if (has_above && has_below)
            {
                return 1;
            }
        }
    }
    return 0;
}

Intersection intersects_surface(Render_Surface *surface, Angle alpha, Angle beta, double player_x, double player_y, double player_z, Render_Canvas *canvas, Render_Scene *scene)
{
    Intersection ret;
    if (beta.sin < 0)
    {
        if (surface->surface->z > player_z)
        {
            return intersection_null;
        }

        double inverse_beta = M_PI_2 - beta.angle;
        Angle inverse_beta_angle = get_precomputed_angle(canvas, inverse_beta);
        double floor_dist = inverse_beta_angle.tg * (surface->surface->z - player_z);
        double ray_floor_dist = (pow(floor_dist, 2) + pow((player_z - surface->surface->z), 2));
        ret.distance_squared = ray_floor_dist;
        Point floor_intersect = {.x = floor_dist * alpha.cos + player_x, .y = floor_dist * alpha.sin + player_y};
        ret.point = floor_intersect;
    }
    else
    {
        if (surface->surface->z < player_z)
        {
            return intersection_null;
        }

        double ceil_dist = (surface->surface->z - player_z) / beta.tg;
        double ray_ceil_dist = (pow(ceil_dist, 2) + pow((surface->surface->z - player_z), 2));
        ret.distance_squared = ray_ceil_dist;
        Point ceil_intersect = {.x = ceil_dist * alpha.cos + player_x, .y = ceil_dist * alpha.sin + player_y};
        ret.point = ceil_intersect;
    }

    if (intersects_polygon_2d(surface->polygon, ret.point))
    {
        ret.texture = surface->texture;
        ret.angle = 0;
        ret.reflexivity = 0;
        Point_3 floor_point_in_space = {
            .x = ret.point.x,
            .y = ret.point.y,
            .z = 0};
        ret.point_in_space = floor_point_in_space;
        ret.color = get_color(ret.texture,
                              ret.point.x,
                              ret.point.y);
        return ret;
    }
    return intersection_null;
}

void add_to_intersection_buffer(Intersection_Buffer *buffer, Intersection *intersection)
{
    copy_intersection(intersection, &buffer->buffer[buffer->top]);
    buffer->top++;
}

Intersection_Buffer_Iterator get_intersection_buffer_iterator(Intersection_Buffer *buffer)
{
    qsort(buffer->buffer, buffer->top, sizeof(Intersection), compare_intersections);
    Intersection_Buffer_Iterator iterator = {.current = buffer->buffer, .items = buffer->top, .buffer = buffer};
    return iterator;
}

void intersection_buffer_iterator_skip_obscured(Intersection_Buffer_Iterator *iterator)
{
    for (int i = iterator->buffer->top - 1; i > 0; i--)
    {
        if (iterator->buffer->buffer[i].color->alpha == 255)
        {
            iterator->current = &iterator->buffer->buffer[i];
            iterator->items = iterator->buffer->top - i;
        }
    }
}

Intersection *intersection_buffer_iterator_get_next(Intersection_Buffer_Iterator *iterator)
{
    if (iterator->items <= 0)
    {
        return NULL;
    }
    Intersection *ret = iterator->current;
    iterator->current++;
    iterator->items--;
    return ret;
}

void follow_ray(Color *color, Intersection *intersection, double alpha, double beta, Render_Canvas *canvas, Render_Scene *scene, int max_bounce)
{
    if (intersection->reflexivity > 0 && max_bounce > 0)
    {
        double new_alpha = 2.0f * intersection->angle - alpha;
        double new_beta = beta;
        Angle new_alpha_angle = get_precomputed_angle(canvas, new_alpha);
        Color reflection_color = trace_ray(
            intersection->point_in_space.x + new_alpha_angle.cos,
            intersection->point_in_space.y + new_alpha_angle.sin,
            intersection->point_in_space.z,
            new_alpha,
            new_beta,
            canvas,
            scene,
            max_bounce - 1);
        reflection_color.alpha_double = intersection->reflexivity;
        add_color(color, &reflection_color);
    }
}

Color trace_ray(double player_x, double player_y, double player_z, double alpha, double beta, Render_Canvas *canvas, Render_Scene *scene, int max_bounce)
{
    Angle alpha_angle = get_precomputed_angle(canvas, alpha);
    Angle beta_angle = get_precomputed_angle(canvas, beta);
    Color color = {.r = ret_black.r, .g = ret_black.g, .b = ret_black.b, .alpha = 255, .alpha_double = 1};
    int buffer_position = omp_get_thread_num() * (scene->max_bounce + 1) + max_bounce;
    Intersection_Buffer *intersection_buffer = scene->intersection_buffers[buffer_position];

    for (int i = 0; i < scene->num_walls; i++)
    {
        Intersection intersection = intersects(player_x, player_y, player_z, alpha_angle, beta_angle, &(scene->walls[i]));
        if (!intersection_equals(&intersection, &intersection_null))
        {
            add_to_intersection_buffer(intersection_buffer, &intersection);
        }
    }

    for (int i = 0; i < scene->num_surfaces; i++)
    {
        Intersection floor_intersection = intersects_surface(&scene->surfaces[i], alpha_angle, beta_angle, player_x, player_y, player_z, canvas, scene);
        if (!intersection_equals(&floor_intersection, &intersection_null))
        {
            add_to_intersection_buffer(intersection_buffer, &floor_intersection);
        }
    }

    Intersection_Buffer_Iterator iterator = get_intersection_buffer_iterator(intersection_buffer);
    intersection_buffer_iterator_skip_obscured(&iterator);
    Intersection *current_intersection;
    while ((current_intersection = intersection_buffer_iterator_get_next(&iterator)) != NULL)
    {
        if (!intersection_equals(current_intersection, &intersection_null))
        {
            add_color(&color, current_intersection->color);
            follow_ray(&color, current_intersection, alpha, beta, canvas, scene, max_bounce);
        }
    }

    reset_intersection_buffer(intersection_buffer);
    return color;
}

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_z, Render_Canvas *canvas, Render_Scene *scene)
{
    double plane_dist = canvas->plane_dist;
    double alpha = canvas->alpha[pixel_x] + player_alpha;
    double beta = canvas->beta[pixel_z * canvas->screen_w + pixel_x];

    return trace_ray(player_x, player_y, player_z, alpha, beta, canvas, scene, scene->max_bounce);
}

Render_Scene *create_render_scene(Scene *scene)
{
    Render_Scene *ret = (Render_Scene *)malloc(sizeof(Render_Scene));
    ret->num_walls = scene->num_walls;
    ret->walls = (Render_Wall *)malloc(scene->num_walls * sizeof(Render_Wall));

    for (int i = 0; i < scene->num_walls; i++)
    {
        Wall *wall = &scene->walls[i];
        ret->walls[i].wall = &scene->walls[i];
        ret->walls[i].line = create_render_line(&scene->walls[i].line);
    }

    //surfaces
    ret->num_surfaces = scene->num_surfaces;
    ret->surfaces = (Render_Surface *)malloc(scene->num_surfaces * sizeof(Render_Surface));
    for (int i = 0; i < scene->num_surfaces; i++)
    {
        Surface *surface = &scene->surfaces[i];
        ret->surfaces[i].surface = &scene->surfaces[i];
        ret->surfaces[i].polygon = create_render_polygon_2d(scene->surfaces[i].polygon);
        ret->surfaces[i].texture = scene->surfaces[i].texture;
    }

    //global settings
    ret->max_bounce = 2;
    ret->num_intersection_buffers = (ret->max_bounce + 1) * omp_get_max_threads();

    ret->intersection_buffers = (Intersection_Buffer **)malloc(ret->num_intersection_buffers * sizeof(Intersection_Buffer *));
    for (int i = 0; i < ret->num_intersection_buffers; i++)
    {
        ret->intersection_buffers[i] = create_intersection_buffer(scene->num_walls + scene->num_surfaces + 1);
    }

    return ret;
}
void destroy_render_scene(Render_Scene *scene)
{
    for (int i = 0; i < scene->num_surfaces; i++)
    {
        destroy_render_polygon_2d(scene->surfaces[i].polygon);
    }
    for (int i = 0; i < scene->num_intersection_buffers; i++)
    {
        destroy_intersection_buffer(scene->intersection_buffers[i]);
    }
    free(scene->intersection_buffers);
    free(scene->surfaces);
    free(scene->walls);
    free(scene);
}

Intersection_Buffer *create_intersection_buffer(int size)
{
    Intersection_Buffer *ret = (Intersection_Buffer *)malloc(sizeof(Intersection_Buffer));
    ret->buffer = (Intersection *)malloc(size * sizeof(Intersection));
    ret->size = size;
    ret->top = 0;
    return ret;
}

void reset_intersection_buffer(Intersection_Buffer *buffer)
{
    buffer->top = 0;
}

void destroy_intersection_buffer(Intersection_Buffer *buffer)
{
    free(buffer->buffer);
    free(buffer);
}
