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

    // Verifica o cabeçalho PNG
    png_byte header[8];
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "Erro: O arquivo %s não é um PNG válido\n", filename);
        fclose(fp);
        return 1;
    }

    // Inicializa estruturas libpng
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

    // Configura tratamento de erros
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    // Verifica dimensões
    png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
    png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
    
    if (width != WIDTH || height != HEIGHT) {
        fprintf(stderr, "Erro: A imagem deve ser %dx%d pixels (tamanho atual: %dx%d)\n", 
                WIDTH, HEIGHT, width, height);
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        return 1;
    }

    // Configurações de leitura
    png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    // Aloca memória para as linhas da imagem
    png_bytep row_pointers[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
        row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
    }

    // Lê a imagem
    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, NULL);

    // Converte para nosso formato de imagem (RGB)
    for (int y = 0; y < HEIGHT; y++) {
        png_bytep row = row_pointers[y];
        for (int x = 0; x < WIDTH; x++) {
            png_bytep px = &(row[x * 3]);
            image[y][x][0] = px[0]; // R
            image[y][x][1] = px[1]; // G
            image[y][x][2] = px[2]; // B
        }
        free(row_pointers[y]);
    }

    // Limpeza
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return 0;
}

// Função para converter RGB para escala de cinza
void rgb_to_grayscale(unsigned char rgb_image[HEIGHT][WIDTH][3], 
                     unsigned char grayscale_image[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned char r = rgb_image[y][x][0];
            unsigned char g = rgb_image[y][x][1];
            unsigned char b = rgb_image[y][x][2];
            
            // Fórmula de luminância (recomendada)
            grayscale_image[y][x] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
        }
    }
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

    // Configuração do PNG (8-bit grayscale)
    png_set_IHDR(png, info, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_GRAY,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);

    // Prepara e escreve os dados
    png_bytep row_pointers[HEIGHT];
    for (int y = 0; y < HEIGHT; y++) {
        row_pointers[y] = (png_bytep)&grayscale[y][0];
    }
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    // Limpeza
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

int main() {
    const char* input_filename = "imagem.png";
    const char* output_filename = "imagem_grayscale.png";
    
    unsigned char rgb_image[HEIGHT][WIDTH][3];  // Imagem RGB original
    unsigned char grayscale_image[HEIGHT][WIDTH]; // Imagem em escala de cinza

    // 1. Ler a imagem PNG
    if (read_png(input_filename, rgb_image)) {
        fprintf(stderr, "Falha ao ler a imagem de entrada\n");
        return 1;
    }
    printf("Imagem %s lida com sucesso!\n", input_filename);

    // 2. Converter para escala de cinza
    rgb_to_grayscale(rgb_image, grayscale_image);
    printf("Conversão para escala de cinza concluída.\n");

    // 3. Salvar a imagem em escala de cinza
    save_grayscale_png(output_filename, grayscale_image);
    printf("Imagem em escala de cinza salva como %s\n", output_filename);

    // 4. Exemplo de acesso aos pixels
    printf("Pixel (0,0) - Original: R=%d, G=%d, B=%d\n", 
           rgb_image[0][0][0], rgb_image[0][0][1], rgb_image[0][0][2]);
    printf("Pixel (0,0) - Escala de Cinza: %d\n", grayscale_image[0][0]);

    return 0;
}
