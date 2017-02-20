#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define HAND_SZ 5

typedef struct {
    int card;
    int combo;
} MinipokerCombo;

void swap(int *x,int *y) {
    *x = *x ^ *y;
    *y = *y ^ *x;
    *x = *x ^ *y;
}

void bubblesort(int *list, int len) {
    int i,j;

    for(i = 0; i < len; i++) {
        for(j = i + 1; j < len; j++) {
            if(list[i] > list[j])
                swap(&(list[i]),&(list[j]));
        }
    }
}

//ultimo card == 0, placeholder
//next hand_ind to initial
int sequence(int card,int combo,int *hand,int *hand_ind) {
    if(hand[*hand_ind] == card) {
        (*hand_ind)++;
        return(sequence(card, combo + 1, hand, hand_ind));
    }

    return(combo);
}

void swap_combo(MinipokerCombo *x,MinipokerCombo *y) {
    MinipokerCombo temp = *x;
    *x = *y;
    *y = temp;
}

void bubblesort_combo(MinipokerCombo *list, int len) {
    int i,j;

    for(i = 0; i < len; i++) {
        for(j = i + 1; j < len; j++) {
            if((list[i]).combo < (list[j]).combo)
                swap_combo(&(list[i]),&(list[j]));
        }
    }
}

void set_mc_values(MinipokerCombo *mc, int card, int combo) {
    mc->card = card;
    mc->combo = combo;
}

int define_prize(MinipokerCombo *list,int sz) {
    if((list[0]).combo == 5) {
        return(200 + (list[0]).card);
    }
    if((list[0]).combo == 4) {
        return(180 + (list[0]).card);
    }
    if((list[0]).combo == 3) {
        if(sz > 1 && (list[1]).combo == 2) return(160 + (list[0]).card);
        else return(140 + (list[0]).card);
    }
    if((list[0]).combo == 2) {
        if(sz > 1 && (list[1]).combo == 2) {
            if((list[0]).card < (list[1]).card)
                swap(&((list[0]).card),&((list[1]).card));

            return(3 * (list[0]).card + 2 * (list[1]).card + 20);
        }
        else return((list[0]).card);
    }

    return(0);
}

short is_full_sequence(int *hand, int sz) {
    int st = hand[0] + 1, i = 0;

    for(i = 1; i < sz; i++, st++) {
        if(hand[i] != st) return(0);
    }
    return(1);
}

int minipoker(int *hand, int sz) {
    if(sz == 0) return(-1);

    bubblesort(hand, sz);
    hand[sz] = 0;

    MinipokerCombo mc_array[5];
    int mc_index = 0;
    int i = 0;

    if(is_full_sequence(hand,sz)) {
        set_mc_values(&(mc_array[mc_index]), hand[0], 5);
        mc_index++;
    }
    else {
        while(i < sz) {
            int card = hand[i];
            i++;

            int combo = sequence(card, 1, hand, &i);
            set_mc_values(&(mc_array[mc_index]), card, combo);
            mc_index++;
        }

        bubblesort_combo(mc_array, mc_index);
    }

    return(define_prize(mc_array, mc_index));
}

void generate_minipoker_hand(int *hand, int sz, char *str) {
    char *tok;
    int i;

    gets(str);
    tok = strtok(str," ");
    for(i = 0; i < sz; i++) {
        hand[i] = atoi(tok);
        tok = strtok(NULL, " ");
    }
}

int main() {
    char str[MAX_STR], *tok;
    int hand[HAND_SZ + 1];

    gets(str);
    tok = strtok(str," ");
    int i, instances = atoi(tok);
    for(i = 1; i <= instances; i++) {
        generate_minipoker_hand(hand, HAND_SZ, str);
        printf("Teste %d\n%d\n", i, minipoker(hand, HAND_SZ));
    }

    return 0;
}
