#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

const int WIDTH = 640;
const int HEIGHT = 480;
const int ballSize = 10;

typedef struct ball
{
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    int size;
}ball;

typedef struct player
{
    int score;
    float yPosition;
}player;

player player1;
player player2;

const int paddleWidth = 20;
const int paddleHeight = 75;
const int paddleGap = 10;

bool served = false;

const float paddleMoveSpeed = 150.0f;

ball variableBall;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool initialize(void);
void update(float);
void shutdown(void);
void renderBall(const ball*);
ball makeBall(int size);
void updateBall(ball *Ball, float elapsed);
void updateScore(int player, int points);

player makePlayer(void);
void updatePaddles(float elapsed);
void renderPaddles(void);

int main(int argc, char *argv[])
{
    srand(time(NULL));
    atexit(shutdown);
    //Calls initialize function and exits if it does not run
    if (!initialize())
    {
        exit(1);
    }
 
    SDL_Event event;
    bool quit = false;
    Uint32 lastTick = SDL_GetTicks();
    while (!quit)
    {
        while(SDL_PollEvent(&event))
        {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
    }
    //Runs while quit is not (!) false 
    Uint32 currentTick = SDL_GetTicks();
    Uint32 difference = currentTick - lastTick;
    float elapsed = difference / 1000.0f;
    update(elapsed); 
    lastTick = currentTick;
}
    SDL_Quit();

    return 0;
}

