#include "led-matrix.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>
#include <termios.h>

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

int length;
int dir; //0=right, 1=up, 2=left, 3=down

struct Coor{
    int x;
    int y;
};
struct Coor snake[1000];
struct Coor food;

struct termios orig_termios;
void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(disableRawMode);
  struct termios raw = orig_termios;
  raw.c_lflag &= ~(ECHO | ICANON);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void initSnake(){
    snake[0] = {32,32};
    snake[1] = {33,32};
    snake[2] = {34,32};
    snake[3] = {35,32};
    snake[4] = {36,32};
    snake[5] = {37,32};
    length = 6;
    dir = 0;
}

void drawSnake(Canvas *canvas){
    canvas->SetPixel(snake[999].x, snake[999].y, 0, 0, 0);
    //erase last snake tail
    int color = 10;
    for(int i=0;i<6;i++){
        color+=15;
        canvas->SetPixel(snake[i].x, snake[i].y, 0, 0, color);
    }
    //draw last 6 snake tail
    canvas->SetPixel(snake[length-2].x, snake[length-2].y, 0, 0, color);
    canvas->SetPixel(snake[length-1].x, snake[length-1].y, color, color, color);
    //draw snake head
}

void moveSnake(Canvas *canvas){
    snake[999] = snake[0];
    //snake tail to be erased
    for (int i=0;i<length-1;i++){
        snake[i] = snake[i+1];
    }
    //snake body
    switch (dir){
    case 0:
        snake[length-1].x = (snake[length-1].x + 1)%64;
        break;
    case 1:
        snake[length-1].y = (snake[length-1].y + 63)%64;
        break;
    case 2:
        snake[length-1].x = (snake[length-1].x + 63)%64;
        break;
    case 3:
        snake[length-1].y = (snake[length-1].y + 1)%64;
        break;
    default:
        break;
    }
    //snake head
}

Coor summonFood(){
    Coor temp;
    temp.x = rand()%64;
    temp.y = rand()%64;
    for(int i=0;i<length;i++){
        if(snake[i].x==temp.x && snake[i].y==temp.y){
            return summonFood();
        }
    }
    printf("spawn food at (%d, %d)\n",temp.x, temp.y);
    return temp;
}

void drawFood(Canvas *canvas){
    canvas->SetPixel(food.x, food.y, 0, 100, 0);
}

void growSnake(){
    length++;
    snake[length-1] = snake[length-2];
    switch (dir){
    case 0:
        snake[length-1].x += 1;
        break;
    case 1:
        snake[length-1].y -= 1;
        break;
    case 2:
        snake[length-1].x -= 1;
        break;
    case 3:
        snake[length-1].y += 1;
        break;
    default:
        break;
    }
}

static void DrawOnCanvas(Canvas *canvas){
    char inp = 'd';
    initSnake();
    food = summonFood();
    drawFood(canvas);
    printf("start\n");
    while(1){
        if(inp == 'q') return;
        drawSnake(canvas);
        read(STDIN_FILENO, &inp, 1);
        switch (inp){
        case 'w':
            dir = 1;
            break;
        case 'a':
            dir = 2;
            break;
        case 's':
            dir = 3;
            break;
        case 'd':
            dir = 0;
            break;
        default:
            break;
        }
        if(snake[length-1].x == food.x && snake[length-1].y == food.y){
            growSnake();
            food = summonFood();
            drawFood(canvas);
        }
        moveSnake(canvas);

//	canvas->SetPixel(0, 0, 255, 0, 0);
//	canvas->SetPixel(1, 1, 0, 255, 0);
        usleep(1 * 40000);
    }
    sleep(10);
}



int main(int argc, char *argv[]){
    srand(time(NULL));
    RGBMatrix::Options defaults;
    defaults.hardware_mapping = "regular";
    defaults.rows = 64;
    defaults.cols = 64;
    defaults.chain_length = 1;
    defaults.parallel = 1;
    defaults.show_refresh_rate = false;
    Canvas *canvas = RGBMatrix::CreateFromFlags(&argc, &argv, &defaults);
    if (canvas == NULL) return 1;

    enableRawMode();
    DrawOnCanvas(canvas);
    canvas->Clear();
    delete canvas;

    return 0;
}
