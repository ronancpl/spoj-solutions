#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_MAX 100000
#define STK_MAX 10000
#define OBJECTIVE 1

typedef struct {
    int pilha[STK_MAX];
    int alt;
} TipoPilha;

int Minimo(int val1,int val2) {
    return((val1 < val2) ? val1 : val2);
}

TipoPilha *InicializaPilha() {
    TipoPilha *p = (TipoPilha *)malloc(sizeof(TipoPilha));
    p->alt = 0;

    return(p);
}

void InsereNaPilha(int item,TipoPilha *p) {
    if(p->alt == STK_MAX) return;

    p->pilha[p->alt] = item;
    (p->alt)++;
}

void FinalizaVetorPilhas(TipoPilha **p,int tam) {
    int i;
    for(i = 0; i < tam; i++) {
        free(p[i]);
    }
    free(p);
}

TipoPilha* LePilha(char *str,int *c1,int *alt) {
    TipoPilha *p;
    char *tok;

    p = InicializaPilha();

    gets(str);
    tok = strtok(str," ");

    int i, qte = atoi(tok);

    for(i = 0; i < qte; i++) {
        tok = strtok(NULL," ");
        int it = atoi(tok);

        if(it == OBJECTIVE) {
            *c1 = 1;
            *alt = i;
        }
        InsereNaPilha(it,p);
    }

    return(p);
}

void VerificaFronteiras(int inicio,int alt_p1,TipoPilha **p,int pilhas,int *inf,int *sup) {
    int i;
    for(i = inicio - 1; i >= 0; i--) {
        if((p[i])->alt <= alt_p1) break;
    }
    *inf = i;

    for(i = inicio + 1; i < pilhas; i++) {
        if((p[i])->alt <= alt_p1) break;
    }
    *sup = i;
}

int LimiteLateral(int inf,int sup,TipoPilha **p,int alt) {
    int val = 0;

    int i;
    for(i = inf; i <= sup; i++) {
        val += (p[i]->alt - alt);
    }

    return(val);
}

int ResolveInstancia() {
    char str[STR_MAX],*tok;

    gets(str);

    tok = strtok(str," ");
    int caixas = atoi(tok);

    tok = strtok(NULL," ");
    int pilhas = atoi(tok);

    if(caixas <= 0 || pilhas <= 0) return(-1);

    TipoPilha **vetor = (TipoPilha **)malloc(pilhas * sizeof(TipoPilha *));
    int i, c1 = 0, pos1 = 0, alt1 = 0;
    for(i = 0; i < pilhas; i++) {
        vetor[i] = LePilha(str,&c1,&alt1);

        if(c1 == 1) {
            pos1 = i;
            c1 = 0;
        }
    }

    int inf,sup;
    VerificaFronteiras(pos1,alt1,vetor,pilhas,&inf,&sup);

    int x = LimiteLateral(inf + 1,pos1 - 1,vetor,alt1);
    int y = LimiteLateral(pos1 + 1,sup - 1,vetor,alt1);

    int res = Minimo(x,y) + (vetor[pos1])->alt - alt1 - 1;

    FinalizaVetorPilhas(vetor,pilhas);

    return(res);
}

int main() {
    while(1) {
        int val = ResolveInstancia();
        if(val == -1) break;

        printf("%d\n",val);
    }

    return 0;
}
