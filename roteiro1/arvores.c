#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAM 1000
#define MAX_CHR 31

typedef struct {
    char **arvores;
    int freq[MAX_TAM];

    int populacao;
    int qte;
} RegistroArvore;

void Particao(int Esq,int Dir,int *i,int *j,char **A,int *B) {
    char *x, *w;
    int t;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2]; /* obtem o pivo x */
    do {
        while (strcmp(x,A[*i]) > 0) (*i)++;
        while (strcmp(x,A[*j]) < 0) (*j)--;
        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            t = B[*i];
            B[*i] = B[*j];
            B[*j] = t;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Quicksort(int Esq,int Dir,char **A,int *B) {
    int i,j;

    Particao(Esq, Dir, &i, &j, A, B);
    if (Esq < j) Quicksort(Esq, j, A, B);
    if (i < Dir) Quicksort(i, Dir, A, B);
}

void RegistraArvore(char *arvore,RegistroArvore *ra) {
    int i;
    for(i = 0; i < ra->qte; i++) {
        if(!strcmp(arvore,ra->arvores[i])) {
            (ra->freq[i])++;
            (ra->populacao)++;
            return;
        }
    }

    if(ra->qte < MAX_TAM) {
        strncpy(ra->arvores[ra->qte],arvore,30);
        ra->freq[ra->qte] = 1;
        (ra->qte)++;

        (ra->populacao)++;
    }
}

RegistroArvore* InicializaRegistro() {
    RegistroArvore *ra = (RegistroArvore *)calloc(1,sizeof(RegistroArvore));

    ra->arvores = (char **)malloc(MAX_TAM*sizeof(char *));
    int i;
    for(i = 0; i < MAX_TAM; i++) {
        (ra->arvores)[i] = (char *)malloc(MAX_CHR*sizeof(char));
    }

    ra->populacao = 0;
    ra->qte = 0;

    return(ra);
}

void FinalizaRegistro(RegistroArvore *ra) {
    int i;
    for(i = 0; i < MAX_TAM; i++) {
        free(ra->arvores[i]);
    }
    free(ra->arvores);
    free(ra);
}

void ImprimeRegistro(RegistroArvore *ra) {
    Quicksort(0,ra->qte - 1,ra->arvores,ra->freq);

    int i;
    float res;

    for(i = 0; i < ra->qte; i++) {
        res = 100.0 * (float)ra->freq[i] / (float)ra->populacao;
        printf("%s %.4f\n",ra->arvores[i],res);
    }
    printf("\n");
}

void EfetuaConjunto() {
    RegistroArvore *ra = InicializaRegistro();
    char string[MAX_CHR];

    while(1) {
        gets(string);
        if(strlen(string) == 0) break;

        RegistraArvore(string,ra);
    }

    ImprimeRegistro(ra);
    FinalizaRegistro(ra);
}

int main() {
    char string[MAX_CHR];
    int i,qte;

    gets(string);
    qte = atoi(string);

    for(i = 0; i < qte; i++) {
        EfetuaConjunto();
    }

    return(0);
}
