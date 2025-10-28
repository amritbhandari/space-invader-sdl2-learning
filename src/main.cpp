#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()

using namespace std;

auto SPRITES_FOLDER = "images/";

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

constexpr int spaceshipWidth = 50;
constexpr int spaceshipHeight = 80;
constexpr int spaceshipXLeftBoundary = 10;
constexpr int spaceshipXRightBoundary = SCREEN_WIDTH - spaceshipWidth - 10;
int spaceshipX = (SCREEN_WIDTH - spaceshipWidth) / 2;
constexpr int spaceshipY = 510;
constexpr int spaceshipSpeed = 10;

constexpr int bulletWidth = 2;
constexpr int bulletHeight = 10;
constexpr int bulletSpeed = 5;
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
    int speed = bulletSpeed;
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
    int health;
};

vector<Enemy> enemies(maxEnemies);

void fireBullets()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; !bullet->active)
        {
            bullet->x = spaceshipX + (spaceshipWidth / 2);
            bullet->y = spaceshipY;
            bullet->active = true;

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

void moveAndRenderBullets()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; bullet->active)
        {
            bullet->y -= bullet->speed;
            if (bullet->y > 0) // within the top of screen
            {
                SDL_Rect rect = {bullet->x - 1, bullet->y, bulletWidth, bulletHeight};
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // red
                SDL_RenderFillRect(renderer, &rect);
            }
            else
                bullet->active = false;
        }
    }
}

void spawnEnemyNewPlace(Enemy& enemy)
{
    int x = rand() % SCREEN_WIDTH;
    if (x + enemy.width > SCREEN_WIDTH)
    {
        x = SCREEN_WIDTH - enemy.width;
    }

    enemy.x = x;
    enemy.y = rand() % 100; // - 150;

    enemy.speed = rand() % 2 + 1;
    enemy.health = rand() % 3 + 1;

    enemy.active = true;
}

void initialiseEnemies()
{
    for (int i = 0; i < maxEnemies; i++)
    {
        spawnEnemyNewPlace(enemies[i]);
    }
}

bool checkCollision(const SDL_Rect& rect1, const SDL_Rect& rect2)
{
    return SDL_HasIntersection(&rect1, &rect2);
}

void checkBulletsAndEnemiesCollision()
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (Bullet* bullet = &bullets[i]; bullet->active)
        {
            SDL_Rect bulletRect = {bullet->x - 1, bullet->y, bulletWidth, bulletHeight};
            for (int j = 0; j < maxEnemies; j++)
            {
                if (Enemy* enemy = &enemies[j]; enemy->active)
                {
                    SDL_Rect enemyRect = {enemy->x, enemy->y, enemy->width, enemy->height};

                    if (checkCollision(bulletRect, enemyRect))
                    {
                        bullet->active = false;

                        enemy->health--;
                        enemy->active = enemy->health > 0;
                    }
                }
            }
        }
    }
}

bool checkShipAndEnemiesCollision(const SDL_Rect& rect)
{
    for (int j = 0; j < maxEnemies; j++)
    {
        if (Enemy* enemy = &enemies[j]; enemy->active)
        {
            SDL_Rect enemyRect = {enemy->x, enemy->y, enemy->width, enemy->height};

            if (checkCollision(rect, enemyRect))
            {
                enemy->health = 0;
                enemy->active = false;

                return true;
            }
        }
    }

    return false;
}

void moveAndRenderEnemies()
{
    for (int i = 0; i < maxEnemies; i++)
    {
        Enemy* enemy = &enemies[i];
        if (enemy->active)
        {
            enemy->y += enemy->speed;
        }

        if (enemy->y > SCREEN_HEIGHT || !enemy->active)
        {
            spawnEnemyNewPlace(*enemy);
        }

        SDL_Rect rect = {enemy->x, enemy->y, enemy->width, enemy->height};
        SDL_RenderCopy(renderer, enemyTexture, nullptr, &rect);
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
    if (currentKeyStates[SDL_SCANCODE_LEFT]) // left arrow key
    {
        spaceshipX = spaceshipX > spaceshipXLeftBoundary
                         ? spaceshipX - spaceshipSpeed // within left boundary
                         : spaceshipXLeftBoundary;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT]) // right arrow key
    {
        spaceshipX = (spaceshipX < spaceshipXRightBoundary)
                         ? spaceshipX + spaceshipSpeed // within right boundary
                         : spaceshipXRightBoundary;
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

    srand(time(nullptr));

    initialiseEnemies();

    while (!quit)
    {
        handleEvents();

        //reset colour
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        //reset
        SDL_RenderClear(renderer);

        // render spaceship
        SDL_Rect spaceshipRect = {spaceshipX, spaceshipY, spaceshipWidth, spaceshipHeight};
        SDL_RenderCopy(renderer, spaceshipTexture, nullptr, &spaceshipRect);

        moveAndRenderBullets();
        moveAndRenderEnemies();

        checkBulletsAndEnemiesCollision();
        if (checkShipAndEnemiesCollision(spaceshipRect))
        {
            quit = true;
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
