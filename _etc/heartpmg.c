#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR   101
#define MAX_SLOTS 101
#define MAX_COMBO 10
#define NUM_TYPES 3

int Max(int i, int j) {
    return((i > j) ? i : j);
}

int heartpmg(short *blist, int bl_top, short *value) {
    int dp[MAX_SLOTS][NUM_TYPES][MAX_COMBO];
    int i, j, k;

    //initializing first state
    for(i = 0; i < NUM_TYPES; i++) {
        for(j = 0; j < MAX_COMBO; j++) {
            dp[bl_top][i][j] = 0;
        }
    }

    //iterate over the slots, from finish to start, which will hold the expected result
    for(k = bl_top - 1; k >= 0; k--) {
        for(j = 0; j < NUM_TYPES; j++) {
            int power = 1;

            for(i = 0; i < MAX_COMBO; i++) {
                //is this part of a combo?
                if(blist[k] == j) {
                    //go for it, only one viable option

                    int combo_upgrade = (i < MAX_COMBO - 1) ? combo_upgrade = i + 1 : i;
                    dp[k][j][i] = value[j]*power + dp[k + 1][j][combo_upgrade];
                }
                else {
                    //two choices: wait for a combo, or grab it and reset combo

                    int wait = dp[k + 1][j][i];
                    int grab = value[blist[k]] + dp[k + 1][blist[k]][1];

                    dp[k][j][i] = Max(wait,grab);
                }

                power = power << 1;
            }
        }
    }

    return(dp[0][0][0]);
}

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

int get_type(char ch) {
    return(mod((int)(ch - 'a'), NUM_TYPES));
}

int load_stage(char *str, short *blist, short *value) {
    char *tok;
    int top;

    gets(str);
    tok = strtok(str," ");

    int i;
    for(i = 0; i < NUM_TYPES; i++) {
        //gets the weights of the objects
        value[i] = atoi(tok);

        tok = strtok(NULL," ");
    }

    gets(str);
    top = 0;
    for(tok = str; *tok != '\0'; tok++) {
        blist[top] = get_type(*tok);
        top++;
    }

    return(top);
}

int main() {
    char str[MAX_STR], *tok;
    short blist[MAX_SLOTS];
    short value[NUM_TYPES];

    gets(str);
    tok = strtok(str," ");

    int i, j = atoi(tok);
    for(i = 0; i < j; i++) {
        int top = load_stage(str, blist, value);
        printf("%d\n", heartpmg(blist, top, value));
    }

    return 0;
}
