#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STR_MAX 10000
#define DIST_INF 9999999.999

typedef struct {
    int posX;
    int posY;
} Estacao;

typedef struct {
    int *pilha;
    int topo;
} PilhaEstacao;

Estacao* InicializaEstacoes(int qte) {
    int i;
    char str[STR_MAX],*tok;
    Estacao *lista = (Estacao *)malloc(qte*sizeof(Estacao));

    gets(str);
    tok = strtok(str," ");
    for(i = 0; i < qte; i++) {
        lista[i].posX = atoi(tok);
        tok = strtok(NULL," ");

        lista[i].posY = atoi(tok);
        tok = strtok(NULL," ");
    }

    return(lista);
}

void FinalizaEstacoes(Estacao *lista) {
    free(lista);
}

short PilhaVazia(PilhaEstacao *pilha) {
    return(pilha->topo == 0);
}

void EmpilhaEstacao(short *catalogado,PilhaEstacao *pilha,int val) {
    pilha->pilha[pilha->topo] = val;
    catalogado[val] = 1;
    (pilha->topo)++;
}

int DesempilhaEstacao(PilhaEstacao *pilha) {
    if(pilha->topo == 0) return(-1);

    (pilha->topo)--;
    return(pilha->pilha[pilha->topo]);
}

PilhaEstacao* InicializaPilha(int tam) {
    PilhaEstacao *pilha = (PilhaEstacao *)malloc(sizeof(PilhaEstacao));

    pilha->pilha = (int *)malloc(tam*sizeof(int));
    pilha->topo = 0;

    return(pilha);
}

void FinalizaPilha(PilhaEstacao *pilha) {
    free(pilha->pilha);
    free(pilha);
}

float DistanciaEstacoes(float X1,float X2,float Y1,float Y2) {
    return(sqrt(((X2 - X1)*(X2 - X1)) + ((Y2 - Y1)*(Y2 - Y1))));
}

short DistanciaEhMenor(Estacao *lista,int ind_atual,int ind_novo,float dist_atual,float dist_novo) {
    if(dist_novo < dist_atual) return(1);
    if(dist_novo == dist_atual) {
        if(lista[ind_novo].posX < lista[ind_atual].posX) return(1);
        if(lista[ind_novo].posX == lista[ind_atual].posX) {
            if(lista[ind_novo].posY <= lista[ind_atual].posY) return(1);
        }
    }

    return(0);
}

void BuscaMaisProximos(Estacao *lista, int qte, int val, int *est1, int *est2) {
    float dist1 = DIST_INF;
    float dist2 = DIST_INF;
    int ind1 = -1;
    int ind2 = -1;

    int i;
    for(i = 0; i < qte; i++) {
        if(i == val) continue;

        float dist_novo = DistanciaEstacoes(lista[val].posX,lista[i].posX,lista[val].posY,lista[i].posY);

        if(DistanciaEhMenor(lista,ind1,i,dist1,dist_novo)) {
            dist2 = dist1;
            ind2 = ind1;

            dist1 = dist_novo;
            ind1 = i;
        }
        else if(DistanciaEhMenor(lista,ind2,i,dist2,dist_novo)) {
            dist2 = dist_novo;
            ind2 = i;
        }
    }

    *est1 = ind1;
    *est2 = ind2;
}

short VerificaConexao(short *ativo,int qte) {
    int i;

    for(i = 0; i < qte; i++) {
        if(ativo[i] == 0) return(0);
    }

    return(1);
}

short Amazonia() {
    char str[STR_MAX],*tok;
    gets(str);

    tok = strtok(str," ");
    int qte = atoi(tok);

    if(qte <= 0) return(0);

    Estacao *lista = InicializaEstacoes(qte);
    PilhaEstacao *pilha = InicializaPilha(qte);
    short *ativo = (short *)calloc(qte,sizeof(short));
    short *catalogado = (short *)calloc(qte,sizeof(short));

    EmpilhaEstacao(catalogado,pilha,0);

    while(!PilhaVazia(pilha)) {
        int val = DesempilhaEstacao(pilha);
        ativo[val] = 1;

        int est1,est2;
        BuscaMaisProximos(lista,qte,val,&est1,&est2);

        if(est1 != -1 && catalogado[est1] == 0) EmpilhaEstacao(catalogado,pilha,est1);
        if(est2 != -1 && catalogado[est2] == 0) EmpilhaEstacao(catalogado,pilha,est2);
    }

    if(!VerificaConexao(ativo,qte)) {
        printf("There are stations that are unreachable.\n");
    }
    else {
        printf("All stations are reachable.\n");
    }

    free(catalogado);
    free(ativo);
    FinalizaPilha(pilha);
    FinalizaEstacoes(lista);

    return(1);
}

int main() {
    while(Amazonia()) {}   //enquanto tiver instancias a serem executadas.

    return 0;
}
