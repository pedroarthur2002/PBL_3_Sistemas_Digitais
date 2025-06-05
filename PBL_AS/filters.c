#include "interface.h" 
/* ========== DEFINIÇÕES DOS KERNELS DOS FILTROS ========== */

// SOBEL 3x3 - Gradiente X
// Kernel original:
// -1,  0,  1
// -2,  0,  2   
// -1,  0,  1
// Posicionado no centro da matriz 5x5 
int8_t sobel_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,   // linha 0
     0, -1,  0,  1,  0,   // linha 1  
     0, -2,  0,  2,  0,   // linha 2
     0, -1,  0,  1,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// SOBEL 3x3 - Gradiente Y  
// Kernel original:
// -1, -2, -1
//  0,  0,  0
//  1,  2,  1
int8_t sobel_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,   // linha 0
     0, -1, -2, -1,  0,   // linha 1
     0,  0,  0,  0,  0,   // linha 2  
     0,  1,  2,  1,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// SOBEL 5x5 - Gradiente X (versão expandida)
int8_t sobel_gx_5x5[MATRIX_SIZE] = {
     2,  2,  4,  2,  2,   // linha 0
     1,  1,  2,  1,  1,   // linha 1
     0,  0,  0,  0,  0,   // linha 2
    -1, -1, -2, -1, -1,   // linha 3
    -2, -2, -4, -2, -2    // linha 4
};

// SOBEL 5x5 - Gradiente Y (versão expandida)
int8_t sobel_gy_5x5[MATRIX_SIZE] = {
     2,  1,  0, -1, -2,   // linha 0
     2,  1,  0, -1, -2,   // linha 1
     4,  2,  0, -2, -4,   // linha 2
     2,  1,  0, -1, -2,   // linha 3
     2,  1,  0, -1, -2,   // linha 4
};

// PREWITT 3x3 - Gradiente X
// Kernel original:
// -1,  0,  1
// -1,  0,  1
// -1,  0,  1
int8_t prewitt_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,   // linha 0
     0, -1,  0,  1,  0,   // linha 1
     0, -1,  0,  1,  0,   // linha 2
     0, -1,  0,  1,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// PREWITT 3x3 - Gradiente Y
// Kernel original:
// -1, -1, -1
//  0,  0,  0
//  1,  1,  1
int8_t prewitt_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,   // linha 0
     0, -1, -1, -1,  0,   // linha 1
     0,  0,  0,  0,  0,   // linha 2
     0,  1,  1,  1,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// ROBERTS 2x2 - Gradiente X
// Kernel original:
//  1,  0
//  0, -1
int8_t roberts_gx_2x2[MATRIX_SIZE] = {
     1,  0,  0,  0,  0,   // linha 0 
     0, -1,  0,  0,  0,   // linha 1
     0,  0,  0,  0,  0,   // linha 2
     0,  0,  0,  0,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// ROBERTS 2x2 - Gradiente Y
// Kernel original:
//  0,  1
// -1,  0
int8_t roberts_gy_2x2[MATRIX_SIZE] = {
     0,  1,  0,  0,  0,   // linha 0
    -1,  0,  0,  0,  0,   // linha 1
     0,  0,  0,  0,  0,   // linha 2
     0,  0,  0,  0,  0,   // linha 3
     0,  0,  0,  0,  0    // linha 4
};

// LAPLACIANO 5x5 (kernel de detecção de borda)
int8_t laplaciano_5x5[MATRIX_SIZE] = {
     0,  0, -1,  0,  0,   // linha 0
     0, -1, -2, -1,  0,   // linha 1
    -1, -2, 16, -2, -1,   // linha 2
     0, -1, -2, -1,  0,   // linha 3
     0,  0, -1,  0,  0    // linha 4
};
