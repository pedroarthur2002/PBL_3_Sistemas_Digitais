#include "png_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>  // opcional se usar sqrt

#define WIDTH 320
#define HEIGHT 240

// Converte RGB para grayscale
void rgb_to_grayscale(unsigned char rgb[HEIGHT][WIDTH][3], unsigned char gray[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char r = rgb[y][x][0];
            unsigned char g = rgb[y][x][1];
            unsigned char b = rgb[y][x][2];
            gray[y][x] = (unsigned char)(0.299*r + 0.587*g + 0.114*b);
        }
    }
}

// Convolução Sobel
int sobel_convolution(unsigned char window[3][3], int filter[3][3]) {
    int sum = 0;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            sum += window[i][j] * filter[i][j];
        }
    }
    return sum;
}

// Extrai janela 3x3 ao redor do pixel (x,y)
void extract_window(unsigned char img[HEIGHT][WIDTH], int x, int y, unsigned char window[3][3]) {
    for (int i=-1; i<=1; i++) {
        for (int j=-1; j<=1; j++) {
            int px = x + j;
            int py = y + i;
            if (px < 0 || px >= WIDTH || py < 0 || py >= HEIGHT) {
                window[i+1][j+1] = 0; // borda = 0
            } else {
                window[i+1][j+1] = img[py][px];
            }
        }
    }
}

int main() {
    const char* input = "imagem.png";
    const char* output = "sobel_output.png";

    unsigned char rgb[HEIGHT][WIDTH][3];
    unsigned char grayscale[HEIGHT][WIDTH];
    unsigned char sobel_result[HEIGHT][WIDTH] = {0};

    int gx_buffer[HEIGHT][WIDTH] = {0};
    int gy_buffer[HEIGHT][WIDTH] = {0};

    int sobel_gx[3][3] = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int sobel_gy[3][3] = {
        { 1,  2,  1},
        { 0,  0,  0},
        {-1, -2, -1}
    };

    if (read_png(input, rgb)) {
        fprintf(stderr, "Erro lendo a imagem\n");
        return 1;
    }

    rgb_to_grayscale(rgb, grayscale);

    // Fase 1: calcula gx
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char window[3][3];
            extract_window(grayscale, x, y, window);
            gx_buffer[y][x] = sobel_convolution(window, sobel_gx);
        }
    }

    // Fase 2: calcula gy
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char window[3][3];
            extract_window(grayscale, x, y, window);
            gy_buffer[y][x] = sobel_convolution(window, sobel_gy);
        }
    }

    // Fase 3: calcula G = |gx| + |gy|
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int gx = gx_buffer[y][x];
            int gy = gy_buffer[y][x];
            int g = abs(gx) + abs(gy); // aproximação da magnitude
            if (g > 255) g = 255;
            sobel_result[y][x] = (unsigned char)g;
        }
    }

    save_grayscale_png(output, sobel_result);
    printf("Imagem Sobel salva em %s\n", output);
    return 0;
}

