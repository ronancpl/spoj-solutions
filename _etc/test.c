#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int key;
    int value;
} Data;

void Change(Data *list) {
    list[0].key = 5;
    list[0].value = 2;
}

int main() {
    Data *list = (Data *)malloc(5*sizeof(Data));

    list[0].key = 4;
    list[0].value = 3;

    Change(list);

    printf("k:%d v:%d",list[0].key,list[0].value);

    free(list);

    return 0;
}
