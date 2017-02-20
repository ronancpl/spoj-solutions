#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAM 1000
#define MAX_CHR 101

typedef struct {
    int valor;
    int freq;
} SomaPar;

SomaPar* SomaPares(int tam,int *A,int *B,int *tam_vetor) {
    SomaPar *vetor = (SomaPar *)malloc(tam * tam * sizeof(SomaPar));

    int i,j,k = 0,l;
    for(i = 0;i < tam; i++) {
        for(j = 0; j < tam; j++) {
            for(l = 0; l < k; l++) {
                if(vetor[l].valor == A[i] + B[j]) {
                    (vetor[l].freq)++;
                    break;
                }
            }
            if(l == k) {
                vetor[k].valor = A[i] + B[j];
                (vetor[k].freq) = 1;
                k++;
            }
        }
    }

    *tam_vetor = k;
    return(vetor);
}

void SomaQuadruplas() {
    int tam1,tam2,tam_lista;
    int i,j,acc;
    char string[MAX_CHR],*tok;

    gets(string);
    tam_lista = atoi(string);

    int *A = (int *)calloc(tam_lista,sizeof(int));
    int *B = (int *)calloc(tam_lista,sizeof(int));
    int *C = (int *)calloc(tam_lista,sizeof(int));
    int *D = (int *)calloc(tam_lista,sizeof(int));

    for(i = 0; i < tam_lista; i++) {
        gets(string);

        tok = strtok(string," \0");
        A[i] = atoi(tok);

        tok = strtok(NULL," \0");
        B[i] = atoi(tok);

        tok = strtok(NULL," \0");
        C[i] = atoi(tok);

        tok = strtok(NULL," \0");
        D[i] = atoi(tok);
    }

    SomaPar *v1 = SomaPares(tam_lista,A,B,&tam1);
    SomaPar *v2 = SomaPares(tam_lista,C,D,&tam2);
    acc = 0;
    for(i = 0; i < tam1; i++) {
        for(j = 0; j < tam2; j++) {
            if(v1[i].valor == -1 * v2[j].valor) {
                acc += (v1[i].freq * v2[j].freq);
            }
        }
    }
    printf("%d",acc);

    free(v1);
    free(v2);

    free(A);
    free(B);
    free(C);
    free(D);
}

int main() {
    SomaQuadruplas();
    return(0);
}
