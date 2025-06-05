#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include <math.h>
#include <string.h>

#define MATRIX_SIZE 25
#define WIDTH 320
#define HEIGHT 240

// Aumentar precisão - usar uint8_t para janelas e int16_t para resultados
typedef uint8_t pixel_t;
typedef int16_t result_t;
// Variável global para armazenar a imagem em escala de cinza
unsigned char grayscale[HEIGHT][WIDTH];
static int8_t kernel_zero[MATRIX_SIZE] = {0};
// Variável global para a janela - sempre 5x5 (25 elementos)
pixel_t window[MATRIX_SIZE];

// Função para redimensionar e carregar imagem usando STB Image
int resize_and_load_image(const char* filename, unsigned char rgb[HEIGHT][WIDTH][3]) {
    int img_width, img_height, channels, y, x;
    unsigned char* input_data = stbi_load(filename, &img_width, &img_height, &channels, 3);
    
    if (!input_data) {
        printf("Erro ao carregar a imagem: %s\n", filename);
        return -1;
    }
    
    printf("Imagem carregada: %dx%d pixels, %d canais\n", img_width, img_height, channels);
    
    // Se a imagem já tem o tamanho correto, copia diretamente
    if (img_width == WIDTH && img_height == HEIGHT) {
        for (y = 0; y < HEIGHT; y++) {
            for (x = 0; x < WIDTH; x++) {
                int idx = (y * WIDTH + x) * 3;
                rgb[y][x][0] = input_data[idx + 0];
                rgb[y][x][1] = input_data[idx + 1];
                rgb[y][x][2] = input_data[idx + 2];
            }
        }
    } else {
        // Redimensionamento simples usando nearest neighbor
        printf("Redimensionando de %dx%d para %dx%d\n", img_width, img_height, WIDTH, HEIGHT);
        
        for (y = 0; y < HEIGHT; y++) {
            for (x = 0; x < WIDTH; x++) {
                int src_x = (x * img_width) / WIDTH;
                int src_y = (y * img_height) / HEIGHT;
                
                // Garante que não ultrapasse os limites
                if (src_x >= img_width) src_x = img_width - 1;
                if (src_y >= img_height) src_y = img_height - 1;
                
                int src_idx = (src_y * img_width + src_x) * 3;
                rgb[y][x][0] = input_data[src_idx + 0];
                rgb[y][x][1] = input_data[src_idx + 1];
                rgb[y][x][2] = input_data[src_idx + 2];
            }
        }
    }
    
    stbi_image_free(input_data);
    return 0;
}

// Função para salvar imagem em escala de cinza como PNG
void save_grayscale_png(const char* filename, unsigned char gray[HEIGHT][WIDTH]) {
    if (!stbi_write_png(filename, WIDTH, HEIGHT, 1, gray, WIDTH)) {
        printf("Erro ao salvar PNG: %s\n", filename);
    } else {
        printf("PNG salvo: %s\n", filename);
    }
}

// Converte RGB para grayscale
void rgb_to_grayscale(unsigned char rgb[HEIGHT][WIDTH][3], unsigned char gray[HEIGHT][WIDTH]) {
    int y, x;
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            unsigned char r = rgb[y][x][0];
            unsigned char g = rgb[y][x][1];
            unsigned char b = rgb[y][x][2];
            gray[y][x] = (unsigned char)(0.299*r + 0.587*g + 0.114*b);
        }
    }
}

// Função de extração de janela corrigida
void extract_window_linear(unsigned char img[HEIGHT][WIDTH], int x, int y, uint32_t size_code) {
    int i, j, idx;
    int px, py;
    
    // Zera toda a janela 5x5
    memset(window, 0, MATRIX_SIZE * sizeof(pixel_t));
    
    if (size_code == 0) {
        // Caso especial para Roberts 2x2
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                px = x + j;
                py = y + i;
                
                // Posiciona a janela 2x2 no centro da matriz 5x5
                int row_in_5x5 = i; 
                int col_in_5x5 = j; 
                idx = row_in_5x5 * 5 + col_in_5x5;
                
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    window[idx] = (pixel_t)img[py][px];
                } else {
                    window[idx] = 0; // padding para bordas
                }
            }
        }
    } else {
        // Caso geral para janelas 3x3 e 5x5
        int window_size;
        switch(size_code) {
            case 1: window_size = 3; break; // Sobel/Prewitt 3x3
            case 3: window_size = 5; break; // Sobel 5x5/Laplaciano 5x5
            default: window_size = 3; break;
        }
        
        int half_size = window_size / 2;
        
        // Extrai janela centralizada
        for (i = -half_size; i <= half_size; i++) {
            for (j = -half_size; j <= half_size; j++) {
                px = x + j;
                py = y + i;
                
                // Calcula o índice na matriz 5x5
                int row_in_5x5 = i + 2; 
                int col_in_5x5 = j + 2; 
                idx = row_in_5x5 * 5 + col_in_5x5;
                
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    window[idx] = (pixel_t)img[py][px];
                } else {
                    window[idx] = 0; // padding para bordas
                }
            }
        }
    }
}

