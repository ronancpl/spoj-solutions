#include <stdio.h>
#include <stdlib.h>

// DUMMY program: for any given number (ACC), and a given (PRIME) hash factor, how much times you need
// to accumulate the counter to ACCESS once every entry of the table?

#define PRIME 5381
#define ACC 7

short all_done(short *list) {
    short i;
    for(i = 0; i < PRIME; i++)
        if(list[i] == 0) return 0;

    return 1;
}

int main() {
    int count = 0;
    short *list = (short *)calloc(PRIME, sizeof(short));

    while(!all_done(list)) {
        list[(count * ACC) % PRIME] = 1;
        count++;
    }

    free(list);

    printf("%d for prime %d\n", count, PRIME);
    return 0;
}
