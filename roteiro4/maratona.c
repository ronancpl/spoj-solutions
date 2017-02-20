#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MARATONA_LEN 42195
#define STR_LEN 210975      //42195 * 5 algarismos

short ExecutaMaratona() {
    char str[STR_LEN],*tok;
    gets(str);

    tok = strtok(str," ");
    int num = atoi(tok);

    tok = strtok(NULL," ");
    int folego = atoi(tok);

    gets(str);

    int val = 0, val_ant = 0;
    tok = strtok(str," ");

    int i;
    for(i = 0; i < num; i++) {
        val = atoi(tok);
        if(val - val_ant > folego) return(0);
        val_ant = val;

        tok = strtok(NULL," ");
    }

    val = MARATONA_LEN;
    if(val - val_ant > folego) return(0);
    return(1);
}

int main() {
    if(ExecutaMaratona()) printf("S");
    else printf("N");

    return 0;
}
