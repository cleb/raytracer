#include <SDL2/SDL.h>

#include "src/raytracer.h"
#include "src/texture_loader.h"



void render(SDL_Renderer *renderer, double x, double y, double alpha, Render_Scene *scene) {
    Color colors[320][240];
    for(int s_y = 0; s_y < 240; s_y++) {
        #pragma omp parallel for
        for(int s_x = 0; s_x < 320; s_x++) {
            colors[s_x][s_y] = render_pixel(x,y,alpha,s_x,s_y,320,240,scene);            
        }
    }

    for(int s_y = 0; s_y < 240; s_y++) {
            #pragma omp parallel for
            for(int s_x = 0; s_x < 320; s_x++) {
                SDL_SetRenderDrawColor(renderer, colors[s_x][s_y].r, colors[s_x][s_y].g, colors[s_x][s_y].b, 255);
                SDL_RenderDrawPoint(renderer, s_x, s_y);
            }
    }
    
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    int active = 1;
    SDL_Event event;
    SDL_Renderer *renderer;
    SDL_Window *window;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(320, 240, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "raytracer");

    Texture *wall = load_texture("./wall.jpg");
    Texture *glass = load_texture("./glass.png");

    Point p1 = {.x = -400, .y = 400};
    Point p2 = {.x = 400, .y = 400};

    Point p3 = {.x = -400, .y = 800};
    Point p4 = {.x = 400, .y = 800};

    Point p5 = {.x = -400, .y = 400};
    Point p6 = {.x = -400, .y = 800};

    Point p7 = {.x = 400, .y = 400};
    Point p8 = {.x = 400, .y = 800};

    Wall w1 = {.p1 = p1, .p2 = p2, .bottom = -200, .top = 200, .texture = glass};
    Wall w2 = {.p1 = p3, .p2 = p4, .bottom = -200, .top = 200, .texture = wall};
    Wall w3 = {.p1 = p5, .p2 = p6, .bottom = -200, .top = 200, .texture = wall};
    Wall w4 = {.p1 = p7, .p2 = p8, .bottom = -200, .top = 200, .texture = wall};
    Wall scene_walls[4] = {w1,w2, w3, w4};
    Scene scene = {.walls = scene_walls, .num_walls = 4};
    Render_Scene *render_scene = create_render_scene(&scene);
    double x = 0;
    double y = 0;
    double alpha = ANGLE_90;
    
    
    while (active) {
        if (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    active = 0;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case 'w':
                            x = x + cos(alpha) * 10;
                            y = y + sin(alpha) * 10;
                            break;
                        case 'a':
                            alpha += 0.01f;
                            break;
                        case 'd':
                            alpha -= 0.01f;
                            break;
                        case 's':
                            x = x - cos(alpha) * 10;
                            y = y - sin(alpha) * 10;
                            break;
                    }
                break;
            }
        }

        render(renderer,x,y,alpha,render_scene);
    }
    destroy_render_scene(render_scene);
    destroy_texture(wall);
    SDL_Quit();
    return 0;
}