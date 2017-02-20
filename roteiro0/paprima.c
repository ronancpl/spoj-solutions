#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIM_SIEVE 10000000
#define STR_MAX 21

void take_multiples(short *sieve, int mult) {
    int acc = 2 * mult;

    for(; acc < LIM_SIEVE; acc += mult) {
        sieve[acc] = 0;
    }
}

short *get_sieve() {
    int i;
    short *sieve = (short *)malloc(LIM_SIEVE * sizeof(short));

    for(i = 0; i < LIM_SIEVE; i++) sieve[i] = 1;

    for(i = 2; i < LIM_SIEVE; i++) {
        if(sieve[i] == 1) {
            take_multiples(sieve, i);
        }
    }

    return(sieve);
}

void free_sieve(short *sieve) {
    free(sieve);
}

int get_letter_value(char ch) {
    if(ch >= 'a' && ch <= 'z') return(ch - 'a' + 1);
    else return(ch - 'A' + 27);
}

short is_word_prime(char *str,short *sieve) {
    int i;
    int acc = 0;

    for(i = 0; i < strlen(str); i++)
        acc += get_letter_value(str[i]);

    return(acc < LIM_SIEVE && sieve[acc]);
}

short get_word(char *str) {
    char ch;
    short i = 0;

    while(1) {
        ch = getchar();

        if(ch == '\n') {
            str[i] = 0;
            return(0);
        }
        else if(ch == EOF) {
            str[i] = 0;
            return(1);
        }

        str[i] = ch;
        i++;
    }
}

int main(int argc, char **argv) {
    short *sieve = get_sieve(), st;
    char word[STR_MAX];

    do {
        st = get_word(word);

        //not empty string
        if(word[0] != 0) {
            if(is_word_prime(word,sieve)) printf("It is a prime word.\n");
            else printf("It is not a prime word.\n");
        }
    } while(st == 0);

    free(sieve);
    return 0;
}
