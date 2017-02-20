#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TAM 1000

struct Stack {
    int *stack;
    int cursor;
};

void getEntry(int entry,int *V_in,int *V_en,int len,int *in,int *en) {
    *in = -1;
    *en = -1;

    int i;
    for(i = 0; i < len; i++) {
        if(V_in[i] == entry) {
            *in = i;
            break;
        }
    }

    for(i = 0; i < len; i++) {
        if(V_en[i] == entry) {
            *en = i;
            break;
        }
    }
}

void buildStack(struct Stack **stack,int tam) {
    (*stack)->stack = (int *)realloc((*stack)->stack, tam*sizeof(int));
    (*stack)->cursor = 0;

    char string[MAX_TAM];
    gets(string);

    char *tok;
    tok = strtok(string," \t\0");

    while(tok != NULL) {
        (*stack)->stack[(*stack)->cursor] = atoi(tok);
        (*stack)->cursor++;

        if((*stack)->cursor == tam) break;
        tok = strtok(NULL," \t\0");
    }
}

int popStack(struct Stack **stack) {
    if((*stack)->cursor == 0) return(-1);

    ((*stack)->cursor)--;
    return((*stack)->stack[(*stack)->cursor]);
}

int ResolveGrid() {
    int qte = 0;
    char string[MAX_TAM];
    gets(string);
    qte = atoi(string);
    if(qte == 0) return(0);

    struct Stack *stack = (struct Stack *)malloc(sizeof(struct Stack));
    stack->stack = (int *)malloc(10*sizeof(int));

    int acc = 0;
    int *start = (int *)calloc(qte,sizeof(int));
    int *ending = (int *)calloc(qte,sizeof(int));

    int i = 0, val;
    buildStack(&stack,qte);
    for(i = qte - 1; i >= 0; i--) {
        val = popStack(&stack);
        start[i] = val;
    }

    buildStack(&stack,qte);
    for(i = qte - 1; i >= 0; i--) {
        val = popStack(&stack);
        ending[i] = val;
        printf("%d ",val);
    }

    int pos_in,pos_en;
    for(i = 1; i <= qte; i++) {
        getEntry(i,start,ending,qte,&pos_in,&pos_en);

        if(pos_in > -1 && pos_en > -1) {
            if(pos_en < pos_in) {
                printf("%d :: %d -> %d\n",i,pos_in,pos_en);

                acc -= (pos_en - pos_in);
            }
        }
        else printf("eh?");
    }
    printf("%d\n",acc);

    free(start);
    free(ending);

    free(stack->stack);
    free(stack);

    return(1);
}


int main() {
    while(ResolveGrid() > 0) {} //resolve grid
    return(0);
}
