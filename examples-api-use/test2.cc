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

struct Snake{
    int x;
    int y;
};

void initSnake(){
    struct Snake snake[1000];
    snake[0] = {32,32};
    snake[1] = {33,32};
    snake[2] = {34,32};
    snake[3] = {35,32};
    snake[4] = {36,32};
    snake[5] = {37,32};
    length = 6;
    dir = 0;
}

void drawSnake(Canvas *canvas, Snake *snake){
//    canvas->SetPixel(snake[999].x, snake[999].y, 0, 0, 0);
//    erase
    int color = 25;
    for(int i=0;i<length;i++){
        if(color<100){
            color+=15;
        }
        canvas->SetPixel(snake[i].x, snake[i].y, 0, 0, color);
    }
}

void moveSnake(Canvas *canvas, Snake *snake){
    for (int i=0;i<length-1;i++){
        snake[i] = snake[i+1];
    }

}

static void DrawOnCanvas(Canvas *canvas){
    int center_x = canvas->width() / 2;
    int center_y = canvas->height() / 2;
    int length = canvas->width();
    for(int a=0;a<length;a++){
        if(interrupt_received) return;
        canvas->SetPixel(a, center_y, 0, 0, 255);
	    for(int b=0;b<=a;b++){
	        int c = b*10;
	        if(c>255){
		        c = 255;
	        }
            canvas->SetPixel(a-b, center_y, 0, 0, 255-c);
	    }
//	canvas->SetPixel(0, 0, 255, 0, 0);
//	canvas->SetPixel(1, 1, 0, 255, 0);
        usleep(1 * 40000);
    }
    sleep(10);
}



int main(int argc, char *argv[]){
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

