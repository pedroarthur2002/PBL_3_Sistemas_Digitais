module Coprocessor (
    input [2:0] op_code,                    // Código da operação a ser executada
    input [1:0] matrix_size,                // Tamanho da matriz (2x2, 3x3, 4x4, 5x5)
    input [199:0] matrix_a,                 // Matriz A de entrada
    input [199:0] matrix_b,                 // Matriz B de entrada (Kernel Gx)
    input [199:0] matrix_c,                 // Matriz C de entrada (Kernel Gy)
    output reg process_Done,                // Sinaliza conclusão do processamento
    output reg [199:0] result_final         // Resultado final da operação                          
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
    wire [15:0] sum_squares_16bit;
    wire [7:0] sqrt_input;
    wire [3:0] sqrt_result;
    wire [4:0] sqrt_remainder;
    
    // Cálculo correto: Gx² + Gy² (sem valor absoluto desnecessário)
    assign gx_squared = conv_gx * conv_gx;  // Automaticamente positivo
    assign gy_squared = conv_gy * conv_gy;  // Automaticamente positivo
    assign sum_squares = gx_squared + gy_squared;
    
    // Adaptação para o ALTSQRT (8 bits de entrada)
    // Estratégia: Normalizar o resultado para usar melhor a faixa de 8 bits
    assign sum_squares_16bit = sum_squares[15:0]; // Pega os 16 bits menos significativos
    
    // Escolher os 8 bits mais significativos do resultado para melhor precisão
    assign sqrt_input = (sum_squares > 16'hFF00) ? sum_squares[15:8] :  // Se muito grande, usa MSBs
                       (sum_squares > 16'h00FF) ? sum_squares[11:4] :   // Valor médio, desloca
                       sum_squares[7:0];                                // Valor pequeno, usa LSBs
    
    // Instância do módulo ALTSQRT
    sqrt sqrt_unit (
        .radical(sqrt_input),
        .q(sqrt_result),
        .remainder(sqrt_remainder)
    );
    
    // Função para ajustar o resultado baseado na escala usada
    function [7:0] adjust_sqrt_result;
        input [31:0] original_sum;
        input [3:0] sqrt_4bit;
        
        reg [7:0] adjusted_result;
        
        begin
            if (original_sum > 32'h0000FF00) begin
                // Usamos MSBs, então resultado precisa ser escalado para cima
                adjusted_result = (sqrt_4bit << 4); // Multiplica por 16
                // Saturação
                if (adjusted_result < sqrt_4bit) adjusted_result = 8'hFF;
            end else if (original_sum > 32'h000000FF) begin
                // Usamos bits médios, escalamento menor
                adjusted_result = (sqrt_4bit << 2); // Multiplica por 4
            end else begin
                // Usamos LSBs, sem escalamento
                adjusted_result = {4'b0000, sqrt_4bit};
            end
            
            // Saturação final para 8 bits
            adjust_sqrt_result = adjusted_result;
        end
    endfunction
    
    // Alternativa mais simples e robusta: usar múltiplas instâncias ALTSQRT
    // para diferentes faixas de valores
    wire [7:0] sqrt_input_high, sqrt_input_mid, sqrt_input_low;
    wire [3:0] sqrt_result_high, sqrt_result_mid, sqrt_result_low;
    wire [4:0] sqrt_remainder_high, sqrt_remainder_mid, sqrt_remainder_low;
    
    // Divisão em faixas para melhor cobertura
    assign sqrt_input_high = sum_squares[23:16];  // Valores altos
    assign sqrt_input_mid  = sum_squares[15:8];   // Valores médios  
    assign sqrt_input_low  = sum_squares[7:0];    // Valores baixos
    
    sqrt sqrt_high (
        .radical(sqrt_input_high),
        .q(sqrt_result_high),
        .remainder(sqrt_remainder_high)
    );
    
    sqrt sqrt_mid (
        .radical(sqrt_input_mid),
        .q(sqrt_result_mid),
        .remainder(sqrt_remainder_mid)
    );
    
    sqrt sqrt_low (
        .radical(sqrt_input_low),
        .q(sqrt_result_low),
        .remainder(sqrt_remainder_low)
    );
    
    // Seleção inteligente do resultado baseado na magnitude
    function [7:0] select_best_sqrt;
        input [31:0] sum_val;
        input [3:0] sqrt_h, sqrt_m, sqrt_l;
        
        begin
            if (sum_val > 32'h00010000) begin
                // Valor alto: usa resultado escalado da faixa alta
                select_best_sqrt = (sqrt_h << 4) | sqrt_m; // Combina alta e média
            end else if (sum_val > 32'h00000100) begin
                // Valor médio: usa resultado escalado da faixa média
                select_best_sqrt = (sqrt_m << 2) | (sqrt_l >> 2); // Combina média e baixa
            end else begin
                // Valor baixo: usa resultado direto da faixa baixa
                select_best_sqrt = {4'b0000, sqrt_l};
            end
        end
    endfunction
    
    // Lógica de controle principal
    always @(*) begin
        case (op_code)
            3'b111: begin
                // Usa a seleção inteligente de resultado
                result_final = {192'b0, select_best_sqrt(sum_squares, sqrt_result_high, sqrt_result_mid, sqrt_result_low)};   
                process_Done = 1'b1;                     
            end
            default: begin 
                result_final = 200'b0;
                process_Done = 1'b0;
            end
        endcase
    end

endmodule