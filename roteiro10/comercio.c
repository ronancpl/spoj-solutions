#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM 25
#define MAX_STR 2000

typedef struct {
    long long num[MAX_NUM];
    short op[MAX_NUM - 1];

    short tam_num;
    short tam_op;
} ObjetoComercio;

short Op(char ch) {
    return(ch == '+');
}

short ehOp(char ch) {
    return(ch == '*' || ch == '+' || ch == '\0');
}

long long ExtraiNum(char *str,short *i) {
    char temp[MAX_STR];
    short val = 0;

    while(!ehOp(str[*i])) {
        temp[val] = str[*i];
        val++;
        (*i)++;
    }
    temp[val] = 0;

    return(atoll(temp));
}

ObjetoComercio GeraObjetoInstancia() {
    ObjetoComercio oc;
    char str[MAX_STR],*tok;

    gets(str);
    tok = strtok(str," ");
    short i = 0;

    short op = 0;
    while(1) {
        if(tok[i] == '\0') break;

        if(op % 2 == 0) oc.num[op / 2] = ExtraiNum(tok,&i);
        else {
            oc.op[op / 2] = Op(tok[i]);
            i++;
        }

        op++;
    }

    oc.tam_num = (op / 2) + 1;
    oc.tam_op  = (op / 2);

    return(oc);
}

ObjetoComercio GeraCopia(ObjetoComercio oc) {
    ObjetoComercio copia;

    copia.tam_num = oc.tam_num;
    copia.tam_op = oc.tam_op;

    short i;
    for(i = 0; i < oc.tam_num; i++) copia.num[i] = oc.num[i];
    for(i = 0; i < oc.tam_op; i++) copia.op[i] = oc.op[i];

    return(copia);
}

void ExtraiIndice_short(short *vetor,short *tam,short slot) {
    short i;
    for(i = slot; i < *tam - 1; i++) {
        vetor[i] = vetor[i + 1];
    }
    (*tam)--;
}

void ExtraiIndice_int(long long *vetor,short *tam,short slot) {
    short i;
    for(i = slot; i < *tam - 1; i++) {
        vetor[i] = vetor[i + 1];
    }
    (*tam)--;
}

long long RealizaOperacao(long long val1,long long val2, short op) {
    if(op == Op('+')) return(val1 + val2);
    else return(val1 * val2);
}

void EfetuaPreferencia(ObjetoComercio *oc,short op) {
    short i = 0;
    while(i < oc->tam_op) {
        if(oc->op[i] == op) {
            oc->num[i] = RealizaOperacao(oc->num[i],oc->num[i + 1],oc->op[i]);

            ExtraiIndice_int(oc->num,&(oc->tam_num),i + 1);
            ExtraiIndice_short(oc->op,&(oc->tam_op),i);

            continue;
        }

        i++;
    }
}

void EfetuaInstancia() {
    ObjetoComercio oc_min = GeraObjetoInstancia();
    ObjetoComercio oc_max = GeraCopia(oc_min);

    EfetuaPreferencia(&oc_max,Op('+'));
    EfetuaPreferencia(&oc_max,Op('*'));

    EfetuaPreferencia(&oc_min,Op('*'));
    EfetuaPreferencia(&oc_min,Op('+'));

    printf("The maximum and minimum are %lld and %lld.\n",oc_max.num[0],oc_min.num[0]);
}

int main() {
    char str[MAX_STR],*tok;

    gets(str);
    tok = strtok(str," ");

    short i;
    for(i = 0; i < atoi(tok); i++) {
        EfetuaInstancia();
    }

    return 0;
}
