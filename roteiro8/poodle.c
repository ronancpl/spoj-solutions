#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_MAX 50
#define POO_MIN 0
#define POO_MAX 20

void LeEntrada(int *docs,int *escopo) {
    char str[STR_MAX],*tok;

    gets(str);

    tok = strtok(str," ");
    *docs = atoi(tok);

    tok = strtok(NULL," ");
    *escopo = atoi(tok);
}

short Poodle(char *str) {
    int docs,escopo;
    LeEntrada(&docs,&escopo);
    if(docs <= 0 || escopo <= 0) return(0);

    int tam = docs / escopo;
    if(docs % escopo > 0) tam++;

    if(tam < POO_MIN) tam = POO_MIN;
    else if(tam > POO_MAX) tam = POO_MAX;

    strcpy(str,"Poo");

    int i;
    for(i = 0; i < tam - 6; i++) strcat(str,"o");
    strcat(str,"dle");

    return(1);
}

int main() {
    char *str = (char *)calloc(STR_MAX,sizeof(char));

    while(Poodle(str)) {
        printf("%s\n",str);
    }

    free(str);
    return 0;
}
