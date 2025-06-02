module Coprocessor (
    input [2:0] op_code,                    // Código da operação a ser executada
    input [1:0] matrix_size,                // Tamanho da matriz (2x2, 3x3, 4x4, 5x5)
    input [199:0] matrix_a,                 // Matriz A de entrada
    input [199:0] matrix_b,                 // Matriz B de entrada (Kernel Gx)
    input [199:0] matrix_c,                 // Matriz C de entrada (Kernel Gy)
    output reg process_Done,                
    output reg [199:0] result_final                                  
);

    // Resultados das convoluções
    wire signed [15:0] conv_gx, conv_gy;
    
    // Instâncias das convoluções
    ConvolutionModule conv_gx_unit (
        .matrix_a(matrix_a),
        .matrix_b(matrix_b),
        .matrix_size(matrix_size),
        .result_out(conv_gx)
    );
    
    ConvolutionModule conv_gy_unit (
        .matrix_a(matrix_a),
        .matrix_b(matrix_c),
        .matrix_size(matrix_size),
        .result_out(conv_gy)
    );
    
    // Sinais para cálculo da magnitude do gradiente
    wire [31:0] gx_squared, gy_squared, sum_squares;
    wire [15:0] sqrt_result;
    wire [16:0] sqrt_remainder;
    
    // Cálculo: Gx² + Gy²
    assign gx_squared = conv_gx * conv_gx;
    assign gy_squared = conv_gy * conv_gy; 
    assign sum_squares = gx_squared + gy_squared;
    
    // Instância do módulo ALTSQRT de 32 bits
    sqrt sqrt_unit (
        .radical(sum_squares),      
        .q(sqrt_result),           
        .remainder(sqrt_remainder)
    );
    
    // Saturação para filtro de borda (0-255)
    wire [7:0] saturated_result;
    
    // Saturação: limita o resultado a 8 bits (0-255)
    assign saturated_result = (sqrt_result > 16'd255) ? 8'd255 : sqrt_result[7:0];
    
    // Lógica de controle principal
    always @(*) begin
        case (op_code)
            3'b111: begin
                result_final = {192'b0, saturated_result};   
                process_Done = 1'b1;                     
            end
            default: begin 
                result_final = 200'b0;
                process_Done = 1'b0;
            end
        endcase
    end

endmodule
