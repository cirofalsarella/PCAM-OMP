#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    int minPos;
    int maxPos;
    double* mediana;
    double* media;
    double* desvioPadrao;
} reg_t;


/* Matriz [OK]
 - cada célula é uma nota
 - cada linha é uma cidade
 - regiões são grupos de linhas
*/

int **geraNotas(int R_regioes, int C_cidades, int A_alunos, int SEED){
    srand(SEED);

    int n_linhas = R_regioes * C_cidades;
    
    int **notas = (int**) malloc (n_linhas * sizeof(int*));
    for (int i=0; i<n_linhas; i++) {
        notas[i] = (int*) malloc (A_alunos * (sizeof(int)));
        for (int j=0; j<A_alunos; j++)  notas[i][j] = rand() % (100+1);
    }

    return notas;
}

void liberaNotas(int R_regioes, int C_cidades, int** notas) {
    int n_linhas = R_regioes * C_cidades;
    for (int i=0; i<n_linhas; i++) {
        free(notas[i]);
    }
    free(notas);

    return;
}



/* Extrair
 (por [OK]cidade/  [OK]região/ [OK]Brasil)
 - [OK] menor
 - [OK] maior nota
 - [OK] mediana
 - [OK] média aritmética simples
 - [TODO] desvio padrão
*/

void quick_sort(int *notas, int left, int right) {
    int i, j, m;
     
    i = left;
    j = right;
    m = notas[(left + right) / 2];
     
    while (i <= j) {
        while (notas[i] < m && i < right)    i++;
        while (notas[j] > m && j > left)     j--;

        if (i <= j) {
            int aux = notas[i];
            notas[i] = notas[j];
            notas[j] = aux;
            i++;
            j--;
        }
    }
     
    if (j > left)    quick_sort(notas, left, j);
    if (i < right)   quick_sort(notas, i, right);
}

double notaMediana(int inicio, int qtde, int C_cidades, int A_alunos, int** notas) {
    int *ct = (int*) calloc (C_cidades, sizeof(int));
    int vetAnt = 0;
    int vetMin = 0;

    for (int i=0; i<=qtde/2; i++) {
        vetAnt = vetMin;
        int valMin = 101;
        for (int j=0; j<C_cidades; j++) {
            if (ct[j] >= A_alunos)  continue;
            if (notas[inicio + j][ct[j]] < valMin){
                vetMin = j;
                valMin = notas[inicio+j][ct[j]];
            }
        }
        ct[vetMin]++;
    }

    ct[vetMin]--;
    double mediana = notas[inicio+vetMin][ct[vetMin]];
    
    if (qtde%2 == 0) {
        ct[vetAnt]--;
        mediana += notas[inicio+vetAnt][ct[vetAnt]];
        mediana /= 2.0;
    }

    free(ct);
    return mediana;
}

double desvioPadrao(int A_alunos, int l_linhas, int inicio, double media, int** notas) {
    double soma = 0.0;
    for (int l=0; l<l_linhas; l++) {
        for (int a=0; a<A_alunos; a++) {
            soma += pow((notas[l+inicio][a] - media), 2);
        }
    }
    double raiz = sqrt(soma/(A_alunos*l_linhas));
    return raiz;
}

reg_t *extrairRegs(int R_regioes, int C_cidades, int A_alunos, int** notas) {
    reg_t *res = (reg_t*) malloc((R_regioes+1)*sizeof(reg_t));

    int totSum = 0;
    int melhorReg = 0;
    int melhorCid = 0;
    res[R_regioes].minPos = 0;
    res[R_regioes].maxPos = 0;
    res[R_regioes].mediana = (double*) malloc (sizeof(double));
    res[R_regioes].media = (double*) malloc (3*sizeof(double));   
    // media[1] --> melhor região;      media[2] --> melhor cidade;
    
    res[R_regioes].desvioPadrao = (double*) malloc (sizeof(double));

    for (int r=0; r<R_regioes; r++) {
        int rSum = 0;
        res[r].minPos = r*C_cidades;
        res[r].maxPos = r*C_cidades;
        res[r].mediana = (double*) malloc ((C_cidades+1)*sizeof(double));    
        res[r].media = (double*) malloc ((C_cidades+1)*sizeof(double));    
        res[r].desvioPadrao = (double*) malloc ((C_cidades+1)*sizeof(double));
    
        for (int c=0; c<C_cidades; c++) {
            // Ordenar os alunos 
            int linha = r*C_cidades + c;
            quick_sort(notas[linha], 0, A_alunos-1);

            // Mediana (maior e menor implicitos)
            res[r].mediana[c] = notas[linha][A_alunos/2];
            if (A_alunos/2 % 2 == 1) {
                res[r].mediana[c] += notas[linha][A_alunos/2 - 1];
                res[r].mediana[c] /= 2.0;
            }

            int cSum = 0;
            for (int a=0; a<A_alunos; a++)  cSum += notas[linha][a];
            rSum += cSum;

            res[r].media[c] = cSum*1.0/A_alunos;
            if (res[r].media[c] > res[melhorCid / C_cidades].media[melhorCid % C_cidades])  melhorCid = linha;

            res[r].desvioPadrao[c] = desvioPadrao(A_alunos, 1, r*C_cidades + c, res[r].media[c], notas);

            if (notas[linha][0] < notas[res[r].minPos][0])                      res[r].minPos = linha;
            if (notas[linha][A_alunos-1] > notas[res[r].maxPos][A_alunos-1])    res[r].maxPos = linha;
        }

        res[r].media[C_cidades] = rSum*1.0 / (A_alunos * C_cidades);
        if (res[r].media[C_cidades] > res[melhorReg].media[C_cidades])  melhorReg = r;

        res[r].mediana[C_cidades] = notaMediana(r * C_cidades, C_cidades * A_alunos, C_cidades, A_alunos, notas);
        
        res[r].desvioPadrao[C_cidades] = desvioPadrao(A_alunos, C_cidades, r*C_cidades, res[r].media[C_cidades], notas);

        totSum += rSum;
        if (notas[res[r].minPos][0] < notas[res[R_regioes].minPos][0])                      res[R_regioes].minPos = res[r].minPos;
        if (notas[res[r].maxPos][A_alunos-1] > notas[res[R_regioes].maxPos][A_alunos-1])    res[R_regioes].maxPos = res[r].maxPos;
    }

    res[R_regioes].mediana[0] = notaMediana(0, R_regioes * C_cidades * A_alunos, R_regioes * C_cidades, A_alunos, notas);
    res[R_regioes].media[0] = (1.0*totSum) / (A_alunos * C_cidades * R_regioes);
    res[R_regioes].desvioPadrao[0] = desvioPadrao(A_alunos, C_cidades*R_regioes, 0, res[R_regioes].media[0], notas);
    res[R_regioes].media[1] = melhorReg;
    res[R_regioes].media[2] = melhorCid;

    return res;
}

