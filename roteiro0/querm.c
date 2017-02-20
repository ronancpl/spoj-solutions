#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 6 * 10000

int querm(char *str, int len) {
    char *tok;
    int i;
    int winner = 0;

    gets(str);
    tok = strtok(str, " ");
    for(i = 1; i <= len; i++) {
        if(atoi(tok) == i) {
            winner = i;
            break;
        }
        tok = strtok(NULL, " ");
    }

    return(winner);
}

int main() {
    char str[MAX_STR];
    int len, st = 1;

    gets(str);
    while((len = atoi(str)) > 0) {
        printf("Teste %d\n%d\n\n", st, querm(str,len));
        gets(str);
        st++;
    }

    return 0;
}
