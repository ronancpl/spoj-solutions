#include <stdio.h>
#include <stdlib.h>

#define LIMIT 100
#define CHUNK 100

typedef struct {
    int val1;
    int val2;
} Par;

int Min(int val1,int val2) {
    if(val1 < val2) return(val1);
    return(val2);
}

void ResolveHOT(int x,int y,int a,int b) {
    int saidas = 0;

    int limite = CHUNK;
    Par *vetor = (Par *)malloc(limite*sizeof(Par));

    int b_;
    for(; a <= x; a++) {
        for(b_ = b; b_ <= Min(a-1,y); b_++) {
            if(saidas == limite) {
                limite += CHUNK;
                vetor = (Par *)realloc(vetor, limite*sizeof(Par));
            }

            vetor[saidas].val1 = a;
            vetor[saidas].val2 = b_;

            saidas++;
        }
    }

    printf("%d\n",saidas);

    int i;
    for(i = 0; i < saidas; i++) {
        printf("%d %d\n",vetor[i].val1,vetor[i].val2);
    }

    free(vetor);
}

int main() {
    int a,b,x,y;

    scanf("%d",&x); //P1 tosses
    scanf("%d",&y); //P2 tosses
    scanf("%d",&a); //P1 win times
    scanf("%d",&b); //P2 win times

    ResolveHOT(x,y,a,b);

    return 0;
}
