#include <math.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#include "raytracer.h"

Point point_null = {.x = INFINITY, .y = INFINITY};

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Point intersects(double x, double y, double alpha, double beta, Wall* wall){
    double a = (wall->p2.y - wall->p1.y) / (wall->p2.x - wall->p1.x);
    double b = wall->p1.y - a*wall->p1.x;
    double tg_alpha = tan(alpha);
    double pa = tg_alpha;
    double pb = y - x * tg_alpha;
    double intersection_x = (b - pb) / (pa - a);
    if(intersection_x < wall->p1.x || intersection_x > wall->p2.x) {
        return point_null;
    }
    
    double intersection_y = a * intersection_x + b;

    if((intersection_x - x) / cos(alpha) < 0 || (intersection_y - y) / sin(alpha) < 0 ) {
        //intersetion happened behind the viewer
        return point_null;
    }

    double wall_x = sqrtl(pow((intersection_y - wall->p1.y),2) + pow((intersection_x - wall->p1.x), 2));
    double dist_from_wall = sqrtl(pow((intersection_y - y),2) + pow((intersection_x - x), 2));
    double wall_y = dist_from_wall * tan(beta);

    if(wall_y > wall->top || wall_y < wall->bottom ) {
        return point_null;
    }

    Point ret = {.x = wall_x, .y = wall_y};
    return ret;
}

Color render_pixel(double player_x, double player_y, double player_alpha, int pixel_x, int pixel_y, int screen_w, int screen_h, Scene scene) {
    double plane_dist = (screen_w / 2.0f);
    double alpha = atan((pixel_x - (screen_w / 2))/plane_dist) + player_alpha;
    double beta = atan((pixel_y - (screen_h / 2))/plane_dist);

    //z-indexing ignored for now, draw the first 
    for(int i = 0; i < scene.num_walls; i++) {
        Point intersection = intersects(player_x,player_y,alpha,beta,&(scene.walls[i]));
        if(intersection.x != point_null.x && intersection.y != point_null.y) {
            Color ret_blue = {.r = 0, .g = 0, .b = 255};
            return ret_blue;
        }
    }   
    Color ret_black = {.r = 0, .g = 0, .b = 0};
    return ret_black;
}
