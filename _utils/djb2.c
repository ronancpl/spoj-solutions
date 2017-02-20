#include <stdio.h>
#include <string.h>
#include <stdint.h>

unsigned long jdb2(unsigned char *str) {
    // src: http://www.cse.yorku.ca/~oz/hash.html

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/*
    JDB2 (STRING HASH):

    unsigned long jdb2(unsigned char *str);

*/

int main() {
    unsigned char *str = "Ronan C. P. Lana";
    printf("%x", jdb2(str));

    return 0;
}
