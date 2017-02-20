#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 1000
#define HOUR2MN 60

typedef struct {
    int hr;
    int mn;
} AlarmTimer;

int get_time_left(AlarmTimer *now, AlarmTimer *lim) {
    int now_time = now->hr * HOUR2MN + now->mn;
    int lim_time = lim->hr * HOUR2MN + lim->mn;

    int time_left = lim_time - now_time;

    //next day recovery maneuver
    if(time_left < 0) time_left += 24 * HOUR2MN;

    return(time_left);
}

void set_timer(AlarmTimer *at, int hr, int mn) {
    at->hr = hr;
    at->mn = mn;
}

short zero_timer(AlarmTimer *at) {
    return(at->hr == 0 && at->mn == 0);
}

int main() {
    char str[MAX_STR], *tok;
    AlarmTimer now, lim;

    int hr, mn;

    while(1) {
        gets(str);

        tok = strtok(str, " ");
        hr = atoi(tok);
        tok = strtok(NULL, " ");
        mn = atoi(tok);

        set_timer(&now, hr, mn);

        tok = strtok(NULL, " ");
        hr = atoi(tok);
        tok = strtok(NULL, " ");
        mn = atoi(tok);

        set_timer(&lim, hr, mn);

        if(zero_timer(&now) && zero_timer(&lim)) break;

        printf("%d\n", get_time_left(&now, &lim));
    }

    return 0;
}
