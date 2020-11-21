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

Angle create_angle(double angle) {
    Angle ret = {.angle = angle, .tg = tan(angle), .sin = sin(angle), .cos = cos(angle)};
    return ret;
}

void copy_point(Point *src, Point *dest) {
    dest->x = src->x;
    dest->y = src->y;
}

void copy_intersection(Intersection *src, Intersection *dest) {
    dest->distance = src->distance;
    dest->reflexivity = src->reflexivity;
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

Intersection * get_intersection_buffer(Render_Scene *scene, int index){
    return &scene->intersection_buffer[(scene->max_bounce - index) * (scene->num_walls + 1)];
}

Color trace_ray(double player_x, double player_y, double player_z, double alpha, double beta, Render_Scene *scene, int max_bounce){
    Angle alpha_angle = create_angle(alpha);
    Angle beta_angle = create_angle(beta);
    Color color = {.r = ret_black.r, .g = ret_black.g, .b=ret_black.b};
    Intersection *intersection_buffer = get_intersection_buffer(scene,max_bounce);
    #pragma omp parallel for
    for(int i = 0; i < scene->num_walls; i++) {
        intersection_buffer[i] = intersects(player_x, player_y, player_z, alpha_angle,beta_angle,&(scene->walls[i]));
    }   

    if(beta_angle.sin < 0) {
        double inverse_beta = M_PI_2 - beta_angle.angle;
        double floor_dist = tan(inverse_beta) * player_z * -1;
        double ray_floor_dist = sqrt(pow(floor_dist,2) + pow(player_z,2));
        intersection_buffer[scene->num_walls].distance = ray_floor_dist;
        Point floor_intersect = {.x = floor_dist * alpha_angle.cos + player_x, .y = floor_dist * alpha_angle.sin + player_y};
        intersection_buffer[scene->num_walls].point = floor_intersect;
        intersection_buffer[scene->num_walls].texture = scene->floor;
        intersection_buffer[scene->num_walls].angle = 0;
        intersection_buffer[scene->num_walls].reflexivity = 0;
        Point_3 floor_point_in_space = {
            .x = floor_intersect.x,
            .y = floor_intersect.y,
            .z = 0
        };
        intersection_buffer[scene->num_walls].point_in_space = floor_point_in_space;
    } else {
        copy_intersection(&intersection_null, &intersection_buffer[scene->num_walls]);
    }

    qsort(intersection_buffer,scene->num_walls + 1,sizeof(Intersection),compare_intersections);

    

    for(int i = 0; i < scene->num_walls + 1; i++) {
        Intersection *current_intersection = &intersection_buffer[i];
        if(!intersection_equals(current_intersection, &intersection_null)) {
            Color *intersection_color = get_color(current_intersection->texture,
            current_intersection->point.x,
            current_intersection->point.y);
            add_color(&color,intersection_color);            
            if(current_intersection->reflexivity > 0 && max_bounce > 0) {
                double new_alpha = 2 * current_intersection->angle - alpha;
                double new_beta = beta;

                Color reflection_color = trace_ray(
                    current_intersection->point_in_space.x + cos(new_alpha), 
                    current_intersection->point_in_space.y + sin(new_alpha), 
                    current_intersection->point_in_space.z, 
                    new_alpha,
                    new_beta, 
                    scene,
                    max_bounce - 1
                );
                reflection_color.alpha = current_intersection->reflexivity * 255;
                add_color(&color, &reflection_color);
            }
        }
    }
    
    return color;
}

Color render_pixel(double player_x, double player_y, double player_z, double player_alpha, int pixel_x, int pixel_z, Render_Canvas *canvas, Render_Scene *scene) {
    double plane_dist = canvas->plane_dist;
    double alpha = canvas->alpha[pixel_x] + player_alpha;
    double beta = canvas->beta[pixel_z * canvas->screen_w + pixel_x];

    return trace_ray(player_x, player_y, player_z, alpha, beta, scene, scene->max_bounce);
}

Render_Scene *create_render_scene(Scene *scene) {
    Render_Scene *ret = (Render_Scene *)malloc(sizeof(Render_Scene));
    ret->num_walls = scene->num_walls;
    ret->walls = (Render_Wall *)malloc(scene->num_walls * sizeof(Render_Wall));
    ret->max_bounce = 2;
    ret->intersection_buffer = (Intersection *)malloc((scene->num_walls + 1) * (ret->max_bounce + 1) * sizeof(Intersection));
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

