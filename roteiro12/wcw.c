#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_MAX 100000

int *InicializaSequencia(int *tam) {
    char str[STR_MAX],*tok;

    gets(str);
    tok = strtok(str," ");

    int len = atoi(tok);
    *tam = len;

    if(len <= 0) return(NULL);
    int *vetor = (int *)malloc(len * sizeof(int));

    gets(str);
    tok = strtok(str," ");

    int i;
    for(i = 0; i < len; i++) {
        vetor[i] = atoi(tok);
        tok = strtok(NULL," ");
    }

    return(vetor);
}

void FinalizaSequencia(int *vetor) {
    free(vetor);
}

int BuscaNovoInicio(int *seq,int tam,int inicio,int *em_ciclo) {
    int i;
    for(i = inicio; i < tam; i++) {
        if(em_ciclo[i] == 0) break;
    }

    return(i);
}

int ExecutaSequencia() {
    int tam;
    int *seq = InicializaSequencia(&tam);
    if(seq == NULL) return(-1);

    int *em_ciclo = (int *)calloc(tam,sizeof(int));

    int ciclos = 0;
    int inicio = 0;
    while(1) {
        inicio = BuscaNovoInicio(seq,tam,inicio,em_ciclo);
        if(inicio == tam) break;

        int temp = inicio;

        em_ciclo[temp] = 1;
        while(temp + 1 != seq[temp]) {
            int s1 = temp;
            int s2 = seq[temp] - 1;

            int val = seq[s1];
            seq[s1] = seq[s2];
            seq[s2] = val;

            em_ciclo[s2] = 1;
        }

        ciclos++;
    }

    int ret = tam - ciclos;

    FinalizaSequencia(seq);
    free(em_ciclo);

    return(ret);
}

int main() {
    char str[STR_MAX],*tok;

    gets(str);
    tok = strtok(str," ");

    int i,iteracoes = atoi(tok);
    for(i = 0; i < iteracoes; i++) {
        printf("%d\n",ExecutaSequencia());
    }

    return 0;
}
