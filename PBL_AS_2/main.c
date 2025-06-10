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

// Função de extração de janela corrigida
void extract_window_linear(unsigned char img[HEIGHT][WIDTH], int x, int y, uint32_t size_code) {
    int i, j, idx;
    int px, py;

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

// Função simples de saturação - clamp para faixa 0-255
unsigned char saturate_pixel(result_t value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return (unsigned char)value;
}

// Função de convolução em C (versão de referência)
int compute_convolution_cpu(pixel_t* image_window, int8_t* filter_kernel_gx, int8_t* filter_kernel_gy, int8_t laplaciano) {
    result_t gx = 0, gy = 0;
    int i;
    
    if (laplaciano == 1) {
        // Para Laplaciano, usa apenas o primeiro kernel
        for (i = 0; i < MATRIX_SIZE; i++) {
            gx += (result_t)image_window[i] * (result_t)filter_kernel_gx[i];
        }
        return saturate_pixel(gx);
    } else {
        // Para filtros de gradiente (Sobel, Prewitt, Roberts)
        for (i = 0; i < MATRIX_SIZE; i++) {
            gx += (result_t)image_window[i] * (result_t)filter_kernel_gx[i];
            gy += (result_t)image_window[i] * (result_t)filter_kernel_gy[i];
        }
        
        // Calcula a magnitude do gradiente
        double magnitude = sqrt((double)(gx * gx + gy * gy));
        return saturate_pixel((result_t)magnitude);
    }
}

// Função para aplicar filtro usando processamento em C
void operation_filter_cpu(int8_t* filter_gx, int8_t* filter_gy, uint32_t size_code, unsigned char result[HEIGHT][WIDTH], int8_t laplaciano) {
    int x, y;
    
    for (y = 0; y < HEIGHT; y++) {        
        for (x = 0; x < WIDTH; x++) {
            extract_window_linear(grayscale, x, y, size_code);
            result[y][x] = compute_convolution_cpu(window, filter_gx, filter_gy, laplaciano);
        }
    }
    
    printf("Filtro aplicado com sucesso (CPU)!\n");
}

int compute_convolution(pixel_t* image_window, int8_t* filter_kernel_gx, int8_t* filter_kernel_gy, int8_t laplaciano) {
    pixel_t result[MATRIX_SIZE];
    pixel_t result_final = 0;
    
    if (laplaciano == 1) {
        struct Params params = {
            .a = image_window,
            .b = filter_kernel_gx,
            .opcode = 6,
            .size = 3,
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
        
        // Combina os bytes do resultado
        result_t result_temp = (int16_t)((result[1] << 8) | result[0]);
        result_t abs_result = abs(result_temp);
        
        return saturate_pixel(abs_result);

    } else {
        struct Params params = {
            .a = image_window,
            .b = filter_kernel_gx,
            .opcode = 7,
            .size = 3,
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
        result_final = result[0];
        return result_final;
    }
}

// Calcula a imagem com o filtro de borda selecionado
void operation_filter(int8_t* filter_gx, int8_t* filter_gy, uint32_t size_code, unsigned char result[HEIGHT][WIDTH], int8_t laplaciano) {
    int x, y;
    
    for (y = 0; y < HEIGHT; y++) {
        if (y % 40 == 0) printf("Processando linha %d/%d\n", y, HEIGHT);
        
        for (x = 0; x < WIDTH; x++) {
            extract_window_linear(grayscale, x, y, size_code);
            result[y][x] = compute_convolution(window, filter_gx, filter_gy, laplaciano);
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

// Função simples para calcular porcentagem de diferença
typedef struct {
    double average_percentage_difference;  // Diferença percentual média
    double max_percentage_difference;      // Maior diferença percentual encontrada
    double min_percentage_difference;      // Menor diferença percentual encontrada
    int pixels_with_zero_reference;        // Pixels onde referência = 0 (divisão por zero)
    int total_valid_pixels;               // Total de pixels válidos para comparação
} PercentageDifference;

PercentageDifference calculate_percentage_difference(unsigned char reference[HEIGHT][WIDTH], unsigned char generated[HEIGHT][WIDTH]) {
    PercentageDifference result = {0};
    double sum_percentage = 0.0;
    int x, y;
    int valid_pixels = 0;
    
    // Inicializa min/max
    result.max_percentage_difference = 0.0;
    result.min_percentage_difference = 100.0;
    
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            unsigned char ref_pixel = reference[y][x];
            unsigned char gen_pixel = generated[y][x];
            
            if (ref_pixel == 0) {
                // Pixel de referência é zero - não podemos dividir
                result.pixels_with_zero_reference++;
                continue;
            }
            
            // Calcula a diferença percentual: |referência - gerada| / referência * 100
            double percentage_diff = (double)abs((int)ref_pixel - (int)gen_pixel) / (double)ref_pixel * 100.0;
            
            sum_percentage += percentage_diff;
            valid_pixels++;
            
            // Atualiza min/max
            if (percentage_diff > result.max_percentage_difference) {
                result.max_percentage_difference = percentage_diff;
            }
            if (percentage_diff < result.min_percentage_difference) {
                result.min_percentage_difference = percentage_diff;
            }
        }
    }
    
    result.total_valid_pixels = valid_pixels;
    
    if (valid_pixels > 0) {
        result.average_percentage_difference = sum_percentage / valid_pixels;
    } else {
        result.average_percentage_difference = 0.0;
    }
    
    return result;
}

// Função para imprimir o relatório simplificado
void print_percentage_report(PercentageDifference diff, const char* filter_name) {
    printf("\n========= DIFERENÇA PERCENTUAL - %s =========\n", filter_name);
    printf("Total de pixels: %d\n", HEIGHT * WIDTH);
    printf("Pixels válidos para comparação: %d\n", diff.total_valid_pixels);
    printf("Pixels com referência = 0: %d\n", diff.pixels_with_zero_reference);
    printf("\nDIFERENÇA PERCENTUAL:\n");
    printf("  Média: %.2f%%\n", diff.average_percentage_difference);
    printf("  Máxima: %.2f%%\n", diff.max_percentage_difference);
    printf("  Mínima: %.2f%%\n", diff.min_percentage_difference);
    
    // Interpretação
    if (diff.average_percentage_difference < 1.0) {
        printf("✓ RESULTADO: Excelente precisão (< 1%% diferença)\n");
    } else if (diff.average_percentage_difference < 5.0) {
        printf("✓ RESULTADO: Boa precisão (< 5%% diferença)\n");
    } else if (diff.average_percentage_difference < 10.0) {
        printf("⚠ RESULTADO: Precisão aceitável (< 10%% diferença)\n");
    } else {
        printf("✗ RESULTADO: Baixa precisão (≥ 10%% diferença)\n");
    }
    printf("==========================================\n");
}


int main() {
    char output[100];
    char jpg_output[100];
    unsigned char rgb[HEIGHT][WIDTH][3];
    unsigned char filter_result[HEIGHT][WIDTH];
    uint32_t selection;
    int y, x;

    unsigned char filter_result_cpu[HEIGHT][WIDTH];  // Resultado do processamento em C (referência)
    PercentageDifference percentage_diff;
    char cpu_output[100];
    char diff_output[100];
    
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
                
                // Processa com CPU (referência)
                printf("Processando com CPU (referência)...\n");
                operation_filter_cpu(sobel_gx_3x3, sobel_gy_3x3, 1, filter_result_cpu, 0);
                sprintf(cpu_output, "1_sobel_3x3_cpu.png");
                
                // Processa com FPGA
                printf("Processando com FPGA...\n");
                operation_filter(sobel_gx_3x3, sobel_gy_3x3, 1, filter_result, 0);
                sprintf(output, "1_sobel_3x3_fpga.png");
                
                // Calcula porcentagem de diferença (CPU como referência)
                percentage_diff = calculate_percentage_difference(filter_result_cpu, filter_result);
                print_percentage_report(percentage_diff, "Sobel 3x3");
                
                // Salva imagens
                save_grayscale_png(cpu_output, filter_result_cpu);
                save_grayscale_png(output, filter_result);
                break;
                
            case 2:
                printf("\nAplicando filtro Sobel 5x5...\n");
                
                // Processa com CPU (referência)
                printf("Processando com CPU (referência)...\n");
                operation_filter_cpu(sobel_gx_5x5, sobel_gy_5x5, 3, filter_result_cpu, 0);
                sprintf(cpu_output, "2_sobel_5x5_cpu.png");
                
                // Processa com FPGA
                printf("Processando com FPGA...\n");
                operation_filter(sobel_gx_5x5, sobel_gy_5x5, 3, filter_result, 0);
                sprintf(output, "2_sobel_5x5_fpga.png");
                
                // Calcula porcentagem de diferença
                percentage_diff = calculate_percentage_difference(filter_result_cpu, filter_result);
                print_percentage_report(percentage_diff, "Sobel 5x5");
                
                // Salva imagens
                save_grayscale_png(cpu_output, filter_result_cpu);
                save_grayscale_png(output, filter_result);
                break;
                
            case 3:
                printf("\nAplicando filtro Prewitt 3x3...\n");
                
                // Processa com CPU (referência)
                printf("Processando com CPU (referência)...\n");
                operation_filter_cpu(prewitt_gx_3x3, prewitt_gy_3x3, 1, filter_result_cpu, 0);
                sprintf(cpu_output, "3_prewitt_3x3_cpu.png");
                
                // Processa com FPGA
                printf("Processando com FPGA...\n");
                operation_filter(prewitt_gx_3x3, prewitt_gy_3x3, 1, filter_result, 0);
                sprintf(output, "3_prewitt_3x3_fpga.png");
                
                // Calcula porcentagem de diferença
                percentage_diff = calculate_percentage_difference(filter_result_cpu, filter_result);
                print_percentage_report(percentage_diff, "Prewitt 3x3");
                
                // Salva imagens
                save_grayscale_png(cpu_output, filter_result_cpu);
                save_grayscale_png(output, filter_result);
                break;
                
            case 4:
                printf("\nAplicando filtro Roberts 2x2...\n");
                
                // Processa com CPU (referência)
                printf("Processando com CPU (referência)...\n");
                operation_filter_cpu(roberts_gx_2x2, roberts_gy_2x2, 0, filter_result_cpu, 0);
                sprintf(cpu_output, "4_roberts_2x2_cpu.png");
                
                // Processa com FPGA
                printf("Processando com FPGA...\n");
                operation_filter(roberts_gx_2x2, roberts_gy_2x2, 0, filter_result, 0);
                sprintf(output, "4_roberts_2x2_fpga.png");
                
                // Calcula porcentagem de diferença
                percentage_diff = calculate_percentage_difference(filter_result_cpu, filter_result);
                print_percentage_report(percentage_diff, "Roberts 2x2");
                
                // Salva imagens
                save_grayscale_png(cpu_output, filter_result_cpu);
                save_grayscale_png(output, filter_result);
                break;
                
            case 5:
                printf("\nAplicando filtro Laplaciano 5x5...\n");
                
                // Processa com CPU (referência)
                printf("Processando com CPU (referência)...\n");
                operation_filter_cpu(laplaciano_5x5, kernel_zero, 3, filter_result_cpu, 1);
                sprintf(cpu_output, "5_laplaciano_5x5_cpu.png");
                
                // Processa com FPGA
                printf("Processando com FPGA...\n");
                operation_filter(laplaciano_5x5, kernel_zero, 3, filter_result, 1);
                sprintf(output, "5_laplaciano_5x5_fpga.png");
                
                // Calcula porcentagem de diferença
                percentage_diff = calculate_percentage_difference(filter_result_cpu, filter_result);
                print_percentage_report(percentage_diff, "Laplaciano 5x5");
                
                // Salva imagens
                save_grayscale_png(cpu_output, filter_result_cpu);
                save_grayscale_png(output, filter_result);
                break;
                
            default:
                printf("Opção inválida!\n");
                continue;
        }
    }
    
    // Limpa os recursos
    printf("Liberando recursos do hardware...\n");
    close_hw_access();
    
    return EXIT_SUCCESS;
}