#include <math.h>
#include <stdlib.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "raytracer.h"
#include "texture.h"

Point point_null = {.x = INFINITY, .y = INFINITY};
Intersection intersection_null = {.point = {.x = INFINITY, .y = INFINITY}, .distance = INFINITY, .reflexivity = INFINITY};
Color ret_black = {.r = 0, .g = 0, .b = 0};

Color trace_ray(double player_x, double player_y, double player_z, double alpha, double beta, Render_Canvas * canvas, Render_Scene *scene, int max_bounce);
Intersection_Buffer *create_intersection_buffer(int size);
void destroy_intersection_buffer(Intersection_Buffer *buffer);

void copy_point(Point *src, Point *dest) {
    dest->x = src->x;
    dest->y = src->y;
}

void copy_intersection(Intersection *src, Intersection *dest) {
    dest->distance = src->distance;
    dest->reflexivity = src->reflexivity;
    dest->angle = src->angle;
    dest->point_in_space = src->point_in_space;
    dest->point = src->point;
    dest->texture = src->texture;
    copy_point(&src->point, &dest->point);    
}


void add_color(Color *c1, Color *c2) {
    double alpha = c2->alpha/255.0f;    
    double inverse_alpha = 1 - alpha;
    c1->r = inverse_alpha * c1->r + alpha * c2->r;
    c1->g = inverse_alpha * c1->g + alpha * c2->g;
    c1->b = inverse_alpha * c1->b + alpha * c2->b;
}

int point_equals(Point p1, Point p2) {
    return p1.x == p2.x && p1.y == p2.y;
}

int intersection_equals(Intersection *i1, Intersection *i2) {
    return i1->distance == i2->distance && point_equals(i1->point, i2->point);
}

int compare_intersections(const void *b, const void *a) {
    Intersection *a_intersection = (Intersection*)a;
    Intersection *b_intersection = (Intersection*)b;
    return a_intersection->distance > b_intersection->distance ? 1 : -1;
}

Point get_intersection_straight(Render_Wall *wall, double pa, double pb) {
    double intersection_x = wall->wall->p1.x;
    double intersection_y = pa * intersection_x + pb;
    if(intersection_y < wall->wall->p1.y || intersection_y > wall->wall->p2.y) {
        return point_null;
    }
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
}

Point get_intersection_90(Render_Wall *wall, double x, double y) {

    if(wall->wall->p1.x > x || wall->wall->p2.x < x) {
        Point null_ret;
        copy_point(&point_null, &null_ret);
        return null_ret;
    }

    double intersection_x = x;
    double intersection_y = wall->a * intersection_x + wall->b;
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
}



Point get_intersection_point(Render_Wall *wall, double x, double y, double a, double b, Angle alpha) {

    if(alpha.angle == M_PI_2 || alpha.angle == M_PI_2 * 3) {
        return get_intersection_90(wall, x, y);
    }

    double tg_alpha = alpha.tg;
    double pa = tg_alpha;
    double pb = y - x * tg_alpha;

    if(wall->wall->p1.x == wall->wall->p2.x){
        return get_intersection_straight(wall, pa, pb);        
    }

    double intersection_x = (b - pb) / (pa - a);
    if(intersection_x < wall->wall->p1.x || intersection_x > wall->wall->p2.x) {
        return point_null;
    }
    
    double intersection_y = a * intersection_x + b;
    Point ret = {.x = intersection_x, .y = intersection_y};
    return ret;
} 

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Intersection intersects(double x, double y, double z, Angle alpha, Angle beta, Render_Wall* wall){
    double a = wall->a;
    double b = wall->b;
    
    Point intersection = get_intersection_point(wall, x, y, a, b, alpha);

    if(point_equals(intersection, point_null)) {
        return intersection_null;
    }

    double intersection_x = intersection.x;
    double intersection_y = intersection.y;

    if((intersection_x - x) / alpha.cos < 0 || (intersection_y - y) / alpha.sin < 0 ) {
        //intersetion happened behind the viewer
        return intersection_null;
    }

    double wall_x = sqrtl(pow((intersection_y - wall->wall->p1.y),2) + pow((intersection_x - wall->wall->p1.x), 2));
    double dist_from_wall = sqrtl(pow((intersection_y - y),2) + pow((intersection_x - x), 2));
    double wall_y = dist_from_wall * beta.tg + z;

    if(wall_y > wall->wall->top || wall_y < wall->wall->bottom ) {
        return intersection_null;
    }

    Intersection ret = {
        .point = {.x = wall_x, .y = wall_y}, 
        .distance=dist_from_wall, 
        .texture = wall->wall->texture, 
        .reflexivity = wall->wall->reflexivity,
        .point_in_space = {.x = intersection_x, .y = intersection_y, .z = wall_y},
        .angle = wall->angle
        };
    return ret;
}

Intersection intersects_floor(Angle alpha, Angle beta, double player_x, double player_y, double player_z, Render_Canvas *canvas, Render_Scene * scene) {
    Intersection ret;
    double inverse_beta = M_PI_2 - beta.angle;
    Angle inverse_beta_angle = get_precomputed_angle(canvas, inverse_beta);
    double floor_dist = inverse_beta_angle.tg * player_z * -1;
    double ray_floor_dist = sqrt(pow(floor_dist,2) + pow(player_z,2));
    ret.distance = ray_floor_dist;
    Point floor_intersect = {.x = floor_dist * alpha.cos + player_x, .y = floor_dist * alpha.sin + player_y};
    ret.point = floor_intersect;
    ret.texture = scene->floor;
    ret.angle = 0;
    ret.reflexivity = 0;
    Point_3 floor_point_in_space = {
        .x = floor_intersect.x,
        .y = floor_intersect.y,
        .z = 0
    };
    ret.point_in_space = floor_point_in_space;
    return ret;
}

