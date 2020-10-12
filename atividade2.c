/*
  Universidade de Sao Paulo - ICMC
  SSC0903 - Computacao de Alto Desempenho
  Atividade 2
  Grupo:
  João Pedro A. S. Secundino (10692054);
  João Pedro Uchôa Cavalcante (10801169);
  Luís Eduardo Rozante de Freitas Pereira (10734794);
  Sérgio Ricardo G. B. Filho (10408386);
*/

# include <omp.h>
# include <math.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# define THREADS 8

// Faz a impressão da matriz. 
void printa_matriz(size_t n, size_t m, double **mat) {

    for(size_t i = 0; i < n; i++) {
        for(size_t j = 0; j < m; j++)
            printf("%.1lf ", mat[i][j]);
        printf("\n");
    }

}

// Função que compara dois números.
int fun_comparacao (const void *a, const void *b){
    return ( *(double *)a - *(double *)b );
}

// Média aritmética Somatório de todos os elementos da amostra, divididos pelo tamanho
// da amostra.
double calcula_media_aritmetica(size_t len, double *linha) {

    // Calcula o somatório dos elementos da linha.
    double sum = 0;
    for(size_t i = 0; i < len; i++)
        sum += linha[i];

    // Retorna a média aritmética dos elementos.
    return (sum / len);

}

// Média harmônica: Razão entre o tamanho da amostra e o somatório do inverso das amostras.
double calcula_media_harmonica(size_t len, double *linha) {

    // Calcula o somatório dos inversos dos elementos da linha.
    double inv_sum = 0;
    for(size_t i = 0; i < len; i++)
        inv_sum += 1 / linha[i];

    // Retorna a média harmônica dos elementos.
    return (len / inv_sum);

}

// Mediana: Elemento médio da amostra (elemento médio da coluna ordenada). Para um
// número par de elementos, a mediana é a média entre os elementos do meio
// ((n/2+n/2+1)/2).
double calcula_mediana(size_t len, double *linha_ordenana) {

    // Retorna a média entre os elementos do meio se len é par.
    if (len % 2 == 0) {
        return (linha_ordenana[len / 2 - 1] + linha_ordenana[len / 2]) / 2;
    }

    // Retorna o elemento do meio se len é ímpar.
    return linha_ordenana[len / 2 - 1];

}

// Moda: Elemento mais frequente da amostra (elemento que mais aparece na coluna, se
// houver mais de um, considera-se somente o primeiro. Se não houver, retorna -1).
double calcula_moda(size_t len, double *linha_ordenana) {
    
    // Guarda o valor a ser retornado como moda e sua frequência.
    double moda = -1.0;
    int freq_moda = 1;

    // Guarda o valor que estamos avaliando.
    double atual = linha_ordenana[0];
    int freq_atual = 0;

    // Analisa os elementos e encontra a moda.
    for (size_t i = 0; i < len; i++){
        
        // Começa a avaliar um novo valor.
        if(linha_ordenana[i] != atual) {
            freq_atual = 1;
            atual = linha_ordenana[i];
        } else {
            freq_atual++;
        }

        // Se a frequência do atual é maior que a da moda, atualiza a moda.
        if(freq_atual > freq_moda) {
            freq_moda = freq_atual;
            moda = atual;
        }

    }

    return moda;

}   

// Variância: Soma dos quadrados das diferenças entre o elemento da amostra e a média
// aritmética calculada.
double calcula_variancia(size_t len, double *linha, double media) {

    // Calcula o somatório dos quadrados das diferenças da linha.
    double sum = 0;
    for(size_t i = 0; i < len; i++) {
        double dif = (linha[i] - media);
        sum += (dif * dif);
    }

    // Retorna a variância da amostra.
    return sum / (len - 1);
}

// Desvio padrão: Raiz quadrada da variância.
double calcula_desvio_padrao(double variancia) {
    return sqrt(variancia);
}

// Coeficiente de variação: Razão entre o desvio padrão e a média aritmética. 
double calcula_coeficiente_variacao(double desvio, double media) {
    return desvio / media;
}

