#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_MAX 2500

int **InicializaMatriz(int tam) {
    int **M = (int **)calloc(tam,sizeof(int *));
    int i;
    for(i = 0; i < tam; i++) {
        M[i] = (int *)calloc(tam,sizeof(int));
        memset(M[i],-1,tam*sizeof(int));
    }

    return(M);
}

void FinalizaMatriz(int **M,int tam) {
    int i;
    for(i = 0; i < tam; i++) free(M[i]);
    free(M);
}

short RecursaoPalindromo(int **M,char *str,int len,int in,int out) {
    //if(in >= len || out < 0) return(1);
    if(M[in][out] > -1) return(M[in][out]);

    if(str[in] == str[out] && (in + 1 >= out - 1 || RecursaoPalindromo(M,str,len,in + 1,out - 1) == 1)) {
        M[in][out] = 1;
    }
    else {
        M[in][out] = 0;
    }

    return(M[in][out]);
}

short VerificaPalindromo(char *str,int **M,int len) {
    if(len == 0) return(1);

    int in = 0, out = len - 1;
    short val = RecursaoPalindromo(M,str,len,in,out);

    return(val);
}

void VerificaParticao(char *str,int len,int **M,int *C) {
    int i,j;


    for(i = 0; i < len; i++) {
        if(RecursaoPalindromo(M,str,len,0,i)) {
            C[i] = 1;
        }

        else {
            for(j = 0; j < i; j++) {
                if(RecursaoPalindromo(M,str,len,j + 1,i) && (C[i] == -1 || C[i] > C[j] + 1)) {
                    C[i] = C[j] + 1;
                }
            }
        }
    }
}

int ContaParticoesPalindromos(char *str) {
    if(strlen(str) == 0) return(0);

    int **M = InicializaMatriz(strlen(str));

    int *C = (int *)malloc(strlen(str)*sizeof(int));
    memset(C,-1,strlen(str)*sizeof(int));

    VerificaPalindromo(str,M,strlen(str));

    VerificaParticao(str,strlen(str),M,C);

    int res = C[strlen(str) - 1];

    FinalizaMatriz(M,strlen(str));
    free(C);

    return(res);
}

int LeEntrada(char *str) {
    char *tok;

    gets(str);
    int val = atoi(str);

    if(val <= 0) return(0);

    gets(str);
    str[val] = 0;
    return(val);
}

int main() {
    int teste = 1;
    char str[STR_MAX];

    while(LeEntrada(str)) {
        printf("Teste %d\n",teste);
        printf("%d\n",ContaParticoesPalindromos(str));
        printf("\n");
        teste++;
    }

    return 0;
}
