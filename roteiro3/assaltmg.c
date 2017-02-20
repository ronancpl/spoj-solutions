#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_KEYS 32
#define MAX_STR 100
#define INF 1000000

typedef struct {
    short list_key[NUM_KEYS];
    int list_len;

    short used;
} Director;

void clear_covered_slots(short *bitmask) {
    int i;
    for(i = 0; i < NUM_KEYS; i++)
        bitmask[i] = 0;
}

int get_covered_slots(short *bitmask) {
    int count = 0;

    int i;
    for(i = 0; i < NUM_KEYS; i++) {
        if(bitmask[i] > 0)
            count++;
    }

    return(count);
}

void color_slots(short mode, short *list, int len, short *bitmask) {
    int i;
    for(i = 0; i < len; i++) {
        bitmask[list[i]] += mode;
    }
}

void get_min_directors(int *min_deep, int deep, short *bitmask, int slot, Director **dir, int dir_len, int keys_needed) {
    //COLOR DIRECTOR
    color_slots(1, (dir[slot])->list_key, (dir[slot])->list_len, bitmask);
    (dir[slot])->used = 1;

    if(get_covered_slots(bitmask) < keys_needed) {
        int i;
        for(i = slot + 1; i < dir_len; i++) {
            get_min_directors(min_deep, deep + 1, bitmask, i, dir, dir_len, keys_needed);
        }
    }
    else {
        //candidate to solution
        if(*min_deep > deep) *min_deep = deep;
    }

    //UNCOLOR DIRECTOR
    color_slots(-1, (dir[slot])->list_key, (dir[slot])->list_len, bitmask);
    (dir[slot])->used = 0;
}

Director** create_directors(char *str, int qty) {
    char *tok;
    int i, j;

    Director **dir = (Director **)malloc(qty * sizeof(Director *));

    for(i = 0; i < qty; i++) {
        dir[i] = (Director *)malloc(sizeof(Director));

        gets(str);
        tok = strtok(str, " ");
        int len = atoi(tok);

        tok = strtok(NULL, " ");
        for(j = 0; j < len; j++) {
            (dir[i])->list_key[j] = atoi(tok);
            tok = strtok(NULL, " ");
        }

        (dir[i])->used = 0;
        (dir[i])->list_len = len;
    }

    return(dir);
}

void delete_directors(Director **dir, int qty) {
    int i;
    for(i = 0; i < qty; i++)
        free(dir[i]);

    free(dir);
}

int main() {
    char str[MAX_STR], *tok;
    Director **dir;

    gets(str);
    tok = strtok(str, " ");
    int i, j, instances = atoi(tok);

    int min_deep;
    short bitmask[NUM_KEYS];
    clear_covered_slots(bitmask);

    for(i = 0; i < instances; i++) {
        gets(str);
        tok = strtok(str, " ");
        int keys_needed = atoi(tok);

        tok = strtok(NULL, " ");
        int qty = atoi(tok);

        dir = create_directors(str, qty);

        if(keys_needed > 0) {
            min_deep = INF;

            for(j = 0; j < qty; j++)
                get_min_directors(&min_deep, 1, bitmask, j, dir, qty, keys_needed);
        }
        else min_deep = 0;

        if(min_deep == INF) printf("Desastre!\n");
        else printf("%d\n", min_deep);

        delete_directors(dir, qty);
    }

    return 0;
}
