#include <check.h>
#include "test_raytracer.h"
#include "test_render_line.h"
#include "test_render_polygon_2d.h"

int main(void)
{
    int number_failed;
    Suite *raytracer_s;
    Suite *render_line_s;
    Suite *render_polygon_2d_s;
    SRunner *sr;

    raytracer_s = raytracer_suite();
    render_line_s = render_line_suite();
    render_polygon_2d_s = render_polygon_2d_suite();
    sr = srunner_create(raytracer_s);
    srunner_add_suite(sr, render_line_s);
    srunner_add_suite(sr, render_polygon_2d_s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? 0 : 1;
}