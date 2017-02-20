#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN 1000

int **InicializaMochila(int cap, int items) {
    int **moch = (int **)malloc(cap * sizeof(int *));

    int i;
    for(i = 0; i < cap; i++)
        moch[i] = (int *)calloc(items, sizeof(int));

    return(moch);
}

void FinalizaMochila(int **moch,int cap) {
    int i;

    for(i = 0; i < cap; i++)
        free(moch[i]);

    free(moch);
}

void InicializaVetores(int **p, int **v, int items) {
    *p = (int *)malloc((items + 1) * sizeof(int));
    *v = (int *)malloc((items + 1) * sizeof(int));

    char str[STR_LEN],*tok;
    int i;
    for(i = 1; i <= items; i++) {
        gets(str);

        tok = strtok(str," ");
        (*p)[i] = atoi(tok);

        tok = strtok(NULL," ");
        (*v)[i] = atoi(tok);
    }
}

void FinalizaVetores(int *p,int *v) {
    free(p);
    free(v);
}

int Maximo(int v1, int v2) {
    return((v1 > v2) ? v1 : v2);
}

int MochilaBinaria(int cap,int items) {
    if(cap < 1 || items < 1) return(-1);

    int **M = InicializaMochila(cap + 1, items + 1);

    int *P,*V;
    InicializaVetores(&P, &V, items);

    int i;
    for(i = 1; i <= cap; i++) {
        M[i][0] = 0;
    }

    for(i = 1; i <= items; i++) {
        M[0][i] = 0;
    }

    int j;
    for(i = 1; i <= cap; i++) {
        for(j = 1; j <= items; j++) {
            if(P[j] > i) {
                M[i][j] = M[i][j - 1];
            }
            else {
                M[i][j] = Maximo(M[i][j - 1], M[i - P[j]][j - 1] + V[j]);
            }
        }
    }

    int res = M[cap][items];

    FinalizaVetores(P, V);
    FinalizaMochila(M,cap + 1);

    return(res);
}

void LeParametros(int *cap, int *items) {
    char str[STR_LEN],*tok;
    gets(str);

    tok = strtok(str," ");
    *cap = atoi(tok);

    tok = strtok(NULL," ");
    *items = atoi(tok);
}

int main() {
    int cap, items;

    LeParametros(&cap,&items);
    printf("%d", MochilaBinaria(cap,items));

    return 0;
}
