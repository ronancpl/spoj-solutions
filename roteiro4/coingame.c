#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_LEN 1000

int* JogoMoeda(int len, int K, int L) {
    if(len <= 0) return(NULL);
    int *mem = (int *)calloc(len, sizeof(int));

    int i;
    mem[0] = 0;
    for(i = 1; i < len; i++) {
        if(mem[i - 1] == 0) mem[i] = 1;
        else if(i - K >= 0 && mem[i - K] == 0) mem[i] = 1;
        else if(i - L >= 0 && mem[i - L] == 0) mem[i] = 1;
    }

    return(mem);
}

int Maximo(int v1, int v2) {
    return((v1 > v2) ? v1 : v2);
}

void LeEntrada(int *tam,int *K,int *L,int **inst,int *inst_len) {
    char str[STR_LEN],*tok;
    int val = 0;

    gets(str);

    tok = strtok(str," ");
    *K = atoi(tok);

    tok = strtok(NULL," ");
    *L = atoi(tok);

    tok = strtok(NULL," ");
    *inst_len = atoi(tok);

    *inst = (int *)malloc((*inst_len) * sizeof(int));

    gets(str);
    tok = strtok(str," ");
    int i;
    for (i = 0; i < *inst_len; i++) {
        (*inst)[i] = atoi(tok);
        val = Maximo(val,(*inst)[i]);

        tok = strtok(NULL," ");
    }

    *tam = val + 1;
}

void LiberaVetores(int *jogo,int *inst) {
    free(jogo);
    free(inst);
}

int main() {
    int *inst, inst_len, tam_jogo, K, L;

    LeEntrada(&tam_jogo,&K,&L,&inst,&inst_len);
    int* jogo = JogoMoeda(tam_jogo, K, L);

    if(jogo != NULL) {
        int i;

        for(i = 0; i < inst_len; i++) {
            if(jogo[inst[i]] == 1) {
                printf("A");
            }
            else {
                printf("B");
            }
        }

    }
    else {
        printf("Coin structure error.\n");
    }

    LiberaVetores(jogo,inst);

    return 0;
}
