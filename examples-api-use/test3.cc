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

int locations[64][64];

void initSnake(){
    snake[0] = {32,32};
    snake[1] = {33,32};
    snake[2] = {34,32};
    snake[3] = {35,32};
    snake[4] = {36,32};
    snake[5] = {37,32};
    length = 6;
    dir = 0;
    for(int i=0;i<64;i++){
        for(int j=0,j<64;j++){
            locations[i][j] = 0;
        }
    }
    locations[32][32] = 1; //snake tail
    locations[33][32] = 1;
    locations[34][32] = 1;
    locations[35][32] = 1;
    locations[36][32] = 1;
    locations[37][32] = 1; //snake head
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
    locations[snake[999].x][snake[999.y]] = 0;
    locations[snake[length-1].x][snake[length-1.y]] = 1;
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
    /*if(dir==0 && snake[length-1].y==food.y && snake[length-1].x > food.x){
        return summonFood();
    }else if(dir==1 && snake[length-1].x==food.x && snake[length-1].y < food.y){
        return summonFood();
    }else if(dir==2 && snake[length-1].y==food.y && snake[length-1].x < food.x){
        return summonFood();
    }else if(dir==3 && snake[length-1].x==food.x && snake[length-1].y > food.y){
        return summonFood();
    }*/
    printf("spawn food at (%d, %d)\n",temp.x, temp.y);
    locations[temp.x][temp.y] = 2;
    return temp;
}

void drawFood(Canvas *canvas){
    canvas->SetPixel(food.x, food.y, 0, 100, 0);
}

void checkCollision(Canvas *canvas){
    int next_x=0;
    int next_y=0;
    switch (dir){
    case 0:
        next_x=1;
        break;
    case 1:
        next_y=-1;
        break;
    case 2:
        next_x=-1;
        break;
    case 3:
        next_y=1;
        break;
    default:
        break;
    }
    if(locations[snake[length-1].x + next_x][snake[length-1].y + next_y] == 2){
        growSnake();
    }else if(locations[snake[length-1].x + next_x][snake[length-1].y + next_y] == 1){
        dir = (dir + 3) % 4
        if(locations[snake[length-1].x + next_x][snake[length-1].y + next_y] == 1){}
        moveSnake(canvas);
    }else{
        moveSnake(canvas);
    }
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
    initSnake();
    food = summonFood();
    drawFood(canvas);
    printf("start\n");
    while(1){
        if(interrupt_received) return;
        drawSnake(canvas);
        if((dir==0 || dir==2) && (snake[length-1].x == food.x)){
            if(snake[length-1].y < food.y){
                printf("go down\n");
                dir = 3;
            }else if(snake[length-1].y > food.y){
                printf("go up\n");
                dir = 1;
            }else{
                printf("spawn food\n");
                if(length<999){
                    growSnake();
                }
                food = summonFood();
                drawFood(canvas);
                continue;
            }
        }
        if((dir==1 || dir==3) && (snake[length-1].y == food.y)){
            if(snake[length-1].x < food.x){
                printf("go right\n");
                dir = 0;
            }else if(snake[length-1].x > food.x){
                printf("go left\n");
                dir = 2;
            }else{
                printf("spawn food\n");
                if(length<999){
                    growSnake();
                }
                food = summonFood();
                drawFood(canvas);
                continue;
            }
        }
        moveSnake(canvas);

//	canvas->SetPixel(0, 0, 255, 0, 0);
//	canvas->SetPixel(1, 1, 0, 255, 0);
        usleep(10 * 40000);
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