void liberaRegs(reg_t *reg, int size) {
    for (int i=0; i<size; i++) {
        free(reg[i].media);
        free(reg[i].mediana);
        free(reg[i].desvioPadrao);
    }
    free(reg);
}



/* Imprimir [OK]
 - [OK] regiões e suas cidades ordenadas pelos números das mesmas
 - [OK] região e então a cidade Brasileira com as melhores médias das notas
*/

void printRegs(reg_t *reg, double tempo, int R_regioes, int C_cidades, int A_alunos, int **notas) {
    for (int r=0; r<R_regioes; r++) {
        for (int c=0; c<C_cidades; c++) {
            int linha = r * C_cidades + c;
            printf("Reg %d - Cid %d: ", r, c);
            printf("menor: %d, maior: %d, ", notas[linha][0], notas[linha][A_alunos-1]);
            printf("mediana %.2f, media %.2f e DP %.2f", reg[r].mediana[c], reg[r].media[c], reg[r].desvioPadrao[c]);
            printf("\n");
        }
        printf("\n\n");
    }

    for (int r=0; r<R_regioes; r++) {
        printf("Reg %d: ", r);
        printf("menor: %d, maior: %d, ", notas[reg[r].minPos][0], notas[reg[r].maxPos][A_alunos-1]);
        printf("mediana %.2f, media %.2f e DP %.2f", reg[r].mediana[R_regioes], reg[r].media[R_regioes], reg[r].desvioPadrao[R_regioes]);
        printf("\n");
    }
    printf("\n\n");

    printf("Brasil: ");
    printf("menor: %d, maior: %d, ", notas[reg[R_regioes].minPos][0], notas[reg[R_regioes].maxPos][A_alunos-1]);
    printf("mediana %.2f, media %.2f e DP %.2f", reg[R_regioes].mediana[0], reg[R_regioes].media[0], reg[R_regioes].desvioPadrao[0]);
    printf("\n");
    printf("\n\n");

    printf("Melhor região: Região %.0f\n", reg[R_regioes].media[1]); 
    printf("Melhor cidade: Região %.0f, Cidade %ld\n", reg[R_regioes].media[2] / C_cidades, lrint(reg[R_regioes].media[2]) % C_cidades);
    printf("\n\n");

    printf("Tempo de resposta sem considerar E/S, em segundos %.3f\n", tempo);
}



/* Entrada
 - R regiões
 - C cidades (por região)
 - A alunos (por cidade)
 - Seed
*/

int main() {
    int R_regioes, C_cidades, A_alunos, SEED;
    scanf("%d", &R_regioes);
    scanf("%d", &C_cidades);
    scanf("%d", &A_alunos);
    scanf("%d", &SEED);

    int **notas = geraNotas(R_regioes, C_cidades, A_alunos, SEED);

    clock_t t0 = clock();
    reg_t *res = extrairRegs(R_regioes, C_cidades, A_alunos, notas);
    clock_t tf = clock();
    
    printRegs(res, (tf-t0*1.0)/CLOCKS_PER_SEC, R_regioes, C_cidades, A_alunos, notas);


    liberaRegs(res, R_regioes +1);
    liberaNotas(R_regioes, C_cidades, notas);

    return 0;
}

