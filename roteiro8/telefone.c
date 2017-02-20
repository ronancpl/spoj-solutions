#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 50

void ParseTelefone(char *str) {
    int i,max = strlen(str);
    int dist[8] = {3,3,3,3,3,4,3,4};

    for(i = 0; i < max; i++) {
        if(str[i] >= 'A' && str[i] <= 'Z') {
            int pos = str[i] - 'A';

            int j = 0;
            while(pos >= 0) {
                pos -= dist[j];
                j++;
            }

            int num = 1 + j;
            str[i] = num + '0';
        }
    }
}

short LeTelefone(char *saida) {
    char str[MAX_STR],*tok;
    gets(str);

    if(feof(stdin)) return(0);

    ParseTelefone(str);
    strcpy(saida,str);

    return(1);
}

int main() {
    char saida[MAX_STR];

    while(LeTelefone(saida)) {
        printf("%s\n",saida);
    }

    return 0;
}
