#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int value;
} StackItem;

typedef struct {
    StackItem *item;

    unsigned int cursor;
    unsigned int len;
} StackTAD;

StackTAD* stack_create(unsigned int len) {
    StackTAD *stack = (StackTAD *)malloc(sizeof(StackTAD));

    stack->item = (StackItem *)malloc(len * sizeof(StackItem));
    stack->cursor = 0;
    stack->len = len;

    return(stack);
}

void stack_destroy(StackTAD *stack) {
    free(stack->item);
    free(stack);
}

short stack_push(StackTAD *stack, StackItem item) {
    if(stack->cursor == stack->len) return(0);

    stack->item[stack->cursor] = item;
    (stack->cursor)++;
    return(1);
}

short stack_pop(StackTAD *stack, StackItem *item) {
    if(stack->cursor == 0) return(0);

    (stack->cursor)--;
    *item = stack->item[stack->cursor];
    return(1);
}

short stack_isempty(StackTAD *stack) {
    return(stack->cursor == 0);
}

StackItem create_item(int val) {
    StackItem v;
    v.value = val;

    return(v);
}

int main() {
    StackTAD *stack = stack_create(5);
    stack_push(stack, create_item(777));

    StackItem si;
    stack_pop(stack, &si);

    printf("%d", si.value);
    return 0;
}
