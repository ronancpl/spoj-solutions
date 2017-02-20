#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BETS 10000

int Max(int x, int y) {
    return((x > y) ? x : y);
}

int generate_max_subarray(int *bets, int bets_len) {
    int max_val = 0, max_yet = 0;
    int i;

    for(i = 0; i < bets_len; i++) {
        max_yet = Max(max_yet + bets[i], 0);
        max_val = Max(max_val, max_yet);
    }

    return(max_val);
}

int main() {
    int i, len;
    int bets[MAX_BETS];

    scanf("%d", &len);
    if(len > 0) {
        while(1) {
            for(i = 0; i < len; i++)
                scanf("%d", &(bets[i]));

            int best_streak = generate_max_subarray(bets, len);

            if(best_streak > 0) printf("The maximum winning streak is %d.", best_streak);
            else printf("Losing streak.");

            scanf("%d", &len);
            if(len <= 0) break;

            printf("\n");
        }
    }

    return 0;
}
