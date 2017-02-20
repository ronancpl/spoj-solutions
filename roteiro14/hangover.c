#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MEM 100000
#define MAX_STR 200

float BuscaLimite() {
    char str[MAX_STR],*tok;

    gets(str);
    tok = strtok(str," ");
    return(atof(tok));
}

float CalculaRazao(unsigned short val) {
    float res = 1.0 / val;
    return(res);
}

unsigned short ResolveLimite(float lim,float *mem,unsigned short *top_mem) {
    if(mem[*top_mem - 1] > lim) {
        //solucao em memoria

        unsigned short i;
        for(i = 2; i < *top_mem; i++) {
            if(mem[i] > lim) {
                return(i);
            }
        }
    }

    float sum = mem[*top_mem - 1];
    unsigned short ind = *top_mem;
    while(sum <= lim) {
        sum += CalculaRazao(ind);
        mem[ind] = sum;
        ind++;
    }

    *top_mem = ind;
    return(ind - 1);
}

void Hangover() {
    float mem[MAX_MEM];
    unsigned short top_mem = 2;
    mem[0] = 0.0;
    mem[1] = 0.0;

    while(1) {
        float lim = BuscaLimite();
        if(lim == 0.00) break;

        printf("%d card(s)\n",ResolveLimite(lim,mem,&top_mem) - 1);
    }
}

int main() {
    Hangover();
    return 0;
}
