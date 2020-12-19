#include "../src/raytracer.h"
#include "../src/render_polygon_2d.h"

Intersection intersects(double x, double y, double z, Angle alpha, Angle beta, Render_Wall *wall);

int Intersects_polygon_2d(Render_Polygon_2D *polygon, Point point);