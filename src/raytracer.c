#include <math.h>
#include <stdlib.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "raytracer.h"
#include "texture.h"

Point point_null = {.x = INFINITY, .y = INFINITY};
Intersection intersection_null = {.point = {.x = INFINITY, .y = INFINITY}, .distance = INFINITY};
Color ret_black = {.r = 0, .g = 0, .b = 0};

typedef struct {
    double angle;
    double tg;
    double sin;
    double cos;
} Angle;

Angle create_angle(double angle) {
    Angle ret = {.angle = angle, .tg = tan(angle), .sin = sin(angle), .cos = cos(angle)};
    return ret;
}

Angle create_angle_only_tg(double angle) {
    Angle ret = {.angle = angle, .tg = tan(angle), .sin = INFINITY, .cos = INFINITY};
    return ret;
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

Point get_intersection_point(Render_Wall *wall, double pa, double pb) {
    if(wall->wall->p1.x != wall->wall->p2.x){
        double a = wall->a;
        double b = wall->b;
        
        double intersection_x = (b - pb) / (pa - a);
        if(intersection_x < wall->wall->p1.x || intersection_x > wall->wall->p2.x) {
            return point_null;
        }
        
        double intersection_y = a * intersection_x + b;
        Point ret = {.x = intersection_x, .y = intersection_y};
        return ret;
    }


    double intersection_straight_x = wall->wall->p1.x;
    double intersection_straight_y = pa * intersection_straight_x + pb;
    if(intersection_straight_y < wall->wall->p1.y || intersection_straight_y > wall->wall->p2.y) {
        return point_null;
    }
    Point ret_straight = {.x = intersection_straight_x, .y = intersection_straight_y};
        return ret_straight;
} 

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Intersection intersects(double x, double y, double z, Angle alpha, Angle beta, Render_Wall* wall){
    double a = wall->a;
    double b = wall->b;
    double tg_alpha = alpha.tg;
    double pa = tg_alpha;
    double pb = y - x * tg_alpha;
    
    Point intersection = get_intersection_point(wall, pa, pb);

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

    Intersection ret = {.point = {.x = wall_x, .y = wall_y}, .distance=dist_from_wall, .wall = wall};
    return ret;
}

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_z, Render_Canvas *canvas, Render_Scene *scene) {
    double plane_dist = canvas->plane_dist;
    double alpha = canvas->alpha[pixel_x] + player_alpha;
    double beta = canvas->beta[pixel_z * canvas->screen_w + pixel_x];

    Angle alpha_angle = create_angle(alpha);
    Angle beta_angle = create_angle_only_tg(beta);

    //z-indexing ignored for now, draw the first 
    Color color = {.r = ret_black.r, .g = ret_black.g, .b=ret_black.b};
    for(int i = 0; i < scene->num_walls; i++) {
        scene->intersection_buffer[i] = intersects(player_x, player_y, player_z, alpha_angle,beta_angle,&(scene->walls[i]));
    }   

    qsort(scene->intersection_buffer,scene->num_walls,sizeof(Intersection),compare_intersections);
    for(int i = 0; i < scene->num_walls; i++) {
        Intersection *current_intersection = &scene->intersection_buffer[i];
        if(!intersection_equals(current_intersection, &intersection_null)) {
            Color *intersection_color = get_color(current_intersection->wall->wall->texture,
            current_intersection->point.x,
            current_intersection->point.y);
            add_color(&color,intersection_color);            
        }
    }


    
    return color;
}

Render_Scene *create_render_scene(Scene *scene) {
    Render_Scene *ret = (Render_Scene *)malloc(sizeof(Render_Scene));
    ret->num_walls = scene->num_walls;
    ret->walls = (Render_Wall *)malloc(scene->num_walls * sizeof(Render_Wall));
    ret->intersection_buffer = (Intersection *)malloc(scene->num_walls * sizeof(Intersection));
    for(int i = 0; i < scene->num_walls; i++) {
        Wall *wall = &scene->walls[i];
        ret->walls[i].wall = &scene->walls[i];
        if(wall->p1.x != wall->p2.x) {
            double a = (wall->p2.y - wall->p1.y) / (wall->p2.x - wall->p1.x);
            double b = wall->p1.y - a*wall->p1.x;
            ret->walls[i].a = a;
            ret->walls[i].b = b;
        } else {
            ret->walls[i].a = INFINITY;
            ret->walls[i].b = INFINITY;
        }
    }
    return ret;
}
void destroy_render_scene(Render_Scene *scene) {
    free(scene->walls);
    free(scene);
}

Render_Canvas *create_render_canvas(int screen_w, int screen_h) {
    Render_Canvas *ret = (Render_Canvas *)malloc(sizeof(Render_Canvas));
    ret->screen_w = screen_w;
    ret->screen_h = screen_h;
    ret->plane_dist = (screen_w / 2.0f);
    ret->alpha = (double *)malloc(ret->screen_w * sizeof(double));
    ret->beta = (double *)malloc(ret->screen_h * ret->screen_w * sizeof(double));

    
    int screen_w_half = screen_w / 2;
    int screen_h_half = screen_h / 2;

    for(int pixel_x = 0; pixel_x < screen_w; pixel_x ++) {
        double plane_x_offset = screen_w_half - pixel_x;
        ret->alpha[pixel_x] = atan(plane_x_offset/ret ->plane_dist);
        for(int pixel_z = 0; pixel_z < screen_h; pixel_z ++) {
            double plane_z_offset = screen_h_half - pixel_z;
            ret->beta[pixel_z * screen_w + pixel_x] = atan(plane_z_offset/sqrt(pow(plane_x_offset,2) + pow(ret->plane_dist,2)));
        }

    }
    return ret;
}

void destroy_render_canvas(Render_Canvas *canvas) {
    free(canvas->alpha);
    free(canvas->beta);
    free(canvas);
}

