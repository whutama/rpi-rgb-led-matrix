#include "led-matrix.h"
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <signal.h>

using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;

volatile bool interrupt_received = false;
static void InterruptHandler(int signo){
    interrupt_received = true;
}
int length;
int dir; //0=right, 1=up, 2=left, 3=down

struct Coor{
    int x;
    int y;
};
struct Coor snake[1000];
struct Coor food;

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
//    erase
    int color = 25;
    for(int i=0;i<length;i++){
        if(color<100){
            color+=15;
        }
        canvas->SetPixel(snake[i].x, snake[i].y, 0, 0, color);
    }
}

void drawSnake2(Canvas *canvas){
    canvas->SetPixel(snake[999].x, snake[999].y, 0, 0, 0);
//    erase
    int color = 10;
    for(int i=0;i<6;i++){
        color+=15;
        canvas->SetPixel(snake[i].x, snake[i].y, 0, 0, color);
    }
    canvas->SetPixel(snake[length-1].x,snake[length-1].y,0,0,100);
}

void moveSnake(Canvas *canvas){
    snake[999] = snake[0];
    for (int i=0;i<length-1;i++){
        snake[i] = snake[i+1];
    }
    switch (dir){
    case 0:
        snake[length-1].x = (snake[length-1].x + 1)%64;
        break;
    case 1:
        snake[length-1].y = (snake[length-1].y - 1)%64;
        break;
    case 2:
        snake[length-1].x = (snake[length-1].x - 1)%64;
        break;
    case 3:
        snake[length-1].y = (snake[length-1].y + 1)%64;
        break;
    default:
        break;
    }
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
    if(dir==0 && snake[length-1].y==food.y && snake[length-1].x > food.x){
        return summonFood();
    }else if(dir==1 && snake[length-1].x==food.x && snake[length-1].y < food.y){
        return summonFood();
    }else if(dir==2 && snake[length-1].y==food.y && snake[length-1].x < food.x){
        return summonFood();
    }else if(dir==3 && snake[length-1].x==food.x && snake[length-1].y > food.y){
        return summonFood();
    }
    return temp;
}

void drawFood(Canvas *canvas){
    canvas->SetPixel(food.x, food.y, 0, 100, 0);
}

void growSnake(){
    length++;
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
    food = summonFood();
}

static void DrawOnCanvas(Canvas *canvas){
    initSnake();
    food = summonFood();
    drawFood(canvas);
    while(1){
        if(interrupt_received) return;
        drawSnake2(canvas);
        if((dir==0 || dir==2) && snake[length-1].x == food.x){
            if(snake[length-1].y < food.y){
                dir = 3;
            }else if(snake[length-1].y > food.y){
                dir = 1;
            }else{
                growSnake();
                drawFood(canvas);
                continue;
            }
        }
        if((dir==1 || dir==3) && snake[length-1].y == food.y){
            if(snake[length-1].x < food.x){
                dir = 0;
            }else if(snake[length-1].x > food.x){
                dir = 2;
            }else{
                growSnake();
                drawFood(canvas);
                continue;
            }
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

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    DrawOnCanvas(canvas);
    canvas->Clear();
    delete canvas;

    return 0;
}

