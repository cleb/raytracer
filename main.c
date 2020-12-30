#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "src/raytracer.h"
#include "src/polygon_2d.h"
#include "src/texture_loader.h"
#include "src/map.h"

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
    int render_width = 160;
    int render_height = 120;

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

    Map *map = load_map("default.map");
    

    Scene scene = {.walls = map->walls, .num_walls = map->num_walls, .surfaces = map->surfaces, .num_surfaces = map->num_surfaces};
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
    destroy_map(map);
    destroy_render_canvas(canvas);
    destroy_render_scene(render_scene);
    SDL_Quit();
    return 0;
}