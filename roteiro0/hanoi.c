#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int conta_hanoi(int n) {
    if(n <= 0) return(0);
    int ret = (int)pow(2,n) - 1;

    return(ret);
}

int main() {
    int val,res;

    scanf("%d",&val);
    res = conta_hanoi(val);

    printf("%d",res);
    return 0;
}
