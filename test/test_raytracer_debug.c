#include <math.h>
#include "../src/raytracer.h"
#include "./raytracer_private_api.h"


int main(void)
{
    Point p1 = {.x = -100, .y = 10};
    Point p2 = {.x = 100, .y = 10};
    Wall wall_data = {.p1 = p1, .p2 = p2, .bottom = -100, .top = 100};
    Render_Wall wall = {.a = 0, .b = 10, .wall = &wall_data};
    Angle alpha = {.angle = M_PI_2, .cos = 0, .sin = 1, .tg= INFINITY};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg= 0};
    Intersection intersection = intersects(0,0,0,alpha,beta, &wall);
}