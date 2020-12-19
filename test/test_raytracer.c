#include <check.h>
#include <math.h>
#include <stdlib.h>
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

Render_Polygon_2D create_intersetcion_test_polygin_2d()
{
    Line *lines = (Line *)malloc(4 * sizeof(Line));
    Render_Line *render_lines = (Render_Line *)malloc(4 * sizeof(Render_Line));
    Point p1 = {.x = 0, .y = 0};
    Point p2 = {.x = 10, .y = 0};
    Point p3 = {.x = 10, .y = 20};
    Point p4 = {.x = 0, .y = 10};
    Line l1 = {.start = p1, .end = p2};
    Line l2 = {.start = p2, .end = p3};
    Line l3 = {.start = p4, .end = p3};
    Line l4 = {.start = p4, .end = p1};
    lines[0] = l1;
    lines[1] = l2;
    lines[2] = l3;
    lines[3] = l4;
    Render_Line render_line = {.a = 0, .b = 0, .line = &lines[0]};
    Render_Line render_line2 = {.a = INFINITY, .b = INFINITY, .line = &lines[1]};
    Render_Line render_line3 = {.a = 1, .b = 10, .line = &lines[2]};
    Render_Line render_line4 = {.a = INFINITY, .b = INFINITY, .line = &lines[3]};
    render_lines[0] = render_line;
    render_lines[1] = render_line2;
    render_lines[2] = render_line3;
    render_lines[3] = render_line4;
    Render_Polygon_2D polygon = {.lines = render_lines, .numlines = 4};
    return polygon;
}

void destroy_intersection_polygon_2d(Render_Polygon_2D *polygon)
{
    free(polygon->lines[0].line);
    free(polygon->lines);
}

START_TEST(test_intersects_polygon_hit)
{

    Point point = {.x = 2, .y = 2};
    Render_Polygon_2D polygon = create_intersetcion_test_polygin_2d();

    int hit = Intersects_polygon_2d(&polygon, point);
    destroy_intersection_polygon_2d(&polygon);

    ck_assert_int_eq(1, hit);
}

START_TEST(test_intersects_polygon_miss_left)
{
    Render_Polygon_2D polygon = create_intersetcion_test_polygin_2d();
    Point point = {.x = -3, .y = 2};

    int hit = Intersects_polygon_2d(&polygon, point);

    destroy_intersection_polygon_2d(&polygon);

    ck_assert_int_eq(0, hit);
}
END_TEST

START_TEST(test_intersects_polygon_miss_above)
{
    Render_Polygon_2D polygon = create_intersetcion_test_polygin_2d();
    Point point = {.x = 2, .y = 20};

    int hit = Intersects_polygon_2d(&polygon, point);

    destroy_intersection_polygon_2d(&polygon);

    ck_assert_int_eq(0, hit);
}
END_TEST

START_TEST(test_intersects_polygon_miss_below)
{
    Render_Polygon_2D polygon = create_intersetcion_test_polygin_2d();
    Point point = {.x = 2, .y = -1};

    int hit = Intersects_polygon_2d(&polygon, point);

    destroy_intersection_polygon_2d(&polygon);

    ck_assert_int_eq(0, hit);
}
END_TEST

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

    tcase_add_test(tc_core, test_intersects_polygon_hit);
    tcase_add_test(tc_core, test_intersects_polygon_miss_left);
    tcase_add_test(tc_core, test_intersects_polygon_miss_above);
    tcase_add_test(tc_core, test_intersects_polygon_miss_below);

    suite_add_tcase(s, tc_core);

    return s;
}