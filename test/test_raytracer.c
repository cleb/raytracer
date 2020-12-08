#include <check.h>
#include <math.h>
#include "../src/raytracer.h"
#include "../src/angle.h"
#include "../src/wall.h"
#include "./raytracer_private_api.h"

START_TEST(test_intersects_direct)
{
    Point p1 = {.x = -100, .y = 10};
    Point p2 = {.x = 100, .y = 10};
    Line l1 = {.start = p1, .end = p2};
    Wall wall_data = {.line = l1, .bottom = -100, .top = 100};
    Render_Line render_line = {.a = 0, .b = 10};
    Render_Wall wall = {.line = render_line, .wall = &wall_data};
    Angle alpha = {.angle = M_PI_2, .cos = 0, .sin = 1, .tg = INFINITY};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg = 0};
    Intersection intersection = intersects(0, 0, 0, alpha, beta, &wall);
    ck_assert_double_eq(10, intersection.distance);
}
END_TEST

START_TEST(test_intersects_angle)
{
    Point p1 = {.x = -10, .y = -15};
    Point p2 = {.x = 10, .y = 5};
    Line l1 = {.start = p1, .end = p2};
    Wall wall_data = {.line = l1, .bottom = -100, .top = 100};
    Render_Line render_line = {.a = 1, .b = -5};
    Render_Wall wall = {.line = render_line, .wall = &wall_data};
    Angle alpha = {.angle = 0, .cos = 1, .sin = 0, .tg = 0};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg = 0};
    Intersection intersection = intersects(0, 0, 0, alpha, beta, &wall);
    ck_assert_double_eq(5, intersection.distance);
}
END_TEST

START_TEST(test_intersects_miss)
{
    Point p1 = {.x = -100, .y = 10};
    Point p2 = {.x = 100, .y = 10};
    Line l1 = {.start = p1, .end = p2};
    Wall wall_data = {.line = l1, .bottom = -100, .top = 100};
    Render_Line render_line = {.a = 0, .b = 50};
    Render_Wall wall = {.line = render_line, .wall = &wall_data};
    Angle alpha = {.angle = 0, .cos = 1, .sin = 0, .tg = 0};
    Angle beta = {.angle = 0, .cos = 1, .sin = 0, .tg = 0};
    Intersection intersection = intersects(0, 0, 0, alpha, beta, &wall);
    ck_assert_double_eq(INFINITY, intersection.distance);
}
END_TEST

START_TEST(test_create_angle_0)
{
    Angle angle = create_angle(0);
    ck_assert_double_eq(0, angle.angle);
    ck_assert_double_eq(sin(0), angle.sin);
    ck_assert_double_eq(cos(0), angle.cos);
    ck_assert_double_eq(tan(0), angle.tg);
}
END_TEST

START_TEST(test_create_angle_90)
{
    Angle angle = create_angle(M_PI_2);
    ck_assert_double_eq(M_PI_2, angle.angle);
    ck_assert_double_eq(sin(M_PI_2), angle.sin);
    ck_assert_double_eq(cos(M_PI_2), angle.cos);
    ck_assert_double_eq(tan(M_PI_2), angle.tg);
}
END_TEST

START_TEST(test_create_intersection_buffer)
{
    Intersection_Buffer *buffer = create_intersection_buffer(10);
    ck_assert_int_eq(10, buffer->size);
    destroy_intersection_buffer(buffer);
}
END_TEST

START_TEST(test_add_to_intersection_buffer)
{
    Intersection_Buffer *buffer = create_intersection_buffer(10);
    Intersection intersection = {.angle = 1, .distance = 1};
    add_to_intersection_buffer(buffer, &intersection);

    ck_assert_int_eq(1, buffer->top);
    ck_assert_double_eq(1, buffer->buffer[0].angle);
    ck_assert_double_eq(1, buffer->buffer[0].distance);
    destroy_intersection_buffer(buffer);
}

START_TEST(test_iterate_intersection_buffer)
{
    Intersection_Buffer *buffer = create_intersection_buffer(2);
    Intersection intersection = {.angle = 1, .distance = 1};
    Intersection intersection2 = {.angle = 2, .distance = 2};
    add_to_intersection_buffer(buffer, &intersection);
    add_to_intersection_buffer(buffer, &intersection2);

    Intersection_Buffer_Iterator iterator = get_intersection_buffer_iterator(buffer);

    Intersection *first = intersection_buffer_iterator_get_next(&iterator);
    Intersection *second = intersection_buffer_iterator_get_next(&iterator);
    Intersection *third = intersection_buffer_iterator_get_next(&iterator);

    ck_assert_double_eq(2, first->angle);
    ck_assert_double_eq(2, first->distance);
    ck_assert_double_eq(1, second->angle);
    ck_assert_double_eq(1, second->distance);
    ck_assert(third == NULL);
    destroy_intersection_buffer(buffer);
}

Suite *raytracer_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Raytracer");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_intersects_direct);
    tcase_add_test(tc_core, test_intersects_miss);
    tcase_add_test(tc_core, test_intersects_angle);
    tcase_add_test(tc_core, test_create_angle_0);
    tcase_add_test(tc_core, test_create_angle_90);

    tcase_add_test(tc_core, test_create_intersection_buffer);
    tcase_add_test(tc_core, test_add_to_intersection_buffer);
    tcase_add_test(tc_core, test_iterate_intersection_buffer);
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