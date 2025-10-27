#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

auto SPRITES_FOLDER = "images/";

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int spaceshipX = 400;
constexpr int spaceshipY = 510;
constexpr int spaceshipWidth = 50;
constexpr int spaceshipSpeed = 10;
constexpr int maxBullets = 10;
constexpr int maxEnemies = 10;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Texture* spaceshipTexture = nullptr;
SDL_Texture* enemyTexture = nullptr;

bool quit = false;

struct Bullet
{
    int x;
    int y;
    int speed;
    bool active;
};

vector<Bullet> bullets(maxBullets);

struct Enemy
{
    int x;
    int y;
    int width = 50;
    int height = 20;
    int speed;
    bool active;
};

vector<Enemy> enemies(maxEnemies);

void fireBullets()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; !bullet->active)
        {
            bullet->active = true;
            bullet->x = spaceshipX + spaceshipWidth / 2;
            bullet->y = spaceshipY;
            bullet->speed = 5;

            break;
        }
    }
}

void moveBullets()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; bullet->active)
        {
            bullet->y -= bullet->speed;
            if (bullet->y < 0)
                bullet->active = false;
        }
    }
}

void renderBullets()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; bullet->active)
        {
            SDL_Rect rect = {bullet->x - 1, bullet->y, 2, 10};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void moveAndRenderEnemies()
{
    for (int i = 0; i < maxEnemies; i++)
    {
        if (Enemy* enemy = &enemies[i]; enemy->active)
        {
            enemy->y += enemy->speed;
            if (enemy->y > SCREEN_HEIGHT)
            {
                enemy->x = rand() % SCREEN_WIDTH;
                if (enemy->x + enemy->width >= SCREEN_WIDTH)
                {
                    enemy->x = SCREEN_WIDTH - enemy->width;
                }
                enemy->y = rand() % 100 - 150;
                enemy->speed = rand() % 2 + 1;
            }

            SDL_Rect rect = {enemy->x, enemy->y, enemy->width, enemy->height};
            SDL_RenderCopy(renderer, enemyTexture, nullptr, &rect);
        }
    }
}

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

void handleEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_f)
            {
                fireBullets();
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

    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    if (currentKeyStates[SDL_SCANCODE_LEFT])
    {
        spaceshipX = spaceshipX > 10
                         ? spaceshipX - spaceshipSpeed
                         : 10;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT])
    {
        spaceshipX = (spaceshipX + spaceshipWidth < SCREEN_WIDTH - 10)
                         ? spaceshipX + spaceshipSpeed
                         : SCREEN_WIDTH - 10 - spaceshipWidth;
    }
}

void initialiseEnemies()
{
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < maxEnemies; i++)
    {
        Enemy* enemy = &enemies[i];
        enemy->x = rand() % SCREEN_WIDTH;
        enemy->y = rand() % 100 - 150;
        enemy->active = true;
        enemy->speed = rand() % 2 + 1;
    }
}

bool loadMedia()
{
    spaceshipTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("spaceship.png")).c_str());
    if (!spaceshipTexture)
    {
        cout << "IMG_LoadTexture images/spaceship.png error: " << IMG_GetError() << endl;
        return false;
    }

    enemyTexture = IMG_LoadTexture(renderer, (SPRITES_FOLDER + string("enemies.png")).c_str());
    if (!enemyTexture)
    {
        cout << "IMG_LoadTexture images/enemies.png error: " << IMG_GetError() << endl;
        return false;
    }

    return true;
}


int main()
{
    if (!initialiseSDL())
        return 1;

    if (!loadMedia())
    {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    initialiseEnemies();

    while (!quit)
    {
        handleEvents();

        //reset colour
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        //reset
        SDL_RenderClear(renderer);

        SDL_Rect spaceshipRect = {spaceshipX, spaceshipY, spaceshipWidth, spaceshipWidth};
        SDL_RenderCopy(renderer, spaceshipTexture, nullptr, &spaceshipRect);

        moveBullets();
        renderBullets();

        moveAndRenderEnemies();

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
