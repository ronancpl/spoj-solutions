#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 1100000
#define SLOTS_LEN 100000
#define INF 2000000000

int Min(int v1, int v2) {
    return((v1 < v2) ? v1 : v2);
}

int swaps_needed(int req, int st, int *list, int len) {
    int out, act, next, acc = 0;
    act = list[st];

    int i;
    for(i = st+1; i < len; i++) {
        next = list[i];

        if(next - list[st] >= req) {
            acc += (list[st] + req - act - 1);
            return(acc);
        }

        out = (next - act - 1);
        acc += out;

        act = next;
    }

    return(acc);
}

int calc_number_of_swaps(int *list, int len, int req, int limit) {
    int min_swaps = INF;
    int i = 0;

    //limit: last slot available + 1
    while(i < len && list[i] + req <= limit) {
        min_swaps = Min(min_swaps, swaps_needed(req, i, list, len));
        i++;
    }

    return(min_swaps);
}

void get_ready_vector(char *str, int ready, int **list, int *limit) {
    char *tok;

    gets(str);
    tok = strtok(str, " ");

    *limit = 0;
    if(ready == 0) return;

    int i;
    for(i = 0; i < ready; i++) {
        (*list)[i] = atoi(tok);
        tok = strtok(NULL, " ");
    }

    *limit = (*list)[i - 1] + 1;
}

int main() {
    char str[MAX_STR], *tok;

    int *list, limit;
    int ready, req;

    list = (int *)malloc(SLOTS_LEN * sizeof(int));

    while(1) {
        gets(str);
        tok = strtok(str, " ");
        req = atoi(tok);

        tok = strtok(NULL, " ");
        ready = atoi(tok);

        if(req == 0 && ready == 0) break;

        get_ready_vector(str, ready, &list, &limit);
        printf("%d\n", calc_number_of_swaps(list, ready, req, limit));
    }

    free(list);

    return 0;
}