//Function creates the window and renderer
bool initialize(void)
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
            return false;
        }
        // Creates window with width and height specified as global variables
        window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH,
            HEIGHT, SDL_WINDOW_SHOWN);
        //Safeguards if window does not open
        if(!window)
        {
            return false;
        }
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer)
        {
            return false;
        }
        player1 = makePlayer();
        player2 = makePlayer();
        variableBall = makeBall(ballSize);
        return true;
}
//Function renders color and images into the window(Most of game is ran here)
void update(float elapsed)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    updateBall(&variableBall, elapsed);
    renderBall(&variableBall);
    renderPaddles();
    updatePaddles(elapsed);
    SDL_RenderPresent(renderer);
}
//Function shuts down the game
void shutdown(void)
{
    if (renderer)
    {
    SDL_DestroyRenderer(renderer);
    }
    if (window)
    {
    SDL_DestroyWindow(window);
    }
    SDL_Quit();
}
bool coinFlip(void)
{
    return rand() % 2 == 1 ? true : false;
}
ball makeBall(int size)
{
    const float speed = 120;
    ball variableBall = 
    {
        .x = WIDTH / 2 - size / 2,
        .y = HEIGHT / 2 - size /2,
        .size = size,
        //Initialize speed to 120 or -120
        .xSpeed = speed * (coinFlip() ? 1 : -1),
        .ySpeed = speed * (coinFlip() ? 1 : -1),
    };
    return variableBall;
}
//Renders ball in the middle of the screen
void renderBall(const ball *variableBall)
{
    int size = variableBall->size;
    int halfSize = size /2;
    SDL_Rect rect = 
    {
        .x = variableBall->x - halfSize,
        .y = variableBall->y - halfSize,
        .w = size,
        .h = size,
    };
    SDL_SetRenderDrawColor(renderer, 255, 255, 255 ,255);
    SDL_RenderFillRect(renderer, &rect);
}
//Function creates motion to the ball and makes it bounce off of walls
void updateBall(ball *variableBall, float elapsed)
{
    if (!served)
    {
        variableBall->x = WIDTH / 2;
        variableBall ->y = HEIGHT / 2;
        return;
    }
    variableBall->x += variableBall->xSpeed * elapsed;
    variableBall->y += variableBall->ySpeed * elapsed;

    if (variableBall->x < ballSize / 2)
    {
       // variableBall->xSpeed = fabs(variableBall->xSpeed);
       updateScore(2, 100);
    }
    if (variableBall->x > WIDTH - ballSize / 2)
    {
        //variableBall->xSpeed = -fabs(variableBall ->xSpeed);
        updateScore(1, 100);
    }
    if (variableBall->y < ballSize / 2)
    {
        variableBall->ySpeed = fabs(variableBall->ySpeed);
    }
    if (variableBall->y > HEIGHT - ballSize)
    {
        variableBall->ySpeed = -fabs(variableBall->ySpeed);
    }
}
//Function creates starting points for p1 and p2 and passes it to struct variable play1 and player 2
player makePlayer(void)
{
    player playerVar = 
    {
        .yPosition = HEIGHT / 2,
    };
    return playerVar;
}
void updatePaddles(float elapsed)
{
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
    if(keyboardState[SDL_SCANCODE_SPACE])
    {
        served = true;
    }
    if (keyboardState[SDL_SCANCODE_W])
    {
        player1.yPosition -= paddleMoveSpeed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        player1.yPosition += paddleMoveSpeed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_UP])
    {
        player2.yPosition -= paddleMoveSpeed * elapsed;
    }
    if (keyboardState[SDL_SCANCODE_DOWN])
    {
        player2.yPosition += paddleMoveSpeed * elapsed;
    }
    //Creating boundary for paddles to stay on screen
    if (player1.yPosition < paddleHeight / 2)
    {
        player1.yPosition = paddleHeight /2;
    }
    if (player1.yPosition > HEIGHT - paddleHeight / 2)
    {
        player1.yPosition = HEIGHT - paddleHeight / 2;
    }
    if (player2.yPosition < paddleHeight / 2)
    {
        player2.yPosition = paddleHeight /2;
    }
    if (player2.yPosition > HEIGHT - paddleHeight / 2)
    {
        player2.yPosition = HEIGHT - paddleHeight / 2;
    }

    //Check if ball rect overlaps with either player rect
    SDL_Rect ballRect = 
    {
        .x = variableBall.x - ballSize / 2,
        .y = variableBall.y - ballSize / 2,
        .w = ballSize,
        .h = ballSize,
    };
    SDL_Rect player1Rect = 
    {
        .x = paddleGap,
        .y = (int)(player1.yPosition) - paddleHeight / 2,
        .w = paddleWidth,
        .h = paddleHeight,
    };
    if (SDL_HasIntersection(&ballRect, &player1Rect))
    {
        variableBall.xSpeed = fabs(variableBall.xSpeed); //Make ball go right
    }
    SDL_Rect player2Rect = 
    {
        .x = WIDTH - paddleWidth - paddleGap,
        .y = (int)(player2.yPosition) - paddleHeight / 2,
        .w = paddleWidth,
        .h = paddleHeight,
    };
    if (SDL_HasIntersection(&ballRect, &player2Rect))
    {
        variableBall.xSpeed = -fabs(variableBall.xSpeed); //Make ball go left
    }
}
void renderPaddles(void)
{
    //Render player 1 (left, red)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect player1Rect = 
    {
        .x = paddleGap,
        .y = (int)(player1.yPosition) - paddleHeight / 2,
        .w = paddleWidth,
        .h = paddleHeight,
    };
    SDL_RenderFillRect(renderer, &player1Rect);

     //Render player 2 (right, blue)
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect player2Rect = 
    {
        .x = WIDTH - paddleWidth - paddleGap,
        .y = (int)(player2.yPosition) - paddleHeight / 2,
        .w = paddleWidth,
        .h = paddleHeight,
    };
    SDL_RenderFillRect(renderer, &player2Rect);
}
void updateScore(int player, int points)
{
    served = false;
    if (player ==1)
    {
        player1.score += points;
    }
    if (player == 2)
    {
        player2.score += points;
    }

    char *fmt = "Player %d | Player2: %d";
    int len = snprintf(NULL, 0, fmt, player1.score, player2.score);
    char buf[len + 1];
    snprintf(buf, len + 1, fmt, player1.score, player2.score);
    SDL_SetWindowTitle(window, buf);
}