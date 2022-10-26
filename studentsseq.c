#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

typedef struct {
    int min;
    int max;
    double mediana;
    double media;
    double desvioPadrao;
} data_t;


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

void printRegs(data_t **cidades, data_t *regioes, data_t brasil, int* melhores, double tempo, int R_regioes, int C_cidades) {
    for (int r=0; r<R_regioes; r++) {
        for (int c=0; c<C_cidades; c++) {
            printf("Reg %d - Cid %d: ", r, c);
            printf("menor: %d, maior: %d, ", cidades[r][c].min, cidades[r][c].max);
            printf("mediana %.2f, media %.2f e DP %.2f", cidades[r][c].mediana, cidades[r][c].media, cidades[r][c].desvioPadrao);
            printf("\n");
        }
        printf("\n\n");
    }

    for (int r=0; r<R_regioes; r++) {
        printf("Reg %d: ", r);
        printf("menor: %d, maior: %d, ", regioes[r].min, regioes[r].max);
        printf("mediana %.2f, media %.2f e DP %.2f", regioes[r].mediana, regioes[r].media, regioes[r].desvioPadrao);
        printf("\n");
    }
    printf("\n\n");

    printf("Brasil: ");
    printf("menor: %d, maior: %d, ", brasil.min, brasil.max);
    printf("mediana %.2f, media %.2f e DP %.2f", brasil.mediana, brasil.media, brasil.desvioPadrao);
    printf("\n");
    printf("\n\n");

    printf("Melhor região: Região %d\n", melhores[2]); 
    printf("Melhor cidade: Região %d, Cidade %d\n", melhores[0], melhores[1]);
    printf("\n\n");

    printf("Tempo de resposta sem considerar E/S, em segundos %.5f\n", tempo);
}



int *pegaFrequencia(int* notas, int A_alunos) {
    int *frequencias = (int*) calloc (101, sizeof(int));
    for (int i=0; i<A_alunos; i++)  frequencias[notas[i]]++;
    return frequencias;
}

int *acumulaFrequencia(int **frequencias, int n_frequencias) {
    int *frequenciaAcumulada = (int*) calloc (101, sizeof(int));

    for (int f=0; f<n_frequencias; f++) {
        for (int i=0; i<101; i++)
            frequenciaAcumulada[i] += frequencias[f][i];
    }

    return frequenciaAcumulada;
}



int pegaMax(int *frequencias) {
    for (int i=100; i>0; i--)   if (frequencias[i]) return i;
    return 0;
}

int pegaMin(int *frequencias) {
    for (int i=0; i<100; i++)   if (frequencias[i]) return i;
    return 100;
}

int pegaMediana(int *frequencias, int a_alunos) {
    int i = 0;
    int soma = 0;
    while (soma < (a_alunos/2)) {
        soma += frequencias[i];
        i++;
    }
    int mediana = i-1;

    if (a_alunos % 2 == 0) {
        int j = i;
        while (!frequencias[j] && j++);
        mediana = (i + j)/2.0;
    }

    return mediana;
}

double pegaMedia(int *frequencias, int a_alunos) {
    double soma = 0.0;
    for (int i=0; i<101; i++)   soma += i*frequencias[i];
    return soma / a_alunos * 1.0;
}

int pegaDP(int *frequencias, int a_alunos, double media) {
    double soma = 0.0;
    for (int i=0; i<101; i++)   soma += pow(((i*frequencias[i]) - media), 2);
    return sqrt(soma/a_alunos);
}



data_t **pegaCidades(int R_regioes, int C_cidades, int A_alunos, int** notas, int*** freqCid) {
    data_t **cidades = (data_t**) malloc(R_regioes*sizeof(data_t*));
    
    for (int r=0; r<R_regioes; r++) {
        cidades[r] = (data_t*) malloc(C_cidades*sizeof(data_t));
        freqCid[r] = (int**) malloc(C_cidades*sizeof(int*));

        for (int c=0; c<C_cidades; c++) {
            int linha = r*C_cidades + c;
            freqCid[r][c] = pegaFrequencia(notas[linha], A_alunos);

            cidades[r][c].max = pegaMax(freqCid[r][c]);
            cidades[r][c].min = pegaMin(freqCid[r][c]);
            cidades[r][c].mediana = pegaMediana(freqCid[r][c], A_alunos);
            
            cidades[r][c].media = pegaMedia(freqCid[r][c], A_alunos);
            cidades[r][c].desvioPadrao = pegaDP(freqCid[r][c], A_alunos, cidades[r][c].media);
        }
    }

    return cidades;
}

