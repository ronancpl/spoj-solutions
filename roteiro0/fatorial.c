#include <stdio.h>
#include <stdlib.h>

int fat(int n) {
    if(n == 1) return(1);
    return(n * fat(n-1));
}

int fatorial(int n) {
    if(n <= 0) return(0);
    return(fat(n));
}

int main() {
    int val,res;

    scanf("%d",&val);
    res = fat(val);

    printf("%d",res);
    return 0;
}
