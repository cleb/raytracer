#include <SDL2/SDL.h>
#include "src/raytracer.h"



void render(SDL_Renderer *renderer, double x, double y, double alpha, Scene scene) {
    for(int s_y = 0; s_y < 240; s_y++) {
        for(int s_x = 0; s_x < 320; s_x++) {
            Color color = render_pixel(x,y,alpha,s_x,s_y,320,240,scene);
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
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

    Point p1 = {.x = -200, .y = 1200};
    Point p2 = {.x = 200, .y = 1200};
    Wall w1 = {.p1 = p1, .p2 = p2, .bottom = -200, .top = 200};
    Wall scene_walls[1] = {w1};
    Scene scene = {.walls = scene_walls, .num_walls = 1};
    double x = 0;
    double y = 0;
    double alpha = ANGLE_90;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(320, 240, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "raytracer");
    while (active) {
        if (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    active = 0;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case 'w':
                            x = x + cos(alpha);
                            y = y + sin(alpha);
                            break;
                        case 'a':
                            alpha -= 0.01f;
                            break;
                        case 'd':
                            alpha += 0.01f;
                            break;
                        case 's':
                            x = x - cos(alpha);
                            y = y - sin(alpha);
                            break;
                    }
                    printf("x: %lf y: %lf alpha: %lf \n", x, y, alpha);
                break;
            }
        }
            
        render(renderer,x,y,alpha,scene);
    }
    SDL_Quit();
    return 0;
}