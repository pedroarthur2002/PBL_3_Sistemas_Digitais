#include "interface.h"

/* ========== DEFINIÇÕES DOS KERNELS DOS FILTROS CORRIGIDOS ========== */

// Sobel 3x3 - Gradiente X (mapeado para matriz 5x5)
//  0  0  0  0  0
//  0 -1  0  1  0
//  0 -2  0  2  0  
//  0 -1  0  1  0
//  0  0  0  0  0
int8_t sobel_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,
     0, -1,  0,  1,  0,
     0, -2,  0,  2,  0,
     0, -1,  0,  1,  0,
     0,  0,  0,  0,  0
};
// Sobel 3x3 - Gradiente Y (mapeado para matriz 5x5)
//  0  0  0  0  0
//  0 -1 -2 -1  0
//  0  0  0  0  0
//  0  1  2  1  0
//  0  0  0  0  0
int8_t sobel_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,
     0, -1, -2, -1,  0,
     0,  0,  0,  0,  0,
     0,  1,  2,  1,  0,
     0,  0,  0,  0,  0
};
// Sobel 5x5 - Gradiente X (versão mais forte)
int8_t sobel_gx_5x5[MATRIX_SIZE] = {
    -2, -1,  0,  1,  2,
    -3, -2,  0,  2,  3,
    -4, -3,  0,  3,  4,
    -3, -2,  0,  2,  3,
    -2, -1,  0,  1,  2
};
// Sobel 5x5 - Gradiente Y (versão mais forte)
int8_t sobel_gy_5x5[MATRIX_SIZE] = {
    -2, -3, -4, -3, -2,
    -1, -2, -3, -2, -1,
     0,  0,  0,  0,  0,
     1,  2,  3,  2,  1,
     2,  3,  4,  3,  2
};
// Prewitt 3x3 - Gradiente X (mapeado para matriz 5x5)
//  0  0  0  0  0
//  0 -1  0  1  0
//  0 -1  0  1  0
//  0 -1  0  1  0
//  0  0  0  0  0
int8_t prewitt_gx_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,
     0, -1,  0,  1,  0,
     0, -1,  0,  1,  0,
     0, -1,  0,  1,  0,
     0,  0,  0,  0,  0
};
// Prewitt 3x3 - Gradiente Y (mapeado para matriz 5x5)
//  0  0  0  0  0
//  0 -1 -1 -1  0
//  0  0  0  0  0
//  0  1  1  1  0
//  0  0  0  0  0
int8_t prewitt_gy_3x3[MATRIX_SIZE] = {
     0,  0,  0,  0,  0,
     0, -1, -1, -1,  0,
     0,  0,  0,  0,  0,
     0,  1,  1,  1,  0,
     0,  0,  0,  0,  0
};
// Roberts 2x2 - Gradiente X (mapeado para matriz 5x5)
//  1  0  0  0  0
//  0 -1  0  0  0
//  0  0  0  0  0
//  0  0  0  0  0
//  0  0  0  0  0
int8_t roberts_gx_2x2[MATRIX_SIZE] = {
     1,  0,  0,  0,  0,
     0, -1,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};
// Roberts 2x2 - Gradiente Y (mapeado para matriz 5x5)
// Posicionado no canto superior esquerdo:
//  0  1  0  0  0
// -1  0  0  0  0
//  0  0  0  0  0
//  0  0  0  0  0
//  0  0  0  0  0
int8_t roberts_gy_2x2[MATRIX_SIZE] = {
     0,  1,  0,  0,  0,
    -1,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,
     0,  0,  0,  0,  0
};
// Laplaciano 5x5 (kernel mais suave)
int8_t laplaciano_5x5[MATRIX_SIZE] = {
     0,  0, -1,  0,  0,
     0, -1, -2, -1,  0,
    -1, -2, 16, -2, -1,
     0, -1, -2, -1,  0,
     0,  0, -1,  0,  0
};