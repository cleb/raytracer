#include <SDL2/SDL.h>
#include "src/raytracer.h"
#include "src/texture_loader.h"



void render(SDL_Renderer *renderer, double x, double y, double alpha, Scene scene) {
    Color *colors[320][240];
    for(int s_y = 0; s_y < 240; s_y++) {
        #pragma omp parallel for
        for(int s_x = 0; s_x < 320; s_x++) {
            colors[s_x][s_y] = render_pixel(x,y,alpha,s_x,s_y,320,240,scene);            
        }
    }

    for(int s_y = 0; s_y < 240; s_y++) {
            #pragma omp parallel for
            for(int s_x = 0; s_x < 320; s_x++) {
                SDL_SetRenderDrawColor(renderer, colors[s_x][s_y]->r, colors[s_x][s_y]->g, colors[s_x][s_y]->b, 255);
                SDL_RenderDrawPoint(renderer, s_x, s_y);
            }
    }

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, scene.walls[0].p1.x / 100 + 100,scene.walls[0].p1.y / 100 + 100,scene.walls[0].p2.x / 100 + 100,scene.walls[0].p2.y / 100 + 100);
    SDL_RenderDrawLine(renderer, x / 100 + 100,y / 100 + 100,(x + 1000* cos(alpha + M_PI_4)) / 100 + 100,(y + 1000* sin(alpha + M_PI_4)) /100 + 100);
    SDL_RenderDrawLine(renderer, x / 100 + 100,y / 100 + 100,(x + 1000* cos(alpha - M_PI_4)) /100 + 100,(y + 1000* sin(alpha - M_PI_4)) /100 + 100);

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

    Point p1 = {.x = -400, .y = 400};
    Point p2 = {.x = 400, .y = 400};

    Point p3 = {.x = -400, .y = 600};
    Point p4 = {.x = 400, .y = 600};
    Wall w1 = {.p1 = p1, .p2 = p2, .bottom = -200, .top = 200, .texture = wall};
    Wall w2 = {.p1 = p3, .p2 = p4, .bottom = -200, .top = 200, .texture = wall};
    Wall scene_walls[2] = {w1,w2};
    Scene scene = {.walls = scene_walls, .num_walls = 2};
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

        render(renderer,x,y,alpha,scene);
    }
    destroy_texture(wall);
    SDL_Quit();
    return 0;
}