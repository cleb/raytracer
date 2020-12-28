#include <check.h>
#include <math.h>
#include "../src/render_polygon_2d.h"

START_TEST(test_create)
{
    Point p1 = {.x = -400, .y = 400};
    Point p2 = {.x = 400, .y = 400};
    Line l1 = {.start = p1, .end = p2};

    Point p3 = {.x = -400, .y = 800};
    Point p4 = {.x = 400, .y = 800};
    Line l2 = {.start = p3, .end = p4};

    Point p5 = {.x = -400, .y = 400};
    Point p6 = {.x = -400, .y = 800};
    Line l3 = {.start = p5, .end = p6};

    Point p7 = {.x = 400, .y = 400};
    Point p8 = {.x = 400, .y = 800};
    Line l4 = {.start = p7, .end = p8};

    Line lines[4] = {l1, l2, l3, l4};
    Polygon_2D polygon = {.lines = lines, .numLines = 4};

    Render_Polygon_2D *render_polygon = create_render_polygon_2d(&polygon);
    ck_assert_int_eq(4, render_polygon->numlines);
    ck_assert_double_eq(0, render_polygon->lines[0].a);
    destroy_render_polygon_2d(render_polygon);
}
END_TEST

Suite *render_polygon_2d_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Render polygon 2d");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_create);
    suite_add_tcase(s, tc_core);

    return s;
}