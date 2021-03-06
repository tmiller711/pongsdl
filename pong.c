#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#define WINDOW_WIDTH (750)
#define WINDOW_HEIGHT (500)

// game variables
SDL_Rect paddle1;
SDL_Rect paddle2;
SDL_Rect ball;
int ballsize = 11;
int paddleWidth = 6;
int paddleHeight = 60;
int paddleSpeed = 5;
// keep track of ball velocity
int xvel = -2;
int yvel = -5;
SDL_Rect result;
int score1 = 0;
int score2 = 0;
SDL_Color White = {255, 255, 255};

void init()
{
    paddle1.x = 100;
    paddle1.y = (WINDOW_HEIGHT/2 - paddleHeight/2);
    paddle1.w = paddleWidth;
    paddle1.h = paddleHeight;

    paddle2.x = WINDOW_WIDTH - 100;
    paddle2.y = (WINDOW_HEIGHT/2 - paddleHeight/2);
    paddle2.w = paddleWidth;
    paddle2.h = paddleHeight;

    ball.x = (WINDOW_WIDTH / 2) - ballsize;
    ball.y = WINDOW_HEIGHT / 2 - ballsize;
    ball.w = ballsize;
    ball.h = ballsize;
}

void DrawPaddles(SDL_Renderer* rend)
{
    // collision detection with borders
    if (paddle1.y <= 0) paddle1.y = 0;
    if (paddle1.y >= WINDOW_HEIGHT - paddleHeight) paddle1.y = WINDOW_HEIGHT - paddleHeight;

    SDL_RenderDrawRect(rend, &paddle1);
    SDL_RenderDrawRect(rend, &paddle2);
    SDL_RenderFillRect(rend, &paddle1);
    SDL_RenderFillRect(rend, &paddle2);
}

void DrawBall(SDL_Renderer* rend)
{
    SDL_RenderDrawRect(rend, &ball);
    SDL_RenderFillRect(rend, &ball);
}

void BallMovement()
{
    ball.x += xvel;
    ball.y += yvel;

    // handle lower and upper bounds
    if (ball.y >= (WINDOW_HEIGHT - ball.h) || ball.y <= 0)
        yvel = -yvel;

    bool collision1 = SDL_IntersectRect(&ball, &paddle1, &result);
    bool collision2 = SDL_IntersectRect(&ball, &paddle2, &result);
    if (collision1 || collision2)
    {
        xvel = -xvel;
    }
}

void DisplayScore(TTF_Font* Sans, SDL_Renderer* rend)
{
    char score1str[2];
    sprintf(score1str, "%d", score1);
    char score2str[2];
    sprintf(score2str, "%d", score2);

    SDL_Surface* score1surface = TTF_RenderText_Solid(Sans, score1str, White);
    SDL_Texture* score1tex = SDL_CreateTextureFromSurface(rend, score1surface);
    SDL_Rect score1;
    score1.x = 100;
    score1.y = 50;
    score1.w = 85;
    score1.h = 85;

    SDL_Surface* score2surface = TTF_RenderText_Solid(Sans, score2str, White);
    SDL_Texture* score2tex = SDL_CreateTextureFromSurface(rend, score2surface);
    SDL_Rect score2;
    score2.x = WINDOW_WIDTH - 185;
    score2.y = 50;
    score2.w = 85;
    score2.h = 85;

    SDL_RenderCopy(rend, score1tex, NULL, &score1);
    SDL_RenderCopy(rend, score2tex, NULL, &score2);
}

void Score()
{
    if (ball.x > WINDOW_WIDTH)
    {
        score1 += 1;
        init();
        printf("player1 score: %i\n", score1);
        return;
    }
    if (ball.x < 0)
    {
        score2 += 1;
        init();
        SDL_Delay(1000);
        return;
    }
}

int main(void)
{
    TTF_Init();
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* win = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
        printf("error creating renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    init();

    TTF_Font* Sans = TTF_OpenFont("Sans.ttf", 20);
    if(!Sans)
    {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        return 1;
    }

    // keep track of inputs given
    int up = 0;
    int down = 0;

    int close_requested = 0;

    while (!close_requested)
    {
        // process events
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    close_requested = 1;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                            up = 1;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down = 1;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                            up = 0;
                            break;
                        case SDL_SCANCODE_DOWN:
                            down = 0;
                            break;
                    }
                    break;
            }
        }

        if (up) paddle1.y -= paddleSpeed;
        if (down) paddle1.y += paddleSpeed;

        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);

        // draw objects to screen
        DrawPaddles(rend);
        DrawBall(rend);
        BallMovement();
        Score();

        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);

        DisplayScore(Sans, rend);

        SDL_RenderPresent(rend);
        SDL_RenderClear(rend);

        // wait 1/60th of a second
        SDL_Delay(1000/60);
    }

    // clean up resources
    // clean up the resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}