data_t *pegaRegioes(int R_regioes, int C_cidades, int A_alunos, int*** freqCid, int** freqReg) {
    data_t *regioes = (data_t*) malloc(R_regioes*sizeof(data_t));

    for (int r=0; r<R_regioes; r++) {
        freqReg[r] = acumulaFrequencia(freqCid[r], C_cidades);

        regioes[r].max = pegaMax(freqReg[r]);
        regioes[r].min = pegaMin(freqReg[r]);
        regioes[r].mediana = pegaMediana(freqReg[r], A_alunos*C_cidades);
        
        regioes[r].media = pegaMedia(freqReg[r], A_alunos*C_cidades);
        regioes[r].desvioPadrao = pegaDP(freqReg[r], A_alunos*C_cidades, regioes[r].media);
    }

    return regioes;
}

data_t pegaBrasil(int R_regioes, int C_cidades, int A_alunos, int** freqReg) {
    data_t brasil;
    int *freqBr =  acumulaFrequencia(freqReg, R_regioes);

    brasil.max = pegaMax(freqBr);
    brasil.min = pegaMin(freqBr);
    brasil.mediana = pegaMediana(freqBr, A_alunos*C_cidades*R_regioes);
    
    brasil.media = pegaMedia(freqBr, A_alunos*C_cidades*R_regioes);
    brasil.desvioPadrao = pegaDP(freqBr, A_alunos*C_cidades*R_regioes, brasil.media);

    free(freqBr);
    return brasil;
}

int *pegaMelhores(data_t** cidades, data_t* regioes, int C_cidades, int R_regioes) {
    int *m = (int*) calloc (3, sizeof(int));    // m -- melhor
    for (int r=0; r<R_regioes; r++) {
        for (int c=0; c<C_cidades; c++) {
            if (cidades[r][c].media > cidades[m[0]][m[1]].media){
                m[0] = r;
                m[1] = c;
            }
        }

        if(regioes[r].media > regioes[m[2]].media)  m[2] = r;
    }

    return m;
}


int main() {
    int R_regioes, C_cidades, A_alunos, SEED;
    scanf("%d", &R_regioes);
    scanf("%d", &C_cidades);
    scanf("%d", &A_alunos);
    scanf("%d", &SEED);

    int **notas = geraNotas(R_regioes, C_cidades, A_alunos, SEED);


    double wtime = omp_get_wtime ( );

    int*** freqCidades = (int***) malloc (R_regioes*sizeof(int**));
    int** freqRegioes = (int**) malloc (R_regioes*sizeof(int*));

    data_t **dataCid = pegaCidades(R_regioes, C_cidades, A_alunos, notas, freqCidades);
    data_t *dataReg = pegaRegioes(R_regioes, C_cidades, A_alunos, freqCidades, freqRegioes);
    data_t dataBra = pegaBrasil(R_regioes, C_cidades, A_alunos, freqRegioes);
    int* melhores = pegaMelhores(dataCid, dataReg, C_cidades, R_regioes);

    wtime = omp_get_wtime ( ) - wtime;


    // printRegs(dataCid, dataReg, dataBra, melhores, wtime, R_regioes, C_cidades);
    printf("seq time -> %.4f\n", wtime);


    // Free used memory
    for (int r=0; r<R_regioes; r++) {
        for (int c=0; c<C_cidades; c++) {
            free(notas[C_cidades*r + c]);
            free(freqCidades[r][c]);
        }
        free(freqRegioes[r]);
        free(freqCidades[r]);
        free(dataCid[r]);
    }
    free(freqRegioes);
    free(freqCidades);
    free(dataReg);
    free(dataCid);
    free(notas);

    return 0;
}

