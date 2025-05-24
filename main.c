#include "png_io.h"
#include <stdio.h>

void rgb_to_grayscale(unsigned char rgb_image[HEIGHT][WIDTH][3], 
                     unsigned char grayscale_image[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char r = rgb_image[y][x][0];
            unsigned char g = rgb_image[y][x][1];
            unsigned char b = rgb_image[y][x][2];
            grayscale_image[y][x] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
        }
    }
}

int main() {
    const char* input = "imagem.png";
    const char* output = "imagem_grayscale.png";
    
    unsigned char rgb[HEIGHT][WIDTH][3];
    unsigned char grayscale[HEIGHT][WIDTH];

    if (read_png(input, rgb)) return 1;
    rgb_to_grayscale(rgb, grayscale);
    save_grayscale_png(output, grayscale);

    return 0;
}
