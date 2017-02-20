#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BILLS 100
#define MAX_STR 10

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

void bits_sort_bills(int *cash_bill, int cash_len) {
    Quicksort(0, cash_len - 1, cash_bill);
}

void include_cash_bill(int value, int *cash_bill, int *cash_len) {
    if(value > 0 && *cash_len < MAX_BILLS) {
        cash_bill[*cash_len] = value;
        (*cash_len)++;
    }
}

void generate_cash_bills(int *cash_bill, int *cash_len) {
    include_cash_bill(50, cash_bill, cash_len);
    include_cash_bill(10, cash_bill, cash_len);
    include_cash_bill( 5, cash_bill, cash_len);
    include_cash_bill( 1, cash_bill, cash_len);

    bits_sort_bills(cash_bill, *cash_len);
}

void bits_init_system(int *cash_emit, int cash_len) {
    int i;
    for(i = 0; i < cash_len; i++)
        cash_emit[i] = 0;
}

void bits_cash_system(int requested, int *cash_bill, int *cash_emit, int cash_len) {
    int i = 0;

    while(i < cash_len) {
        if(requested < cash_bill[i]) {
            i++;
            continue;
        }

        requested -= cash_bill[i];
        (cash_emit[i])++;
    }
}

void bits_print_emit(int *cash_emit, int cash_len) {
    int i;
    for(i = 0; i < cash_len; i++)
        printf("%d ", cash_emit[i]);
}

int main() {
    int cash_bill[MAX_BILLS], cash_emit[MAX_BILLS];
    int cash_len = 0, instance = 1;

    generate_cash_bills(cash_bill, &cash_len);

    char str[MAX_STR], *tok;

    while(1) {
        gets(str);
        tok = strtok(str, " ");

        int request = atoi(tok);
        if(request == 0) break;

        printf("Teste %d\n", instance);
        bits_init_system(cash_emit, cash_len);
        bits_cash_system(request, cash_bill, cash_emit, cash_len);
        bits_print_emit(cash_emit, cash_len);
        printf("\n\n");

        instance++;
    }

    return 0;
}
