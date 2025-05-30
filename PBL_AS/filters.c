#include "interface.h"

/* ========== DEFINIÇÕES DOS KERNELS DOS FILTROS CORRIGIDOS ========== */

// Sobel 3x3 - Gradiente X
// Kernel original centralizado na matriz 5x5:
// -1  0  1
// -2  0  2  
// -1  0  1
int8_t sobel_gx_3x3[MATRIX_SIZE] = {
     -1,  0,  1, -2, 0,      // Linha 0 (padding)
     2,- 1,  0,  1,  0,      // Linha 1 - CENTRALIZADO
     0,  0,  0,  0,  0,      // Linha 2 - CENTRALIZADO
     0,  0, 0,  0,  0,       // Linha 3 - CENTRALIZADO
     0,  0,  0,  0,  0       // Linha 4 (padding)
};

// Sobel 3x3 - Gradiente Y
// Kernel original centralizado:
//  1  2  1
//  0  0  0
// -1 -2 -1
int8_t sobel_gy_3x3[MATRIX_SIZE] = {
     1,  2,  1, 0,  0,    // Linha 0 (padding)
     0, -1, -2,-1,  0,    // Linha 1 - CENTRALIZADO
     0,  0,  0,  0,  0,   // Linha 2 - CENTRALIZADO
     0,  0,  0,  0,  0,     // Linha 3 - CENTRALIZADO
     0,  0,  0,  0,  0     // Linha 4 (padding)
         
};

// Sobel 5x5 - Gradiente X (corrigido)
int8_t sobel_gx_5x5[MATRIX_SIZE] = {
    -1, -2,  0,  2,  1,
    -4, -8,  0,  8,  4,
    -6,-12,  0, 12,  6,
    -4, -8,  0,  8,  4,
    -1, -2,  0,  2,  1
};

// Sobel 5x5 - Gradiente Y (corrigido)
int8_t sobel_gy_5x5[MATRIX_SIZE] = {
    -1, -4, -6, -4, -1,
    -2, -8,-12, -8, -2,
     0,  0,  0,  0,  0,
     2,  8, 12,  8,  2,
     1,  4,  6,  4,  1
};

// Prewitt 3x3 - Gradiente X (centralizado)
// Kernel original:
// -1  0  1
// -1  0  1
// -1  0  1
int8_t prewitt_gx_3x3[MATRIX_SIZE] = {
     -1, 0,  1,  -1,  0,    // Linha 0 (padding)
     1,  -1,  0,  1,  0,    // Linha 1 - CENTRALIZADO
     0,  0,  0,  0,  0,    // Linha 2 - CENTRALIZADO
     0,  0,  0,  0,  0,    // Linha 3 - CENTRALIZADO
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Prewitt 3x3 - Gradiente Y (centralizado)
// Kernel original:
//  1  1  1
//  0  0  0
// -1 -1 -1
int8_t prewitt_gy_3x3[MATRIX_SIZE] = {
     1,  1,  1,  0,  0,    // Linha 0 (padding)
     0,  -1,  -1,  -1,  0,    // Linha 1 - CENTRALIZADO
     0,  0,  0,  0,  0,    // Linha 2 - CENTRALIZADO
     0, 0, 0, 0,  0,    // Linha 3 - CENTRALIZADO
     0,  0,  0,  0,  0     // Linha 4 (padding)
};

// Roberts 2x2 - Gradiente X (posicionado no canto superior esquerdo)
// Kernel original:
//  1  0
//  0 -1
int8_t roberts_gx_2x2[MATRIX_SIZE] = {
     1,  0,  0,  -1,  0,    
     0,  0,  0,  0,  0,    
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

// Roberts 2x2 - Gradiente Y
// Kernel original:
//  0  1
// -1  0  
int8_t roberts_gy_2x2[MATRIX_SIZE] = {
     0,  1,  0, -1,  0,    
     0,  0,  0,  0,  0,    
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};

// Laplaciano 5x5 (kernel clássico)
int8_t laplaciano_5x5[MATRIX_SIZE] = {
     0,  0, -1,  0,  0,
     0, -1, -2, -1,  0,
    -1, -2, 16, -2, -1,
     0, -1, -2, -1,  0,
     0,  0, -1,  0,  0
};