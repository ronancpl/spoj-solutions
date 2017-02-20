#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 200

void Limites(int v1,int v2,int *min,int *max) {
    if(v1 < v2) {
        *min = v1;
        *max = v2;
    }
    else {
        *min = v2;
        *max = v1;
    }
}

void BuscaCoordenadas_Retangulo(int *posX1,int *posY1,int *posX2,int *posY2) {
    char str[MAX_STR], *tok;
    gets(str);

    tok = strtok(str," ");
    *posX1 = atoi(tok);

    tok = strtok(NULL," ");
    *posY1 = atoi(tok);

    tok = strtok(NULL," ");
    *posX2 = atoi(tok);

    tok = strtok(NULL," ");
    *posY2 = atoi(tok);
}

void BuscaCoordenadas(int *posX,int *posY) {
    char str[MAX_STR], *tok;
    gets(str);

    tok = strtok(str," ");
    *posX = atoi(tok);

    tok = strtok(NULL," ");
    *posY = atoi(tok);
}

short DentroRetangulo(int pX, int pY, int posX1, int posY1, int posX2, int posY2) {
    int minX, maxX, minY, maxY;

    Limites(posX1,posX2,&minX,&maxX);
    Limites(posY1,posY2,&minY,&maxY);

    if(pX >= minX && pX <= maxX && pY >= minY && pY <= maxY) return(1);
    return(0);
}

short CalculaMeteoro(int teste) {
    int posX1, posY1, posX2, posY2;
    BuscaCoordenadas_Retangulo(&posX1,&posY1,&posX2,&posY2);

    if(posX1 == 0 && posY1 == 0 && posX2 == 0 && posY2 == 0) return(0);

    char str[MAX_STR], *tok;
    gets(str);

    tok = strtok(str," ");
    int i, qte = atoi(tok);

    int acc = 0;
    int pX, pY;
    for(i = 0; i < qte; i++) {
        BuscaCoordenadas(&pX,&pY);
        if(DentroRetangulo(pX,pY,posX1,posY1,posX2,posY2)) acc++;
    }

    printf("Teste %d\n",teste);
    printf("%d\n\n",acc);

    return(1);
}

int main() {
    int teste = 1;

    while(CalculaMeteoro(teste)) {
        teste++;
    }

    return 0;
}
