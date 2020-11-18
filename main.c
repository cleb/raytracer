#include <SDL2/SDL.h>

#include "src/raytracer.h"
#include "src/texture_loader.h"



void render(SDL_Renderer *renderer, double x, double y, double alpha, Render_Canvas *canvas, Render_Scene *scene) {
    Color colors[320][240];
    for(int s_y = 0; s_y < 240; s_y++) {
        #pragma omp parallel for
        for(int s_x = 0; s_x < 320; s_x++) {
            colors[s_x][s_y] = render_pixel(x,y,100.0f,alpha,s_x,s_y, canvas,scene);            
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
    Texture *floor = load_texture("./floor.jpg");
    Texture *glass = load_texture("./glass.png");

    Point p1 = {.x = -400, .y = 400};
    Point p2 = {.x = 400, .y = 400};

    Point p3 = {.x = -400, .y = 800};
    Point p4 = {.x = 400, .y = 800};

    Point p5 = {.x = -400, .y = 400};
    Point p6 = {.x = -400, .y = 800};

    Point p7 = {.x = 400, .y = 400};
    Point p8 = {.x = 400, .y = 800};

    Wall w1 = {.p1 = p1, .p2 = p2, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w2 = {.p1 = p3, .p2 = p4, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall w3 = {.p1 = p5, .p2 = p6, .bottom = 0, .top = 200, .texture = glass, .reflexivity = 0.5};
    Wall w4 = {.p1 = p7, .p2 = p8, .bottom = 0, .top = 200, .texture = wall, .reflexivity = 0};
    Wall scene_walls[4] = {w1,w2, w3, w4};
    Scene scene = {.walls = scene_walls, .num_walls = 4, .floor = floor};
    Render_Scene *render_scene = create_render_scene(&scene);
    Render_Canvas *canvas = create_render_canvas(320,240);


    double x = 0;
    double y = 600;
    double alpha = ANGLE_270;
    
    
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

        render(renderer,x,y,alpha,canvas,render_scene);
    }
    destroy_render_canvas(canvas);
    destroy_render_scene(render_scene);
    destroy_texture(wall);
    destroy_texture(glass);
    destroy_texture(floor);
    SDL_Quit();
    return 0;
}