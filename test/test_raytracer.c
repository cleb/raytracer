#include <check.h>
#include <math.h>
#include "../src/raytracer.h"
#include "./raytracer_private_api.h"

START_TEST(test_intersects_direct)
{
    Point p1 = {.x = -100, .y = 10};
    Point p2 = {.x = 100, .y = 10};
    Wall wall_data = {.p1 = p1, .p2 = p2, .bottom = -100, .top = 100};
    Render_Wall wall = {.a = 0, .b = 10, .wall = &wall_data};
    Angle alpha = {.angle = M_PI_2, .cos = 0, .sin = 1, .tg= INFINITY};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg= 0};
    Intersection intersection = intersects(0,0,0,alpha,beta, &wall);
    ck_assert_double_eq(10,intersection.distance);
}
END_TEST

START_TEST(test_intersects_angle)
{
    Point p1 = {.x = -10, .y = -15};
    Point p2 = {.x = 10, .y = 5};
    Wall wall_data = {.p1 = p1, .p2 = p2, .bottom = -100, .top = 100};
    Render_Wall wall = {.a = 1, .b = -5, .wall = &wall_data};
    Angle alpha = {.angle = 0, .cos = 1, .sin = 0, .tg= 0};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg= 0};
    Intersection intersection = intersects(0,0,0,alpha,beta, &wall);
    ck_assert_double_eq(5,intersection.distance);
}
END_TEST

START_TEST(test_intersects_miss)
{
    Point p1 = {.x = -100, .y = 10};
    Point p2 = {.x = 100, .y = 10};
    Wall wall_data = {.p1 = p1, .p2 = p2, .bottom = -100, .top = 100};
    Render_Wall wall = {.a = 0, .b = 50, .wall = &wall_data};
    Angle alpha = {.angle = 0, .cos = 1, .sin = 0, .tg= 0}; 
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg= 0};
    Intersection intersection = intersects(0,0,0,alpha,beta, &wall);
    ck_assert_double_eq(INFINITY, intersection.distance);
}
END_TEST


Suite * raytracer_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Raytracer");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_intersects_direct);
    tcase_add_test(tc_core, test_intersects_miss);
    tcase_add_test(tc_core, test_intersects_angle);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
   int number_failed;
   Suite *s;
   SRunner *sr;

   s = raytracer_suite();
   sr = srunner_create(s);

   srunner_run_all(sr, CK_NORMAL);
   number_failed = srunner_ntests_failed(sr);
   srunner_free(sr);
   return (number_failed == 0) ? 0 : 1;
}