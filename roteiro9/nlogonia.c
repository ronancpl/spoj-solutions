#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100

void BuscaCoordenadas(int *posX,int *posY) {
    char str[MAX_STR], *tok;
    gets(str);

    tok = strtok(str," ");
    *posX = atoi(tok);

    tok = strtok(NULL," ");
    *posY = atoi(tok);
}

void ImprimeResposta(int val) {
    if((val >> 2) % 2 == 1) printf("divisa");
    else {
        if((val >> 1) % 2 == 0) printf("S");
        else printf("N");

        if((val >> 0) % 2 == 0) printf("O");
        else printf("E");
    }

    printf("\n");
}

short GeraResposta(int pX,int pY,int ptX,int ptY) {
    short res = 0;

    if((pX - ptX == 0) || (pY - ptY == 0)) {
        res += (1 << 2);
        return(res);
    }

    if(pX - ptX > 0) res += (1 << 0);
    if(pY - ptY > 0) res += (1 << 1);

    return(res);
}

short nlogonia() {
    int i,inst;
    int ptX, ptY;
    char str[MAX_STR], *tok;

    gets(str);
    tok = strtok(str," ");
    inst = atoi(tok);
    if(inst <= 0) return(1);

    BuscaCoordenadas(&ptX,&ptY);

    int pX,pY;
    short res;
    for(i = 0; i < inst; i++) {
        BuscaCoordenadas(&pX,&pY);
        res = GeraResposta(pX,pY,ptX,ptY);
        ImprimeResposta(res);
    }

    return(0);
}

int main() {
    while(!nlogonia()) {}   //enquanto tiver casos de teste a serem analisados.
    return 0;
}
