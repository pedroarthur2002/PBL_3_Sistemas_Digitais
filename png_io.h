#ifndef PNG_IO_H
#define PNG_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

#define WIDTH 320
#define HEIGHT 240

// Função para ler imagem PNG
int read_png(const char* filename, unsigned char image[HEIGHT][WIDTH][3]) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Erro: Não foi possível abrir o arquivo %s\n", filename);
        return 1;
    }

    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "Erro: O arquivo %s não é um PNG válido\n", filename);
        fclose(fp);
        return 1;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "Erro: A imagem deve ser %dx%d pixels\n", WIDTH, HEIGHT);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    png_bytep row_pointers[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
        row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);

    for (int y = 0; y < HEIGHT; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < WIDTH; x++) {
            png_bytep px = &(row[x * 3]);
            image[y][x][0] = px[0];
            image[y][x][1] = px[1];
            image[y][x][2] = px[2];
        }
        free(row_pointers[y]);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return 0;
}

// Função para salvar imagem em escala de cinza como PNG
void save_grayscale_png(const char* filename, unsigned char grayscale[HEIGHT][WIDTH]) {
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Erro ao criar arquivo %s\n", filename);
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_set_IHDR(png, info, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_GRAY,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    png_bytep row_pointers[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
        row_pointers[y] = (png_bytep)&grayscale[y][0];
    }
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

#endif
