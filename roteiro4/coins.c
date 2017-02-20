#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 100000
#define MAX_BILLS 200

int retrieve_integer() {
    int val;
    scanf("%d", &val);

    return(val);
}

//decreasing order quicksort
void Partition(int Esq,int Dir,int *i,int *j,int *A) {
    int x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2];
    do {
        while (x < A[*i]) (*i)++;
        while (x > A[*j]) (*j)--;

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

void coins_sort_bills(int *cash_bill, int cash_len) {
    Quicksort(0, cash_len - 1, cash_bill);
}

void include_cash_bill(int value, int *cash_bill, int *cash_len) {
    if(value > 0 && *cash_len < MAX_BILLS) {
        cash_bill[*cash_len] = value;
        (*cash_len)++;
    }
}

void generate_cash_bills(int *cash_bill, int *cash_len) {
    *cash_len = 0;

    int qty = retrieve_integer(), i;
    for(i = 0; i < qty; i++) {
        int bill_val = retrieve_integer();
        include_cash_bill(bill_val, cash_bill, cash_len);
    }

    coins_sort_bills(cash_bill, *cash_len);
}

void coins_init_cash_emit_count(int *cash_emit_count, int cash_emit_len) {
    int i;
    for(i = 0; i < cash_emit_len; i++)
        cash_emit_count[i] = INF;
}

void coins_init_system(int *cash_emit_count, int **cash_emit, int cash_emit_len, int cash_len) {
    int i, j;

    for(i = 0; i < cash_emit_len; i++) {
        for(j = 0; j < cash_len; j++)
            cash_emit[i][j] = 0;
    }

    coins_init_cash_emit_count(cash_emit_count, cash_emit_len);
    cash_emit_count[0] = 0;
}

void coins_copy_cash_emit(int to, int from, int **cash_emit, int cash_len) {
    int i;
    for(i = 0; i < cash_len; i++)
        cash_emit[to][i] = cash_emit[from][i];
}

void propagate_required_cash(int req, int *cash_bill, int **cash_emit, int cash_len, int *cash_emit_count, int cash_emit_len) {
    int i;
    for(i = 0; i < cash_len; i++) {
        if((req + cash_bill[i]) < cash_emit_len && (cash_emit_count[req] + 1) < cash_emit_count[req + cash_bill[i]]) {
            coins_copy_cash_emit(req + cash_bill[i], req, cash_emit, cash_len);
            (cash_emit[req + cash_bill[i]][i]) += 1;

            cash_emit_count[req + cash_bill[i]] = cash_emit_count[req] + 1;
        }
    }
}

void coins_init_structures(int ***ce, int **cec, int cash_emit_len, int cash_len) {
    int *cash_emit_count = (int *)malloc(cash_emit_len * sizeof(int));
    int **cash_emit = (int **)malloc(cash_emit_len * sizeof(int *));

    int i;
    for(i = 0; i < cash_emit_len; i++)
        cash_emit[i] = (int *)malloc(cash_len * sizeof(int));

    *cec = cash_emit_count;
    *ce = cash_emit;
}

void coins_destroy_structures(int **cash_emit, int *cash_emit_count, int cash_emit_len) {
    int i;
    for(i = 0; i < cash_emit_len; i++)
        free(cash_emit[i]);
    free(cash_emit);

    free(cash_emit_count);
}

int coins_cash_system(int requested, int *cash_bill, int cash_len) {
    if(cash_len == 0) return(INF);

    int **cash_emit, *cash_emit_count, cash_emit_len = requested + 1;
    coins_init_structures(&cash_emit, &cash_emit_count, cash_emit_len, cash_len);
    coins_init_system(cash_emit_count, cash_emit, cash_emit_len, cash_len);

    int i;
    for(i = 0; i < requested; i++)
        propagate_required_cash(i, cash_bill, cash_emit, cash_len, cash_emit_count, cash_emit_len);

    int coins = cash_emit_count[requested];
    coins_destroy_structures(cash_emit, cash_emit_count, cash_emit_len);

    return(coins);
}

int main() {
    int *cash_bill;
    int cash_len;

    cash_bill = (int *)malloc(MAX_BILLS * sizeof(int));

    while(1) {
        int request = retrieve_integer();
        if(request <= 0) break;

        generate_cash_bills(cash_bill, &cash_len);

        int coins = coins_cash_system(request, cash_bill, cash_len);

        if(coins != INF) printf("%d\n", coins);
        else printf("Impossivel\n");
    }

    free(cash_bill);
    return 0;
}
