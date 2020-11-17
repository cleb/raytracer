#include <math.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "raytracer.h"
#include "texture.h"

Point point_null = {.x = INFINITY, .y = INFINITY};
Intersection intersection_null = {.point = {.x = INFINITY, .y = INFINITY}, .distance = INFINITY};
Color ret_black = {.r = 0, .g = 0, .b = 0};

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Intersection intersects(double x, double y, double alpha, double beta, Wall* wall){
    double a = (wall->p2.y - wall->p1.y) / (wall->p2.x - wall->p1.x);
    double b = wall->p1.y - a*wall->p1.x;
    double tg_alpha = tan(alpha);
    double pa = tg_alpha;
    double pb = y - x * tg_alpha;
    double intersection_x = (b - pb) / (pa - a);
    if(intersection_x < wall->p1.x || intersection_x > wall->p2.x) {
        return intersection_null;
    }
    
    double intersection_y = a * intersection_x + b;

    if((intersection_x - x) / cos(alpha) < 0 || (intersection_y - y) / sin(alpha) < 0 ) {
        //intersetion happened behind the viewer
        return intersection_null;
    }

    double wall_x = sqrtl(pow((intersection_y - wall->p1.y),2) + pow((intersection_x - wall->p1.x), 2));
    double dist_from_wall = sqrtl(pow((intersection_y - y),2) + pow((intersection_x - x), 2));
    double wall_y = dist_from_wall * tan(beta);

    if(wall_y > wall->top || wall_y < wall->bottom ) {
        return intersection_null;
    }

    Intersection ret = {.point = {.x = wall_x, .y = wall_y}, .distance=dist_from_wall};
    return ret;
}

Color *render_pixel(double player_x, double player_y, double player_alpha, int pixel_x, int pixel_z, int screen_w, int screen_h, Scene scene) {
    double plane_dist = (screen_w / 2.0f);
    double plane_x_offset = (screen_w / 2) - pixel_x;
    double plane_z_offset = pixel_z - (screen_h / 2);
    double alpha = atan(plane_x_offset/plane_dist) + player_alpha;
    double beta = atan(plane_z_offset/sqrt(pow(plane_x_offset,2) + pow(plane_dist,2)));

    //z-indexing ignored for now, draw the first 
    double closest_distance = INFINITY;
    Color *closest_color = &ret_black;
    for(int i = 0; i < scene.num_walls; i++) {
        Intersection intersection = intersects(player_x, player_y, alpha,beta,&(scene.walls[i]));
        if(intersection.point.x != point_null.x && intersection.point.y != point_null.y && intersection.distance < closest_distance) {
            closest_distance = intersection.distance;
            closest_color = get_color(scene.walls[i].texture,intersection.point.x,intersection.point.y);
        }
    }   
    
    return closest_color;
}