int main() {

    // Escaneia o tamanho da matriz.
    size_t n, m;
    scanf(" %llu %llu", &n, &m);

    // Aloca a matriz transposta.
    double **matriz = calloc(m, sizeof(double*)); // Aloca as linhas.
    for(size_t i = 0; i < m; i++)
        matriz[i] = calloc(n, sizeof(double)); // Aloca a coluna.

    // Lê a entrada transposta.
    for(size_t i = 0; i < n; i++) {
        for(size_t j = 0; j < m; j++)
            scanf(" %lf", &(matriz[j][i]));
    }

    // Aloca a matriz de respostas.
    double **matriz_resposta = calloc(7, sizeof(double*)); // Aloca as linhas.
    for(size_t i = 0; i < 7; i++)
        matriz_resposta[i] = calloc(m, sizeof(double)); // Aloca a coluna.

    // Começa a contar o tempo.
    double wtime = omp_get_wtime ( );

    # pragma omp parallel num_threads(THREADS) shared(m, n, matriz, matriz_resposta)
    {

        # pragma omp single
        {

            // Cria tasks para os dados de cada linha da matriz transposta.
            for(size_t i = 0; i < m; i++) {

                // Cria a task da média aritmética, da variância, do desvio padrão e do coeficiente de variação.
                # pragma omp task
                {
                    // Essas tarefas são executas sequêncialmente pois necessitam do resultado das anteriores, 
                    // tentar paraleliza-las provavelmente causaria uma perda de tempo muito grande com a 
                    // sincronização.

                    double media_a = calcula_media_aritmetica(n, matriz[i]);
                    double variancia = calcula_variancia(n, matriz[i], media_a); // Depende da média.
                    double desvio = calcula_desvio_padrao(variancia); // Depende da variância(por consequência também depende da média).
                    double coef_variacao = calcula_coeficiente_variacao(desvio, media_a); // Depende da media e do desvio padrão (por consequência também depende da variância).

                    matriz_resposta[0][i] = media_a;
                    matriz_resposta[4][i] = variancia;
                    matriz_resposta[5][i] = desvio;
                    matriz_resposta[6][i] = coef_variacao;

                }

                // Cria a task da média harmônica.
                # pragma omp task
                {
                    matriz_resposta[1][i] = calcula_media_harmonica(n, matriz[i]);
                }

                // Cria a task da moda e da mediana.
                # pragma omp task
                {

                    // Essas tarefas são executas juntas e sequencialmente pois necessitam da linha com os dados ordenada. 
                    // Isso é feito para evitar ter que ordenar o vetor duas vezes e para facilitar a sincronização, já que 
                    // a média e mediana são assintóticamente ingsignificantes comparadas a ordenção.

                    // Aloca espaço para guardar a versão ordenada da linha e copia ela.
                    double *linha_ord = malloc(n * sizeof(double));
                    memcpy(linha_ord, matriz[i], n * sizeof(double));

                    // Ordena os elementos da linha.
                    qsort(linha_ord, n, sizeof(double), fun_comparacao); // O(n(log(n)))

                    matriz_resposta[2][i] = calcula_mediana(n, linha_ord); // O(1)
                    matriz_resposta[3][i] = calcula_moda(n, linha_ord); // O(n)

                    // Libera a memória usada para o guardar a linha ordenada.
                    free(linha_ord);

                }

            }

        }

    }

    // Pega o tempo final e calcula a durção da execução.
    wtime = omp_get_wtime ( ) - wtime;

    // Printa a matriz com os resultados
    printa_matriz(7, m, matriz_resposta);
    
    printf("\nElapsed wall clock time = %.5f\n", wtime );

    // Desaloca a matriz transposta.
    for(size_t i = 0; i < m; i++)
        free(matriz[i]);
    free(matriz);

    // Desaloca a matriz de resposta.
    for(size_t i = 0; i < 7; i++)
        free(matriz_resposta[i]);
    free(matriz_resposta);

    return 0;
}
