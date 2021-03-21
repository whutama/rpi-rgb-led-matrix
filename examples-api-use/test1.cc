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

static void DrawOnCanvas(Canvas *canvas){
    int center_x = canvas->width() / 2;
    int center_y = canvas->height() / 2;
    int length = center_x;
    for(int a=0;a<length;a++){
        if(interrupt_received) return;
        canvas->SetPixel((center_x/2)+a, center_y, 0, 0, 255);
        usleep(1 * 20000);
    }
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

