#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARC_INF 1000000
#define MAX_STR 100

int Minimo(int val1,int val2) {
    return val1 < val2 ? val1 : val2;
}

#define BRANCO 0
#define CINZA 1
#define PRETO 2

typedef struct {
    int *vetor;
    int topo;
    int fundo;

    int limite;
} Fila;

Fila* InicializaFila(int limite) {
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    fila->vetor = (int *)malloc(limite * sizeof(int));
    fila->limite = limite;

    return(fila);
}

void ResetaFila(Fila *fila) {
    fila->topo = fila->fundo = 0;
}

void FinalizaFila(Fila *fila) {
    free(fila->vetor);
    free(fila);
}

void InsereNaFila(Fila *fila,int *cor,int entrada) {
    fila->vetor[fila->topo] = entrada;
    cor[entrada] = CINZA;
    fila->topo = (fila->topo + 1) % fila->limite;
}

int RetiraDaFila(Fila *fila,int *cor) {
    if(fila->topo == fila->fundo) return(-1);
    cor[fila->vetor[fila->fundo]] = PRETO;

    int x = fila->vetor[fila->fundo];
    fila->fundo = (fila->fundo + 1) % fila->limite;
    return(x);
}

short ehFilaVazia(Fila *fila) {
    return(fila->topo == fila->fundo);
}

short bfs(int orig, int dest,int num,int **capacidade,int **fluxo,int *cor,int *path, Fila *fila) {
    int i;

    for(i = 0; i < num; i++) {
        cor[i] = BRANCO;
    }
    ResetaFila(fila);

    InsereNaFila(fila,cor,orig);
    path[orig] = -1;

    int val;
    while(!ehFilaVazia(fila)) {
        val = RetiraDaFila(fila,cor);

        //busca por nos brancos, com valores residuais POSITIVOS.
        for(i = 0; i < num; i++) {
            if(cor[i] == BRANCO && capacidade[val][i] - fluxo[val][i] > 0) {
                InsereNaFila(fila,cor,i);
                path[i] = val;
            }
        }
    }

    return(cor[dest] == PRETO);
}

int **InicializaMatriz(int num) {
    int **M;
    int i;

    M = (int **)malloc(num*sizeof(int *));
    for(i = 0 ; i < num; i++) {
        M[i] = (int *)calloc(num,sizeof(int));
    }

    return(M);
}

void ResetaMatriz(int **M,int num) {
    int i,j;

    for(i = 0; i < num; i++) {
        for(j = 0; j < num; j++) {
            M[i][j] = 0;
        }
    }
}

void FinalizaMatriz(int **M,int num) {
    int i;
    for(i = 0; i < num; i++) {
        free(M[i]);
    }
    free(M);
}

void MaximizaFluxo(int **fluxo,int **capacidade,int num,int *f_max) {
    int i;

    for(i = 1; i < num; i++) {
        fluxo[0][i] = capacidade[0][i];
        (*f_max) += capacidade[0][i];
    }
}

int MaxFlow(int **capacidade,int orig,int dest,int num) {
    int **fluxo = InicializaMatriz(num);
    int *cor = (int *)malloc(num*sizeof(int));
    int *path = (int *)malloc(num*sizeof(int));
    Fila *fila = InicializaFila(num);

    int fluxo_max = 0;

    ResetaMatriz(fluxo,num);
    MaximizaFluxo(fluxo,capacidade,num,&fluxo_max);

    //ainda existe possibilidade de passar fluxo da origem ao destino
    while(bfs(orig,dest,num,capacidade,fluxo,cor,path,fila)) {
        //determinar quantidade de fluxo adicional

        int f = ARC_INF;
        int i;

        for(i = num-1; path[i] >= 0; i = path[i]) {
            f = Minimo(f,capacidade[path[i]][i] - fluxo[path[i]][i]);
        }

        for(i = num-1; path[i] >= 0; i = path[i]) {
            fluxo[path[i]][i] += f;
            fluxo[i][path[i]] -= f;
        }

        fluxo_max += f;
    }

    FinalizaFila(fila);
    FinalizaMatriz(fluxo,num);
    FinalizaMatriz(capacidade,num);
    free(cor);
    free(path);

    return fluxo_max;
}

short ReadHooligan(int ***capacidade,int *tam,int *pontos) {
    int **grafo = NULL;
    int *soma_pontos,*soma_partidas,*partidas,*pt;
    char str[MAX_STR],*tok;

    int times, enfr, concl;

    gets(str);
    tok = strtok(str," ");
    times = atoi(tok);

    tok = strtok(NULL," ");
    enfr = atoi(tok);

    tok = strtok(NULL," ");
    concl = atoi(tok);

    if(times == 0 && enfr == 0 && concl == 0) return(0);

    if(times == 0) return(1);

    grafo = InicializaMatriz(times + 2);    //times + src + sick
    soma_pontos = (int *)calloc(times,sizeof(int));
    soma_partidas = (int *)calloc(times,sizeof(int));
    partidas = (int *)calloc(times,sizeof(int));
    pt = (int *)calloc(times,sizeof(int));

    int i,t1,t2,temp;
    char ch;
    for(i = 0; i < concl; i++) {
        gets(str);
        tok = strtok(str," ");
        t1 = atoi(tok);

        tok = strtok(NULL," ");
        ch = tok[0];

        tok = strtok(NULL," ");
        t2 = atoi(tok);

        soma_partidas[t1] += 1;
        soma_partidas[t2] += 1;

        if(t1 == 0 || t2 == 0) {
            partidas[t1] += 1;
            partidas[t2] += 1;
        }

        switch(ch) {
        case '>':
            temp = t2;
            t2 = t1;
            t1 = temp;

        case '<':
            soma_pontos[t2] += 2;
            if(t2 == 0) pt[t1] += 2;
            break;

        default:
            soma_pontos[t1] += 1;
            soma_pontos[t2] += 1;

            if(t2 == 0) pt[t1] += 1;
            if(t1 == 0) pt[t2] += 1;
        }
    }

    //popula grafo de capacidades
    int resto;
    int p = soma_pontos[0] + 2*(((times - 1) * enfr) - soma_partidas[0]);

    grafo[times][0] = p;
    for(i = 1; i < times; i++) {
        resto = 2*(((times - 1) * enfr) - soma_partidas[i]);
        grafo[times][i] = Minimo(resto,p - 1 - soma_pontos[i]);
    }

    int j;

    for(j = 1; j < times; j++) {
        grafo[0][j] = 2*(enfr - partidas[j]) + pt[j];
    }

    for(i = 1; i < times - 1; i++) {
        for(j = i + 1; j < times; j++) {
            grafo[i][j] = 2*enfr;
        }
    }

    for(i = 0; i < times; i++) {
        grafo[i][times + 1] = 2 * ((enfr * times*(times - 1)) - concl);
    }

    free(soma_pontos);
    free(soma_partidas);
    free(partidas);
    free(pt);

    *pontos = 2 * enfr * times * (times - 1);
    *tam = times + 2;
    *capacidade = grafo;
    return(1);
}

short RunHooligan() {
    int **cap, tam, pts;

    if(ReadHooligan(&cap,&tam,&pts)) {
        if(cap != NULL) {
            if(MaxFlow(cap,tam - 2,tam - 1,tam) >= pts) printf("Y\n");
            else printf("N\n");
        }
        else printf("-1\n");

        return(1);
    }

    return(0);
}

int main() {
    while(RunHooligan()) {}
    return 0;
}
