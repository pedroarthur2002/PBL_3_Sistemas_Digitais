#include "interface.h"

/* ========== DEFINIÇÕES DOS KERNELS DOS FILTROS ========== */

// Sobel 3x3 - Gradiente X
// Kernel original:
// -1  0  1
// -2  0  2  
// -1  0  1
// Mapeado para matriz 5x5:
int8_t sobel_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,    // Linha 0 (padding)
     0, -1,  0,  1,  0,    // Linha 1
     0, -2,  0,  2,  0,    // Linha 2  
     0, -1,  0,  1,  0,    // Linha 3
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Sobel 3x3 - Gradiente Y
// Kernel original:
//  1  2  1
//  0  0  0
// -1 -2 -1
int8_t sobel_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,    // Linha 0 (padding)
     0,  1,  2,  1,  0,    // Linha 1
     0,  0,  0,  0,  0,    // Linha 2
     0, -1, -2, -1,  0,    // Linha 3  
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Sobel 5x5 - Gradiente X
int8_t sobel_gx_5x5[MATRIX_SIZE] = {
    -2, -1,  0,  1,  2,
    -2, -1,  0,  1,  2,
    -4, -2,  0,  2,  4,
    -2, -1,  0,  1,  2,
    -2, -1,  0,  1,  2
};

// Sobel 5x5 - Gradiente Y
int8_t sobel_gy_5x5[MATRIX_SIZE] = {
    -2, -2, -4, -2, -2,
    -1, -1, -2, -1, -1,
     0,  0,  0,  0,  0,
     1,  1,  2,  1,  1,
     2,  2,  4,  2,  2
};

// Prewitt 3x3 - Gradiente X
// Kernel original:
// -1  0  1
// -1  0  1
// -1  0  1
int8_t prewitt_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,    // Linha 0 (padding)
     0, -1,  0,  1,  0,    // Linha 1
     0, -1,  0,  1,  0,    // Linha 2
     0, -1,  0,  1,  0,    // Linha 3
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Prewitt 3x3 - Gradiente Y  
// Kernel original:
//  1  1  1
//  0  0  0
// -1 -1 -1
int8_t prewitt_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,    // Linha 0 (padding)
     0,  1,  1,  1,  0,    // Linha 1
     0,  0,  0,  0,  0,    // Linha 2
     0, -1, -1, -1,  0,    // Linha 3
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Roberts 2x2 - Gradiente X
// Kernel original:
//  1  0
//  0 -1
int8_t roberts_gx_2x2[MATRIX_SIZE] = {
     1,  0,  0,  0,  0,    // Apenas os primeiros 4 elementos são usados
     0, -1,  0,  0,  0,    // para representar a matriz 2x2
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

// Roberts 2x2 - Gradiente Y
// Kernel original:
//  0  1
// -1  0  
int8_t roberts_gy_2x2[MATRIX_SIZE] = {
     0,  1,  0,  0,  0,    // Apenas os primeiros 4 elementos são usados
    -1,  0,  0,  0,  0,    // para representar a matriz 2x2
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

// Laplaciano 5x5
int8_t laplaciano_5x5[MATRIX_SIZE] = {
     0,  0,  1,  0,  0,
     0,  1,  2,  1,  0,
     1,  2,-16,  2,  1,
     0,  1,  2,  1,  0,
     0,  0,  1,  0,  0
};