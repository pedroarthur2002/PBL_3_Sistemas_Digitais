# PROBLEMA 3 - SISTEMAS DIGITAIS - Processamento de Imagens

Repositório para armazenar os códigos do processamento de imagens. Ele implementa os filtros de:

1. Sobel 3x3
2. Sobel Expandido 5x5
3. Prewitt 3x3
4. Roberts 2x2
5. Laplaciano 5x5

## Como executar

Certifique-se de que você possui o compilador GCC e a biblioteca `libpng` instalados:

```bash
sudo apt update
sudo apt install build-essential libpng-dev
```
### Compilação

Salve o código como conversor_png.c e compile com o seguinte comando:

```bash
gcc conversor_png.c -o conversor_png -lpng
```
#### Aviso

Caso a imagem não esteja na resolução requerida, execute o comando: 

```bash
convert entrada.png -resize 320x240! imagem.png
```
### Execução

1. Coloque uma imagem PNG chamada imagem.png com 320x240 pixels na mesma pasta.
2. Execute o programa:

```bash
./conversor_png
```
3. O programa vai gerar uma imagem chamada `imagem_grayscale.png` em escala de cinza.
