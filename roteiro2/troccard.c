#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 60000
#define INF 1000000

typedef struct {
    int *cards;
    int *ucards;    //list of unique cards

    int len;
    int ulen;
} CardList;

void Partition(int Esq,int Dir,int *i,int *j,int *A) {
    int x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2];
    do {
        while (x > A[*i]) (*i)++;
        while (x < A[*j]) (*j)--;

        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Quicksort(int Esq,int Dir,int *A) {
    int i,j;

    if(Esq == Dir) return;

    Partition(Esq, Dir, &i, &j, A);
    if (Esq < j) Quicksort(Esq, j, A);
    if (i < Dir) Quicksort(i, Dir, A);
}

void sort_cardlist(CardList *cl) {
    Quicksort(0, cl->len - 1, cl->cards);
}

void generate_unique_list(CardList *cl) {
    int i, j;
    cl->ucards[0] = cl->cards[0];
    j = 1;
    for(i = 1; i < cl->len; i++) {
        if(cl->cards[i - 1] != cl->cards[i]) {
            cl->ucards[j] = cl->cards[i];
            j++;
        }
    }

    cl->ucards[j] = INF;
    cl->ulen = j;
}

void add_unique_to_list(int *list, int *len, CardList *cl, int *it) {
    list[*len] = cl->ucards[*it];

    (*it)++;
    (*len)++;
}

void pass_unique(int *it) {
    (*it)++;
}

short is_repeated(int *list, int slot, int val) {
    return(val == list[slot - 1]);
}

void generate_global_list(CardList *cl1, CardList *cl2, int **l, int *len) {
    int *list = (int *)malloc((cl1->ulen + cl2->ulen) * sizeof(int));

    int it1 = 0, it2 = 0;
    int it = 0;

    if(cl1->ucards[it1] < cl2->ucards[it2]) {
        add_unique_to_list(list, &it, cl1, &it1);
    }
    else {
        add_unique_to_list(list, &it, cl2, &it2);
    }

    while(it1 < cl1->ulen || it2 < cl2->ulen) {
        if(cl1->ucards[it1] < cl2->ucards[it2]) {
            if(!is_repeated(list, it, cl1->ucards[it1])) {
                add_unique_to_list(list, &it, cl1, &it1);
            }
            else {
                pass_unique(&it1);
            }
        }
        else {
            if(!is_repeated(list, it, cl2->ucards[it2])) {
                add_unique_to_list(list, &it, cl2, &it2);
            }
            else {
                pass_unique(&it2);
            }
        }
    }

    *l = list;
    *len = it;
}

void destroy_global_list(int *list) {
    free(list);
}

int get_remaining(CardList *cl, int glen) {
    return(glen - cl->ulen);
}

int card_change(CardList *cl1, CardList *cl2) {
    if(cl1->len == 0 || cl2->len == 0) return(0);

    sort_cardlist(cl1);
    sort_cardlist(cl2);

    generate_unique_list(cl1);
    generate_unique_list(cl2);

    int *glist;
    int glen;
    generate_global_list(cl1, cl2, &glist, &glen);

    int w1 = get_remaining(cl1, glen);
    int w2 = get_remaining(cl2, glen);

    destroy_global_list(glist);
    return((w1 < w2) ? w1 : w2);
}

CardList* get_card_list(char *str, int len) {
    char *tok;
    CardList *cl = (CardList *)malloc(sizeof(CardList));

    cl->ucards = (int *)malloc((len + 1) * sizeof(int));
    cl->cards = (int *)malloc((len + 1) * sizeof(int));

    gets(str);
    tok = strtok(str," ");

    int i;
    for(i = 0; i < len; i++) {
        cl->cards[i] = atoi(tok);
        tok = strtok(NULL," ");
    }

    cl->len = len;
    return(cl);
}

void free_card_list(CardList *cl) {
    free(cl->cards);
    free(cl->ucards);

    free(cl);
}

int main() {
    char str[MAX_STR], *tok;

    int v1, v2;
    CardList *cl1, *cl2;
    while(1) {
        gets(str);

        tok = strtok(str," ");
        v1 = atoi(tok);

        tok = strtok(NULL," ");
        v2 = atoi(tok);

        if(v1 == 0 && v2 == 0) break;

        cl1 = get_card_list(str, v1);
        cl2 = get_card_list(str, v2);

        printf("%d\n", card_change(cl1, cl2));

        free_card_list(cl1);
        free_card_list(cl2);
    }

    return 0;
}
