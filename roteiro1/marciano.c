#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BASES 15
#define MAX_CHR 101

struct Registro {
    int bases[MAX_BASES];
    int freq;

    struct Registro *prox;
};

int ContaLista(struct Registro *lista) {
    struct Registro *aux = lista;
    int i = 0;

    while(aux->prox != NULL) {
        i++;
        aux = aux->prox;
    }

    return(i);
}

struct Registro* InicializaLista() {
    struct Registro *lista = (struct Registro *)malloc(sizeof(struct Registro));
    lista->prox = NULL;

    return(lista);
}

void FinalizaLista(struct Registro *lista) {
    struct Registro *aux,*aux2;

    aux = lista;
    while(aux->prox != NULL) {
        aux2 = aux;
        aux = aux->prox;
        free(aux2);
    }
    free(aux);
}

int BasesIdenticas(struct Registro *reg, struct Registro ind) {
    int i;
    for(i = 0; i < MAX_BASES; i++) {
        if(reg->bases[i] != ind.bases[i]) return(0);
    }

    return(1);
}

void InsereIndividuo(struct Registro *lista,struct Registro base_ind) {
    struct Registro *aux = lista;

    while(aux->prox != NULL) {
        if(BasesIdenticas(aux, base_ind)) {
            aux->freq++;
            break;
        }

        aux = aux->prox;
    }

    if(aux->prox == NULL) {
        aux->prox = (struct Registro *)malloc(sizeof(struct Registro));
        aux->prox->prox = NULL;

        int i;
        for(i = 0; i < MAX_BASES; i++) aux->bases[i] = base_ind.bases[i];
        aux->freq = 1;
    }
}

void CodificaBase(struct Registro *reg,char ch) {
    if(ch < 'A' || ch > 'A' + MAX_BASES) return;

    int val = ch - 'A';
    (reg->bases[val])++;
}

struct Registro CodificaIndividuo(char *str) {
    struct Registro ind;

    int i;
    for(i = 0; i < MAX_BASES; i++) ind.bases[i] = 0;
    for(i = 0; i < strlen(str); i++) CodificaBase(&ind,str[i]);

    return(ind);
}

void EfetuaBloco(int qte) {
    char string[MAX_CHR];
    struct Registro *lista = InicializaLista();

    int i;
    for(i = 0; i < qte; i++) {
        gets(string);
        struct Registro ind = CodificaIndividuo(string);
        InsereIndividuo(lista,ind);
    }
    printf("%d\n",ContaLista(lista));
    FinalizaLista(lista);
}

int main() {
    char string[MAX_CHR];
    int qte;

    while(1) {
        gets(string);
        qte = atoi(string);
        if(qte <= 0) break;

        EfetuaBloco(qte);
    }

    return(0);
}
