#include <SDL2/SDL.h>


void render(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, rand() % 255, rand() % 255, rand() % 255, 255);
    SDL_RenderDrawPoint(renderer, rand() % 320, rand() % 240);
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
    while (active) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            active = 0;
        render(renderer);
    }
    SDL_Quit();
    return 0;
}