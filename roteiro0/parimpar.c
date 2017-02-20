#include <stdio.h>
#include <stdlib.h>

#define MAX_NAME 100

void parimpar() {
    int cont,i,teste = 0;
    char player1[MAX_NAME],player2[MAX_NAME];
    int val1,val2;

    while(1) {
        teste++;
        scanf("%d",&cont);

        if(cont == 0) return;
        scanf("%s",player1);
        scanf("%s",player2);

        printf("Teste %d\n",teste);
        for(i = 0; i < cont; i++) {
            scanf("%d",&val1);
            scanf("%d",&val2);

            if((val1 + val2) % 2 == 0) printf("%s\n",player1);
            else printf("%s\n",player2);
        }

        printf("\n");
    }
}

int main() {
    parimpar();
    return 0;
}
