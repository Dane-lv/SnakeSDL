#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <time.h>

#include "./constants.h"  

#define MAXSEGMENT 100
#define CELL_SIZE 20
#define GRID_COLS (WINDOW_WIDTH / CELL_SIZE)
#define GRID_ROWS (WINDOW_HEIGHT / CELL_SIZE)


#define MOVE_INTERVAL 30

// Score
TTF_Font* font = NULL;
SDL_Texture* texture;
int intscore = 0;
char score[16] = "0";
SDL_Color color = {255,255,255,255};

//General
int isRunning = false;
SDL_Window* window;
SDL_Renderer* renderer;

typedef struct {
    int x;  
    int y;  
} SnakeSegment;

typedef struct {
    int x;  
    int y;  
} Apple;

SnakeSegment snake[MAXSEGMENT];
Apple apple;

int snakeLength = 40;
int dx = 1;  
int dy = 0;

int initialize_window() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    window = SDL_CreateWindow(
        "Snake",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0
    );
    if (!window) {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(!renderer) {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        fprintf(stderr, "Error initializing SDL_ttf: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }
    font = TTF_OpenFont("/Users/dane/Downloads/from-cartoon-blocks/From Cartoon Blocks.ttf", 32);
    if(!font)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }
    
    return true;
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_QUIT:
                isRunning = false;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        isRunning = false;
                        break;
                    case SDLK_UP:
                    if(dy != 1){
                        dx = 0;
                        dy = -1;
                    }
                        break;
                    case SDLK_DOWN:
                        if(dy != -1){
                            dx = 0;
                            dy = 1;
                        }
                        break;
                    case SDLK_LEFT:
                        if(dx != 1)
                        dx = -1;
                        dy = 0;
                        break;
                    case SDLK_RIGHT:
                        if(dx != -1)
                        dx = 1;
                        dy = 0;
                        break;
                }
                break;
        }
    }
}

void spawnApple() {
    apple.x = rand() % GRID_COLS;
    apple.y = rand() % GRID_ROWS;
}

void setup() {
   
    snake[0].x = GRID_COLS / 2;
    snake[0].y = GRID_ROWS / 2;

    for(int i = 1; i < snakeLength; i++) {
        snake[i].x = snake[0].x - i; // to the left
        snake[i].y = snake[0].y;
    }

    spawnApple();
}

void resetSnake()
{
    dx = 1;
    dy = 0;
    snakeLength = 40;
    snake[0].x = GRID_COLS / 2;
    snake[0].y = GRID_ROWS / 2;
    intscore = 0;
    sprintf(score, "%d", intscore);


    // Place subsequent segments behind the head
    for(int i = 1; i < snakeLength; i++) {
        snake[i].x = snake[0].x - i; // to the left
        snake[i].y = snake[0].y;
    }

    spawnApple();
}

// Moves the snake exactly 1 cell in the direction (dx, dy)
void update() {
    // Save the head's old position
    int prevX = snake[0].x;
    int prevY = snake[0].y;

    // Move head by 1 cell
    snake[0].x += dx;
    snake[0].y += dy;

    // Check for boundary collision
    if (snake[0].x < 0 || snake[0].x >= GRID_COLS ||
        snake[0].y < 0 || snake[0].y >= GRID_ROWS) {
        resetSnake();
        return;
    }

    // Move the body
    for(int i = 1; i < snakeLength; i++) {
        int tempX = snake[i].x;
        int tempY = snake[i].y;
        snake[i].x = prevX;
        snake[i].y = prevY;
        prevX = tempX;
        prevY = tempY;
    }

    for(int i = 1; i < snakeLength; i++)
    {
        if(snake[0].x == snake[i].x && snake[0].y == snake[i].y){
            resetSnake();
            return; 
        }
    }

    if (snake[0].x == apple.x && snake[0].y == apple.y) {
        
        if (snakeLength < MAXSEGMENT) {
            snake[snakeLength].x = snake[snakeLength - 1].x; //new segments position, 
            // global arrays arrays are initialized to 0,0. without it pixel appears at 0,0 after each time you consume an apple
            snake[snakeLength].y = snake[snakeLength - 1].y;
            snakeLength++;
        }
        intscore++;
        sprintf(score, "%d", intscore);
        spawnApple();
    }
}

void render() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Surface* surface = TTF_RenderText_Solid(font, score, color);
    if (!surface) {
        printf("Error: %s", SDL_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if(!texture){
        fprintf(stderr, "SDL_CreateTextureFromSurface error: %s", SDL_GetError());
        return;
    }


    SDL_Rect textRect;
    textRect.x = 350;
    textRect.y = 30;
    textRect.w = 60;
    textRect.h = 60;

    SDL_RenderCopy(renderer,texture,NULL, &textRect);

    SDL_Rect rect;
    // Draw snake
    for (int i = 0; i < snakeLength; i++) {
        rect.x = snake[i].x * CELL_SIZE;
        rect.y = snake[i].y * CELL_SIZE;
        rect.w = CELL_SIZE;
        rect.h = CELL_SIZE;

        if (i == 0) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // head in green
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // body in white
        }
        SDL_RenderFillRect(renderer, &rect);
    }

    // Draw apple
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    rect.x = apple.x * CELL_SIZE;
    rect.y = apple.y * CELL_SIZE;
    rect.w = CELL_SIZE;
    rect.h = CELL_SIZE;
    SDL_RenderFillRect(renderer, &rect);

    // Show everything
    SDL_RenderPresent(renderer);
}

int main() {
    if(!initialize_window()) {
        return 1;
    }

    setup();

    isRunning = true;

    Uint32 previousMoveTime = SDL_GetTicks();  // last time we moved the snake
    const Uint32 moveInterval = MOVE_INTERVAL; // ms between moves

    while(isRunning) {
        process_input();

        // Check if it's time to move the snake
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - previousMoveTime >= moveInterval) {
            update();
            previousMoveTime = currentTime;
        }

        
        render();
        SDL_Delay(16);  // ~60 FPS for drawing
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
    SDL_Quit();

    return 0;
}
