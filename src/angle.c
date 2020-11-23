#include "angle.h"

Angle create_angle(double angle) {
    Angle ret = {.angle = angle, .tg = tan(angle), .sin = sin(angle), .cos = cos(angle)};
    return ret;
}