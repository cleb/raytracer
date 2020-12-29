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

    Wall w1 = {.line = l1, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w2 = {.line = l2, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall w3 = {.line = l3, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w4 = {.line = l4, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall scene_walls[4] = {w1, w2, w3, w4};

    Line floor_lines[4] = {l1, l2, l3, l4};
    Polygon_2D polygon = {.lines = floor_lines, .numLines = 4};
    Surface surface = {.polygon = &polygon, .texture = texture_floor};
    Surface scene_floors[1] = {surface};

    Scene scene = {.walls = scene_walls, .num_walls = 4, .surface = grass, .surfaces = scene_floors, .num_surfaces = 1};

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