void add_to_intersection_buffer(Intersection_Buffer *buffer, Intersection *intersection) {
    copy_intersection(intersection,&buffer->buffer[buffer->top]);
    buffer->top ++;
}

void insert_to_intersection_buffer(Intersection_Buffer *buffer, Intersection *intersection, int index) {
    copy_intersection(intersection,&buffer->buffer[index]);    
}

void adjust_intersection_buffer_top(Intersection_Buffer *buffer, int top) {
    buffer->top = top;
}

Intersection_Buffer_Iterator get_intersection_buffer_iterator(Intersection_Buffer * buffer){
    qsort(buffer->buffer,buffer->top ,sizeof(Intersection),compare_intersections);
    Intersection_Buffer_Iterator iterator = {.current = buffer->buffer, .items = buffer->size};
    return iterator;
}

Intersection* intersection_buffer_iterator_get_next(Intersection_Buffer_Iterator *iterator) {
    if(iterator->items <= 0) {
        return NULL;
    }
    Intersection *ret = iterator->current;
    iterator->current++;
    iterator->items--;
    return ret;    
}

void follow_ray(Color *color, Intersection *intersection, double alpha, double beta, Render_Canvas * canvas, Render_Scene * scene, int max_bounce) {
    if(intersection->reflexivity > 0 && max_bounce > 0) {
        double new_alpha = 2 * intersection->angle - alpha;
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
            max_bounce - 1
        );
        reflection_color.alpha = intersection->reflexivity * 255;
        add_color(color, &reflection_color);
    }
}

Color trace_ray(double player_x, double player_y, double player_z, double alpha, double beta, Render_Canvas *canvas, Render_Scene *scene, int max_bounce){
    Angle alpha_angle = get_precomputed_angle(canvas, alpha);
    Angle beta_angle = get_precomputed_angle(canvas, beta);
    Color color = {.r = ret_black.r, .g = ret_black.g, .b=ret_black.b};
    Intersection_Buffer *intersection_buffer = create_intersection_buffer(scene->num_walls + 1);
    
    for(int i = 0; i < scene->num_walls; i++) {
        Intersection intersection = intersects(player_x, player_y, player_z, alpha_angle,beta_angle,&(scene->walls[i]));
        insert_to_intersection_buffer(intersection_buffer, &intersection, i);
    }   
    adjust_intersection_buffer_top(intersection_buffer, scene->num_walls);

    if(beta_angle.sin < 0) {
        Intersection floor_intersection = intersects_floor(alpha_angle, beta_angle, player_x, player_y, player_z, canvas, scene);
        add_to_intersection_buffer(intersection_buffer, &floor_intersection);
    } else {
        add_to_intersection_buffer(intersection_buffer, &intersection_null);
    }

    Intersection_Buffer_Iterator iterator = get_intersection_buffer_iterator(intersection_buffer);
    Intersection *current_intersection;
    while((current_intersection = intersection_buffer_iterator_get_next(&iterator)) != NULL) {
        if(!intersection_equals(current_intersection, &intersection_null)) {
            Color *intersection_color = get_color(current_intersection->texture,
            current_intersection->point.x,
            current_intersection->point.y);
            add_color(&color,intersection_color);            
            follow_ray(&color, current_intersection, alpha, beta, canvas, scene, max_bounce);
        }
    }

    destroy_intersection_buffer(intersection_buffer);
    
    return color;
}

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_z, Render_Canvas *canvas, Render_Scene *scene) {
    double plane_dist = canvas->plane_dist;
    double alpha = canvas->alpha[pixel_x] + player_alpha;
    double beta = canvas->beta[pixel_z * canvas->screen_w + pixel_x];

    return trace_ray(player_x, player_y, player_z, alpha, beta, canvas, scene, scene->max_bounce);
}

Render_Scene *create_render_scene(Scene *scene) {
    Render_Scene *ret = (Render_Scene *)malloc(sizeof(Render_Scene));
    ret->num_walls = scene->num_walls;
    ret->walls = (Render_Wall *)malloc(scene->num_walls * sizeof(Render_Wall));
    ret->max_bounce = 2;
    ret->floor = scene->floor;
    for(int i = 0; i < scene->num_walls; i++) {
        Wall *wall = &scene->walls[i];
        ret->walls[i].wall = &scene->walls[i];
        if(wall->p1.x != wall->p2.x) {
            double a = (wall->p2.y - wall->p1.y) / (wall->p2.x - wall->p1.x);
            double b = wall->p1.y - a*wall->p1.x;
            ret->walls[i].a = a;
            ret->walls[i].b = b;
            ret->walls[i].angle = atan(a);
        } else {
            ret->walls[i].a = INFINITY;
            ret->walls[i].b = INFINITY;
            ret->walls[i].angle = ANGLE_90;
        }
    }

    

    return ret;
}
void destroy_render_scene(Render_Scene *scene) {
    free(scene->walls);
    free(scene);
}

Intersection_Buffer *create_intersection_buffer(int size){
    Intersection_Buffer *ret = (Intersection_Buffer *)malloc(sizeof(Intersection_Buffer));
    ret->buffer = (Intersection *)malloc(size * sizeof(Intersection));    
    ret->size = size;
    ret->top = 0;
}

void destroy_intersection_buffer(Intersection_Buffer *buffer){
    free(buffer->buffer);
    free(buffer);
}

