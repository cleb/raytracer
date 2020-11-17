#pragma once
#include <math.h>
#include "wall.h"
#include "point.h"
#include "color.h"
#include "scene.h"
#define ANGLE_90 (M_PI / 2)
#define ANGLE_180 M_PI
#define ANGLE_270 (M_PI * 3 / 2)

Color *render_pixel(double player_x, double player_y, double player_alpha, int pixel_x, int pixel_y, int screen_w, int screen_h, Scene scene);
