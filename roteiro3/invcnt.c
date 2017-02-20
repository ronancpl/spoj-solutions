#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 10

typedef struct {
    int *list;

    int cursor;
    int limit;
} List;

void make_sorted_list(List** list, int limit) {
    List *l = (List *)malloc(sizeof(List));
    l->list = (int *)malloc(limit * sizeof(int));

    l->cursor = 0;
    l->limit = limit;

    *list = l;
}

void destroy_sorted_list(List* list) {
    free(list->list);
    free(list);
}

void push_to_sorted_list(List *list, int val) {
    if(list->cursor < list->limit) {
        list->list[list->cursor] = val;
        (list->cursor)++;
    }
}

void reset_sorted_list(List *sorted, int *list, int st) {
    int i;

    for(i = 0; i < sorted->cursor; i++)
        list[st + i] = sorted->list[i];

    sorted->cursor = 0;
}

void mergesort(long long int *invcnt, List *sorted, int *list, int st, int en) {
    if(st + 2 > en) return;

    int div = (st + en) / 2;
    mergesort(invcnt, sorted, list, st, div);
    mergesort(invcnt, sorted, list, div, en);

    int v1_cursor = st, v2_cursor = div;
    while(v1_cursor < div || v2_cursor < en) {
        if(v1_cursor == div) {
            //v1 empty, pushing from v2

            push_to_sorted_list(sorted, list[v2_cursor]);
            v2_cursor++;
        }
        else if(v2_cursor == en) {
            //v2 empty, pushing from v1

            push_to_sorted_list(sorted, list[v1_cursor]);
            v1_cursor++;

            //*invcnt += (div - v1_cursor);
        }
        else {
            if(list[v1_cursor] <= list[v2_cursor]) {
                push_to_sorted_list(sorted, list[v1_cursor]);
                v1_cursor++;
            }
            else {
                push_to_sorted_list(sorted, list[v2_cursor]);
                v2_cursor++;

                *invcnt += (div - v1_cursor);
            }
        }
    }

    //makes section on original list sorted, for use on next iterations of the mergesort
    reset_sorted_list(sorted, list, st);
}

long long int get_inversion_count(int *list, int len) {
    //using mergesort-like algorithm
    long long int invcnt = 0;
    List *cache;

    make_sorted_list(&cache, len);
    mergesort(&invcnt, cache, list, 0, len);
    destroy_sorted_list(cache);

    //value can get overflow at 32-bits
    return(invcnt);
}

int* get_list(char *str, int len) {
    char *tok;
    int *list;
    int i;

    list = (int *)malloc(len * sizeof(int));
    for(i = 0; i < len; i++) {
        gets(str);

        //not really needed, just to normalize certain situations
        tok = strtok(str, " ");

        list[i] = atoi(tok);
    }

    return(list);
}

void free_list(int *list) {
    free(list);
}

int main() {
    char str[MAX_STR], *tok;

    gets(str);
    tok = strtok(str, " ");
    int i, instances = atoi(tok);

    for(i = 0; i < instances; i++) {
        gets(str);
        gets(str);
        tok = strtok(str, " ");

        int len = atoi(tok);
        int *list = get_list(str, len);

        printf("%lld\n", get_inversion_count(list, len));

        free_list(list);
    }

    return 0;
}