// Calcula a convolução usando a FPGA
int compute_convolution(pixel_t* image_window, int8_t* filter_kernel_gx, int8_t* filter_kernel_gy, uint32_t size_code) {
    pixel_t result[MATRIX_SIZE];
    int i;
   
    if (size_code == 5){
        struct Params params = {
            .a = image_window,
            .b = filter_kernel_gx,
            .opcode = 6,
            .size = size_code,
            .c = filter_kernel_gy
        };
    
        if (send_all_data(&params) != HW_SUCCESS) {
            fprintf(stderr, "Falha no envio de dados para a FPGA\n");
            return 0;
        }
        if (read_all_results(result) != HW_SUCCESS) {
            fprintf(stderr, "Falha na leitura dos resultados da FPGA\n");
            return 0;
        }
    } else{
        struct Params params = {
            .a = image_window,
            .b = filter_kernel_gx,
            .opcode = 7,
            .size = size_code,
            .c = filter_kernel_gy
        };
    
        if (send_all_data(&params) != HW_SUCCESS) {
            fprintf(stderr, "Falha no envio de dados para a FPGA\n");
            return 0;
        }
        if (read_all_results(result) != HW_SUCCESS) {
            fprintf(stderr, "Falha na leitura dos resultados da FPGA\n");
            return 0;
        }
    }
    
    return result[0]; 
}

// Calcula a imagem com o filtro de borda selecionado
void operation_filter(int8_t* filter_gx, int8_t* filter_gy, uint32_t size_code, unsigned char result[HEIGHT][WIDTH]) {
    int x, y;
    
    for (y = 0; y < HEIGHT; y++) {
        if (y % 40 == 0) printf("Processando linha %d/%d\n", y, HEIGHT);
        
        for (x = 0; x < WIDTH; x++) {
            extract_window_linear(grayscale, x, y, size_code);
            result[y][x] = compute_convolution(window, filter_gx, filter_gy, size_code);
        }
    
    }
    
    printf("Filtro de gradiente aplicado com sucesso!\n");
}

int validate_operation(uint32_t selection) {
    if (selection < 1 || selection > 6) {
        fprintf(stderr, "Opção inválida: %u\n", selection);
        return HW_SEND_FAIL;
    }
    return HW_SUCCESS;
}

int main() {
    char output[100];
    char jpg_output[100];
    unsigned char rgb[HEIGHT][WIDTH][3];
    unsigned char filter_result[HEIGHT][WIDTH];
    uint32_t selection;
    int y, x;
    
    // Inicializa buffer de resultado
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            filter_result[y][x] = 0;
        }
    }
    
    // Carrega apenas a imagem.png
    printf("Carregando imagem.png...\n");
    if (resize_and_load_image("imagem.png", rgb) != 0) {
        fprintf(stderr, "Não foi possível carregar imagem.png!\n");
        return EXIT_FAILURE;
    }
    
    printf("Convertendo para escala de cinza...\n");
    rgb_to_grayscale(rgb, grayscale);
    save_grayscale_png("imagem_cinza.png", grayscale);
    
    // Inicializa o hardware
    printf("Inicializando hardware...\n");
    if (init_hw_access() != HW_SUCCESS) { 
        fprintf(stderr, "Falha na inicialização do hardware\n");
        return EXIT_FAILURE;
    }
    
    while (1) {
        selection = 0;
        
        printf("\n\n========= FILTRO DE BORDA =========\n");
        printf("Escolha o método de filtro de borda:\n");
        printf("1 - Sobel (3x3)\n");
        printf("2 - Sobel expandido (5x5)\n");
        printf("3 - Prewitt (3x3)\n");
        printf("4 - Roberts (2x2)\n");
        printf("5 - Laplaciano (5x5)\n");
        printf("6 - Sair\n");
        printf("Opção: ");
        
        if (scanf("%u", &selection) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n'); // Limpa o buffer
            continue;
        }
        
        if (selection == 6) {
            printf("Encerrando programa...\n");
            break;
        }
        
        if (validate_operation(selection) != HW_SUCCESS) {
            continue;
        }
        
        // Aplica o filtro selecionado
        switch (selection) {
            case 1:
                printf("\nAplicando filtro Sobel 3x3...\n");
                operation_filter(sobel_gx_3x3, sobel_gy_3x3, 1, filter_result);
                sprintf(output, "sobel_3x3_output.png");
                break;
                
            case 2:
                printf("\nAplicando filtro Sobel 5x5...\n");
                operation_filter(sobel_gx_5x5, sobel_gy_5x5, 3, filter_result);
                sprintf(output, "sobel_5x5_output.png");
                break;
                
            case 3:
                printf("\nAplicando filtro Prewitt 3x3...\n");
                operation_filter(prewitt_gx_3x3, prewitt_gy_3x3, 1, filter_result);
                sprintf(output, "prewitt_3x3_output.png");
                break;
                
            case 4:
                printf("\nAplicando filtro Roberts 2x2...\n");
                operation_filter(roberts_gx_2x2, roberts_gy_2x2, 0, filter_result);
                sprintf(output, "roberts_2x2_output.png");
                break;
                
            case 5:
                printf("\nAplicando filtro Laplaciano 5x5...\n");
                operation_filter(laplaciano_5x5, kernel_zero, selection, filter_result);
                sprintf(output, "laplaciano_5x5_output.png");
                break;
                
            default:
                printf("Opção inválida!\n");
                continue;
        }
        
        // Salva a imagem resultante em múltiplos formatos
        printf("Salvando resultado...\n");
        save_grayscale_png(output, filter_result);
        
        printf("Imagem salva como '%s'\n", output);
    }
    
    // Limpa os recursos
    printf("Liberando recursos do hardware...\n");
    close_hw_access();
    
    return EXIT_SUCCESS;
}