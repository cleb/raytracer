#include <stdlib.h>
#include <math.h>
#include "render_canvas.h"


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