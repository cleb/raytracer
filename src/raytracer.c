#include <math.h>
#include "wall.h"
#include "point.h"
#define ANGLE_90 (M_PI / 2)
#define ANGLE_180 M_PI
#define ANGLE_270 (M_PI * 3 / 2)

// returns the coordinates where the ray hit the wall if it hits, point_null otherwise
Point intersects(double x, double y, double alpha, double beta, Wall* wall){
    double a = (wall->p2.y - wall->p1.y) / (wall->p2.x - wall->p1.x);
    double b = wall->p1.y - a*wall->p1.x;
    double tg_alpha = tan(alpha);
    double pa = tg_alpha;
    double pb = x * tg_alpha - y;
    double intersection_x = (b - pb) / (pa - a);
    if(intersection_x < wall->p1.x || intersection_x > wall->p2.x) {
        return point_null;
    }
    
    double intersection_y = a * intersection_x + b;
    double wall_x = sqrtl(pow((intersection_y - wall->p1.y),2) + pow((intersection_x - wall->p1.x), 2));
    double dist_from_wall = sqrtl(pow((intersection_y - y),2) + pow((intersection_x - x), 2));
    double wall_y = dist_from_wall * tan(beta);

    if(wall_y > wall->height) {
        return point_null;
    }

    Point ret = {.x = wall_x, .y = wall_y};
    return ret;
}
