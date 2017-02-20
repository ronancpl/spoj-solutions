#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_STR 1000

typedef struct {
    float x;
    float y;

    float diam;
} Ponto;

void ParticaoX(int Esq,int Dir,int *i,int *j,Ponto *A) {
    Ponto x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2]; /* obtem o pivo x */
    do {
        while (x.x > A[*i].x) (*i)++;
        while (x.x < A[*j].x) (*j)--;
        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void QuicksortX(int Esq,int Dir,Ponto *A) {
    int i,j;

    ParticaoX(Esq, Dir, &i, &j, A);
    if (Esq < j) QuicksortX(Esq, j, A);
    if (i < Dir) QuicksortX(i, Dir, A);
}

void ParticaoY(int Esq,int Dir,int *i,int *j,Ponto *A) {
    Ponto x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2]; /* obtem o pivo x */
    do {
        while (x.y > A[*i].y) (*i)++;
        while (x.y < A[*j].y) (*j)--;
        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void QuicksortY(int Esq,int Dir,Ponto *A) {
    int i,j;

    ParticaoY(Esq, Dir, &i, &j, A);
    if (Esq < j) QuicksortY(Esq, j, A);
    if (i < Dir) QuicksortY(i, Dir, A);
}

void OrdenaPontos(Ponto *A,int tam) {
    QuicksortX(0,tam - 1, A);

    int *intersec = (int *)calloc(tam + 1,sizeof(int));
    int topo = 1;

    intersec[0] = 0;

    int i;
    for(i = 1; i < tam; i++) {
        if(A[i - 1].x != A[i].x) {
            intersec[topo] = i;
            topo++;
        }
    }
    intersec[topo] = tam;

    for(i = 0; i < topo; i++) {
        QuicksortY(intersec[i],intersec[i+1] - 1,A);
    }

    free(intersec);

    /*printf("ordem: ");
    for(i = 0; i < tam; i++) {
        printf("(%.2f %.2f) ",A[i].x,A[i].y);
    }
    printf("\n");
    */
}

Ponto BuscaPonto(char *str,char *tok,float diam) {
    Ponto p;

    gets(str);
    tok = strtok(str," ");
    p.x = (float)atoi(tok);

    tok = strtok(NULL," ");
    p.y = (float)atoi(tok);

    p.diam = diam;
    return(p);
}

short InicializaPontos(Ponto **lista,int *tam) {
    char str[MAX_STR],*tok;

    *lista = NULL;

    gets(str);
    if(feof(stdin)) return(0);

    tok = strtok(str," ");
    int pontos = atoi(tok);

    tok = strtok(NULL," ");     //tecnicamente nao vai ner necessario para este programa,
    int diam = atoi(tok);       //pode ignorar para convex hull com itens de mesmo diametro.

    if(pontos <= 0) return(0);

    *lista = (Ponto *)malloc(pontos * sizeof(Ponto));
    int i;
    for(i = 0; i < pontos; i++) {
        (*lista)[i] = BuscaPonto(str,tok,diam);
    }

    OrdenaPontos(*lista,pontos);
    *tam = pontos;
    return(1);
}

void FinalizaPontos(Ponto *lista) {
    free(lista);
}

float ProdutoVetorial(Ponto O, Ponto A, Ponto B) {
    // Returns a positive value, if OAB makes a counter-clockwise turn,
    // Negative for clockwise turn, and zero if the points are collinear.

    return(((A.x - O.x) * (B.y - O.y)) - ((A.y - O.y) * (B.x - O.x)));
}

void GeraConvexHull(Ponto *lista,int pontos,Ponto **ch,int *ch_tam) {
    //algoritmo: Andrew's monotone chain convex hull

    Ponto *up_lista = (Ponto *)malloc(pontos * sizeof(Ponto));
    int up_tam = 0;

    Ponto *lo_lista = (Ponto *)malloc(pontos * sizeof(Ponto));
    int lo_tam = 0;

    //build lower hull
    int i;
    for(i = 0; i < pontos; i++) {
        while(lo_tam >= 2 && ProdutoVetorial(lo_lista[lo_tam - 2], lo_lista[lo_tam - 1], lista[i]) <= 0) {
            lo_tam--;
        }

        lo_lista[lo_tam] = lista[i];
        lo_tam++;
    }

    //build upper hull
    for(i = pontos - 1; i >= 0; i--) {
        while(up_tam >= 2 && ProdutoVetorial(up_lista[up_tam - 2], up_lista[up_tam - 1], lista[i]) <= 0) {
            up_tam--;
        }

        up_lista[up_tam] = lista[i];
        up_tam++;
    }

    up_tam--;   //ultimo ponto eh o primeiro da outra lista
    lo_tam--;

    for(i = 0; i < lo_tam; i++) {
        up_lista[up_tam] = lo_lista[i];
        up_tam++;
    }

    free(lo_lista);

    *ch = up_lista;
    *ch_tam = up_tam;
}

float DistanciaPontos(Ponto A,Ponto B) {
    return(sqrt(((A.x - B.x)*(A.x - B.x)) + ((A.y - B.y)*(A.y - B.y))));
}

float CalculaComprimento(Ponto *lista,int tam) {
    float comp = 0.0;

    int i;
    for(i = 1; i < tam; i++) {
        comp += DistanciaPontos(lista[i],lista[i-1]);
    }
    comp += DistanciaPontos(lista[tam - 1],lista[0]);

    return(comp);
}

float ComprimentoCerca() {
    Ponto *lista;
    int lista_tam;
    if(!InicializaPontos(&lista,&lista_tam)) return(-1.0);

    Ponto *ch;
    int ch_tam;

    GeraConvexHull(lista,lista_tam,&ch,&ch_tam);
    float comp = CalculaComprimento(ch,ch_tam);

    FinalizaPontos(ch);
    FinalizaPontos(lista);

    return(comp);
}

int main() {

    while(1) {
        float res = ComprimentoCerca();
        if(res == -1.0) break;

        printf("%.2f\n",res);
    }

    return 0;
}
