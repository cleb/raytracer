#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "src/raytracer.h"
#include "src/polygon_2d.h"
#include "src/texture_loader.h"

void render(SDL_Renderer *renderer, double x, double y, double alpha, Render_Canvas *canvas, Render_Scene *scene)
{
    Color **colors = canvas->colors;
    #pragma omp parallel for
    for (int s_y = 0; s_y < canvas->screen_h; s_y++)
    {
        for (int s_x = 0; s_x < canvas->screen_w; s_x++)
        {
            colors[s_y][s_x] = render_pixel(x, y, 100.0f, alpha, s_x, s_y, canvas, scene);
        }
    }

    for (int s_y = 0; s_y < canvas->screen_h; s_y++)
    {
        for (int s_x = 0; s_x < canvas->screen_w; s_x++)
        {
            SDL_SetRenderDrawColor(renderer, colors[s_y][s_x].r, colors[s_y][s_x].g, colors[s_y][s_x].b, 255);
            SDL_RenderDrawPoint(renderer, s_x, s_y);
        }
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    int render_width = 320;
    int render_height = 240;

    int window_width = 1024;
    int window_height = 768;

    int active = 1;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window, &renderer);
    SDL_RenderSetLogicalSize(renderer, render_width, render_height);

    SDL_SetWindowTitle(window, "raytracer");

    Texture *wall = load_texture("./wall.jpg");
    Texture *texture_floor = load_texture("./floor.jpg");
    Texture *grass = load_texture("./grass.jpg");
    Texture *glass = load_texture("./glass.png");

    //scene geometry
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

    Point fp1 = {.x = -1000, .y = -1000};
    Point fp2 = {.x = 1000, .y = -1000};
    Line fl1 = {.start = fp1, .end = fp2};

    Point fp3 = {.x = -1000, .y = 400};
    Point fp4 = {.x = 1000, .y = 400};
    Line fl2 = {.start = fp3, .end = fp4};

    Point fp5 = {.x = -1000, .y = -1000};
    Point fp6 = {.x = -1000, .y = 400};
    Line fl3 = {.start = fp5, .end = fp6};

    Point fp7 = {.x = 1000, .y = -1000};
    Point fp8 = {.x = 1000, .y = 400};
    Line fl4 = {.start = fp7, .end = fp8};

    Point f2p1 = {.x = 400, .y = -1000};
    Point f2p2 = {.x = 1000, .y = -1000};
    Line f2l1 = {.start = f2p1, .end = f2p2};

    Point f2p3 = {.x = 400, .y = 1000};
    Point f2p4 = {.x = 1000, .y = 1000};
    Line f2l2 = {.start = f2p3, .end = f2p4};

    Point f2p5 = {.x = 400, .y = -1000};
    Point f2p6 = {.x = 400, .y = 1000};
    Line f2l3 = {.start = f2p5, .end = f2p6};

    Point f2p7 = {.x = 1000, .y = -1000};
    Point f2p8 = {.x = 1000, .y = 1000};
    Line f2l4 = {.start = f2p7, .end = f2p8};

    Point f3p1 = {.x = -1000, .y = -1000};
    Point f3p2 = {.x = 400, .y = -1000};
    Line f3l1 = {.start = f3p1, .end = f3p2};

    Point f3p3 = {.x = -1000, .y = 1000};
    Point f3p4 = {.x = 400, .y = 1000};
    Line f3l2 = {.start = f3p3, .end = f3p4};

    Point f3p5 = {.x = -1000, .y = -1000};
    Point f3p6 = {.x = -1000, .y = 1000};
    Line f3l3 = {.start = f3p5, .end = f3p6};

    Point f3p7 = {.x = 400, .y = -1000};
    Point f3p8 = {.x = 400, .y = 1000};
    Line f3l4 = {.start = f3p7, .end = f3p8};

    

    Wall w1 = {.line = l1, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w2 = {.line = l2, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall w3 = {.line = l3, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w4 = {.line = l4, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall scene_walls[4] = {w1, w2, w3, w4};

    Line floor_lines[4] = {l1, l2, l3, l4};
    Polygon_2D polygon = {.lines = floor_lines, .numLines = 4};
    Surface floor_inside = {.polygon = &polygon, .texture = texture_floor};

    Line floor_outside_lines[4] = {fl1, fl2, fl3, fl4};
    Polygon_2D floor_outside_polygon = {.lines = floor_outside_lines, .numLines = 4};
    Surface floor_outside = {.polygon = &floor_outside_polygon, .texture = grass};

    Line floor_outside_lines_2[4] = {f2l1, f2l2, f2l3, f2l4};
    Polygon_2D floor_outside_polygon_2 = {.lines = floor_outside_lines_2, .numLines = 4};
    Surface floor_outside_2 = {.polygon = &floor_outside_polygon_2, .texture = grass};

    Line floor_outside_lines_3[4] = {f3l1, f3l2, f3l3, f3l4};
    Polygon_2D floor_outside_polygon_3 = {.lines = floor_outside_lines_3, .numLines = 4};
    Surface floor_outside_3 = {.polygon = &floor_outside_polygon_3, .texture = grass};

    Surface scene_floors[4] = {floor_outside, floor_outside_2, floor_outside_3, floor_inside};

    Scene scene = {.walls = scene_walls, .num_walls = 4, .surface = grass, .surfaces = scene_floors, .num_surfaces = 4};
    //end scene geometry

    Render_Scene *render_scene = create_render_scene(&scene);
    Render_Canvas *canvas = create_render_canvas(render_width, render_height);

    double x = 0;
    double y = 600;
    double alpha = ANGLE_270;

    int ticks = 0;

    while (active)
    {
        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                active = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case 'w':
                    x = x + cos(alpha) * 10;
                    y = y + sin(alpha) * 10;
                    break;
                case 'a':
                    alpha += 0.1f;
                    break;
                case 'd':
                    alpha -= 0.1f;
                    break;
                case 's':
                    x = x - cos(alpha) * 10;
                    y = y - sin(alpha) * 10;
                    break;
                }
                break;
            }
        }

        int now_ticks = SDL_GetTicks();
        if (now_ticks - ticks > 60)
        {
            render(renderer, x, y, alpha, canvas, render_scene);
            ticks = now_ticks;
        }
    }
    destroy_render_canvas(canvas);
    destroy_render_scene(render_scene);
    destroy_texture(wall);
    destroy_texture(glass);
    destroy_texture(texture_floor);
    destroy_texture(grass);
    SDL_Quit();
    return 0;
}