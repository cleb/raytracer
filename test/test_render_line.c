#include <check.h>
#include <math.h>
#include "../src/render_line.h"
#include "../src/angle.h"
#include "../src/wall.h"

START_TEST(test_create_line)
{
    Point p1 = {.x = 0, .y = 2};
    Point p2 = {.x = 100, .y = 102};
    Line l1 = {.start = p1, .end = p2};
    Render_Line render_line = create_render_line(l1);

    ck_assert_double_eq(1, render_line.a);
    ck_assert_double_eq(2, render_line.b);
    ck_assert_double_eq(M_PI_4, render_line.angle);
}
END_TEST

Suite *render_line_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Render line");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_create_line);
    suite_add_tcase(s, tc_core);

    return s;
}