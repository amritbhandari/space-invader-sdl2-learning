#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

auto SPRITES_FOLDER = "images/";
constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 400;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool quit = false;

bool initialiseSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return false;

    window = SDL_CreateWindow("Space Invader",
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (!window)
    {
        cout << "SDL_CreateWindow error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        cout << "SDL_CreateRenderer error: " << SDL_GetError() << endl;
        SDL_Quit();
        return false;
    }

    return true;
}

bool handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_SPACE)
            {
                // HandleJump();
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYUP:
            // if (gameOver)
            // {
            //     if (event.key.keysym.sym == SDLK_SPACE)
            //         ResetGame();
            // }
            break;
        }
    }
}

int main()
{
    if (!initialiseSDL())
        return 1;

    while (!quit)
    {
        handleEvents();
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
