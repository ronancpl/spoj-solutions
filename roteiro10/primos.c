#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STR_MAX 1000
#define LIM_CRIVO 10000000

void RetiraMultiplos(short *crivo,long long mult, long long tam) {
    long long acc = 2*mult;

    for(; acc < tam; acc += mult) {
        crivo[acc] = 0;
    }
}

short *GeraCrivo() {
    short *crivo = (short *)malloc((long long)LIM_CRIVO * sizeof(short));
    long long raiz = sqrt(LIM_CRIVO);

    long long i;
    for(i = 0; i < LIM_CRIVO; i++) crivo[i] = 1;
    crivo[0] = 0; crivo[1] = 0;

    for(i = 2; i <= raiz; i++) {
        if(crivo[i] == 1) {
            RetiraMultiplos(crivo, i, LIM_CRIVO);
        }
    }

    return(crivo);
}

void LiberaCrivo(short *crivo) {
    free(crivo);
}

short IntervaloDentroDoCrivo(long long v1,long long v2) {
    return(v1 >= 1 && v2 < (long long)LIM_CRIVO);
}

void LeIntervalo(long long *v1,long long *v2) {
    char str[STR_MAX],*tok;

    gets(str);
    tok = strtok(str," ");
    *v1 = atoll(tok);

    tok = strtok(NULL," ");
    *v2 = atoll(tok);
}

void ResolveCrivoComum(short *crivo,long long v1, long long v2) {
    long long i;
    for(i = v1; i <= v2; i++) {
        if(crivo[i] == 1) {
            printf("%lld\n",i);
        }
    }
}

void ResolveCrivoSegmentado(short *crivo,long long v1, long long v2) {
    long long tam = v2 - v1;
    short *seg = (short *)malloc((tam + 1)*sizeof(short));

    long long i;
    for(i = 0; i < (tam + 1); i++) seg[i] = 1;

    for (i = 0; i < LIM_CRIVO; i++) {
        if(crivo[i] == 1) {
            if (i >= v2) break;

            long long menor_primo = i * (v1 / i);
            if (menor_primo < v1) menor_primo += i;
            if (i == menor_primo) continue;

            long long j;
            for (j = menor_primo; j <= v2; j += i) seg[j - v1] = 0;
        }
    }

    for(i = 0; i <= tam; i++) {
        if(seg[i] == 1) printf("%lld\n",(long long)(v1 + i));
    }

    free(seg);
}

void ResolveInstancia() {
    long long v1, v2;

    LeIntervalo(&v1,&v2);
    if(v2 < v1) return;

    short *crivo = GeraCrivo();

    if(IntervaloDentroDoCrivo(v1,v2)) {
        ResolveCrivoComum(crivo, v1, v2);
    }
    else {
        ResolveCrivoSegmentado(crivo, v1, v2);
    }

    printf("\n");

    LiberaCrivo(crivo);
}

int main() {
    char str[STR_MAX],*tok;
    gets(str);
    tok = strtok(str," ");

    int i;
    for(i = 0; i < atoi(tok); i++) {
        ResolveInstancia();
    }

    return 0;
}
