#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 20

typedef struct {
    int value;
    int weight;
} Item;

int Max(int x, int y) {
    return((x > y) ? x : y);
}

int **create_knapsack(int items, int weight) {
    int **knapsack = (int **)malloc(items * sizeof(int *));

    int i;
    for(i = 0; i < items; i++)
        knapsack[i] = (int *)malloc(weight * sizeof(int));

    //initialize first row of knapsack system
    for(i = 0; i < weight; i++)
        knapsack[0][i] = 0;

    return(knapsack);
}

void destroy_knapsack(int **knapsack, int items) {
    int i;
    for(i = 0; i < items; i++)
        free(knapsack[i]);
    free(knapsack);
}

int execute_0_1_knapsack(int weight, Item *list, int len) {
    int **knapsack = create_knapsack(len, weight);

    int i, j;
    for(i = 1; i < len; i++) {
        for(j = 0; j < weight; j++) {
            if(list[i].weight > j) {
                knapsack[i][j] = knapsack[i - 1][j];
            }
            else {
                knapsack[i][j] = Max(knapsack[i - 1][j], knapsack[i - 1][j - list[i].weight] + list[i].value);
            }
        }
    }

    int max_value = knapsack[len - 1][weight - 1];
    destroy_knapsack(knapsack, len);

    return(max_value);
}

Item *generate_item_list(char *str, int len) {
    int i;
    char *tok;

    Item *list = (Item *)malloc((len + 1) * sizeof(Item));
    for(i = 1; i <= len; i++) {
        gets(str);

        tok = strtok(str, " ");
        list[i].weight = atoi(tok);

        tok = strtok(NULL, " ");
        list[i].value = atoi(tok);
    }

    return(list);
}

void destroy_item_list(Item *list) {
    free(list);
}

int main() {
    char str[MAX_STR], *tok;
    int instances = 1;

    while(1) {
        gets(str);
        tok = strtok(str, " ");
        int capacity = atoi(tok);

        tok = strtok(NULL, " ");
        int len = atoi(tok);

        if(capacity <= 0 || len <= 0) break;

        Item *list = generate_item_list(str, len);

        //list: starts item informations from index 1
        //capacity: must be provided after adding 1, for the real capacity to be computed
        printf("Teste %d\n%d\n\n", instances, execute_0_1_knapsack(capacity + 1, list, len + 1));

        destroy_item_list(list);
        instances++;
    }

    return 0;
}
