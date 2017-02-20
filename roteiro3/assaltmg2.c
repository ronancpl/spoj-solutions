//AUTHOR NOTE: this program tried to use PD approach algorithm to solve an EXPONENTIAL (NP-HARD) problem.
//As matter of fact, the optimal solution will not be found by the program.
//Unfeasible, as the memory space required increases incredibly fast. Lesson learned.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 100
#define MAX_DIR 20
#define NUM_KEYS 32

#define INF 1000000
#define INC_CACHE 100

typedef struct {
    short list_key[NUM_KEYS];
    int list_len;
} Director;

typedef struct {
    int *dlist;
    int dlen;

    short bitmask[NUM_KEYS];
} DirectorPath;

typedef struct {
    DirectorPath *list;
    int len;
} DirectorDP;

int get_covered_slots(short *bitmask) {
    int count = 0;

    int i;
    for(i = 0; i < NUM_KEYS; i++) {
        if(bitmask[i] > 0)
            count++;
    }

    return(count);
}

void clear_covered_slots(short *bitmask) {
    int i;
    for(i = 0; i < NUM_KEYS; i++)
        bitmask[i] = 0;
}

void reset_director_dp(DirectorDP *dp, int length, int klen) {
    int i, j;

    for(i = 0; i <= klen; i++) {
        for(j = 0; j < (length * length); j++) {
            dp[i].list[j].dlen = 0;
            clear_covered_slots(dp[i].list[j].bitmask);
        }

        dp[i].len = 0;
    }
}

DirectorDP* create_director_dp(int length, int klen) {
    DirectorDP *dp = (DirectorDP *)malloc((klen + 1) * sizeof(DirectorDP));

    int i, j;
    for(i = 0; i <= klen; i++) {
        dp[i].list = (DirectorPath *)malloc((length * length) * sizeof(DirectorPath));

        for(j = 0; j < length * length; j++) {
            dp[i].list[j].dlist = (int *)malloc((length) * sizeof(int));

            dp[i].list[j].dlen = 0;
            clear_covered_slots(dp[i].list[j].bitmask);
        }

        dp[i].len = 0;
    }

    return(dp);
}

void delete_director_dp(DirectorDP *dp, int length, int klen) {
    int i, j;
    for(i = 0; i <= klen; i++) {
        for(j = 0; j < (length * length); j++)
            free(((dp[i]).list[j].dlist));

        free((dp[i]).list);
    }

    free(dp);
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

void copy_bitmask_from_item(short *to, short *from) {
    int i;
    for(i = 0; i < NUM_KEYS; i++)
        to[i] = from[i];
}

short is_enough_keys(short *bitmask, int keys_needed) {
    int count = 0, i;

    for(i = 0; i < NUM_KEYS; i++) {
        if(bitmask[i] > 0)
            count++;
    }

    return(count >= keys_needed);
}

void color_bitmask_with_director(short *bitmask, Director *dir) {
    int i;
    for(i = 0; i < dir->list_len; i++)
        (bitmask[dir->list_key[i]])++;
}

short got_new_keys(short *original, short *processed) {
    int i;
    for(i = 0; i < NUM_KEYS; i++) {
        if(original[i] == 0 && processed[i] > 0) return(1);
    }

    return(0);
}

void Partition(int Esq,int Dir,int *i,int *j,int *A) {
    int x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2];
    do {
        while (x > A[*i]) (*i)++;
        while (x < A[*j]) (*j)--;

        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Quicksort(int Esq,int Dir,int *A) {
    int i,j;

    if(Esq == Dir) return;

    Partition(Esq, Dir, &i, &j, A);
    if (Esq < j) Quicksort(Esq, j, A);
    if (i < Dir) Quicksort(i, Dir, A);
}

void quicksort_path(int *path, int path_len) {
    //guaranteed vector with more than 1 element
    Quicksort(0, path_len - 1, path);
}

short is_unique_path(DirectorPath *path, int cursor) {
    int i, j;
    for(i = 0; i < cursor; i++) {
        for(j = 0; j < path[i].dlen; j++) {
            if(path[i].dlist[j] != path[cursor].dlist[j]) break;
        }

        if(j == path[i].dlen) return(0);
    }

    return(1);
}

short process_stage_dp(int stage, DirectorDP *dp, Director **dir, int dir_len) {
    int i, j, k;
    int last = stage - 1;

    for(i = 0; i < dp[last].len; i++) {
        //from the last stage, select those paths which fulfills required keys for this stage
        int cursor = dp[stage].len;

        copy_bitmask_from_item(dp[stage].list[cursor].bitmask, dp[last].list[i].bitmask);
        if(is_enough_keys(dp[stage].list[cursor].bitmask, stage)) {
            //include this entry to the this level path possibilities

            for(j = 0; j < dp[last].list[i].dlen; j++)
                dp[stage].list[cursor].dlist[j] = dp[last].list[i].dlist[j];

            dp[stage].list[cursor].dlen = dp[last].list[i].dlen;
            (dp[stage].len)++;
        }
    }

    if(dp[stage].len == 0) {
        //could not recycle any path from the last level, must now find new directors to fill the gap

        for(i = 0; i < dp[last].len; i++) {

            //test all directors for new possible paths
            for(j = 0; j < dir_len; j++) {
                int cursor = dp[stage].len;

                //exits early, allocated limit reached.
                if(cursor == MAX_DIR * MAX_DIR) return(dp[stage].len > 0);

                copy_bitmask_from_item(dp[stage].list[cursor].bitmask, dp[last].list[i].bitmask);
                color_bitmask_with_director(dp[stage].list[cursor].bitmask, dir[j]);

                if(got_new_keys(dp[last].list[i].bitmask, dp[stage].list[cursor].bitmask)) {
                    //guarantees unique directors on this path

                    for(k = 0; k < dp[last].list[i].dlen; k++)
                        dp[stage].list[cursor].dlist[k] = dp[last].list[i].dlist[k];

                    //add this director to possible path
                    dp[stage].list[cursor].dlist[k] = j;
                    dp[stage].list[cursor].dlen = dp[last].list[i].dlen + 1;

                    quicksort_path(dp[stage].list[cursor].dlist, dp[stage].list[cursor].dlen);
                    if(is_unique_path(dp[stage].list, dp[stage].len))
                        (dp[stage].len)++;
                }
            }
        }
    }

    return(dp[stage].len > 0);
}

void init_stage_dp(DirectorDP *dp, Director **dir, int dir_len) {
    int i;
    for(i = 0; i < dir_len; i++) {
        if(dir[i]->list_len > 0) {
            int cursor = dp[1].len;

            dp[1].list[cursor].dlist[0] = i;
            dp[1].list[cursor].dlen = 1;
            color_bitmask_with_director(dp[1].list[cursor].bitmask, dir[i]);

            (dp[1].len)++;
        }
    }
}

int main() {
    char str[MAX_STR], *tok;
    Director **dir;

    DirectorDP *dp = create_director_dp(MAX_DIR, NUM_KEYS);

    gets(str);
    tok = strtok(str, " ");
    int i, j, instances = atoi(tok);

    int min_deep;

    for(i = 0; i < instances; i++) {
        gets(str);
        tok = strtok(str, " ");
        int keys_needed = atoi(tok);

        tok = strtok(NULL, " ");
        int qty = atoi(tok);

        dir = create_directors(str, qty);
        reset_director_dp(dp, MAX_DIR, NUM_KEYS);

        if(keys_needed > 0) {
            init_stage_dp(dp, dir, qty);
            min_deep = INF;

            int stage = 2;
            while(stage <= keys_needed) {
                if(!process_stage_dp(stage, dp, dir, qty))
                    break;

                stage++;
            }

            if(stage > keys_needed) {
                //whatever, only the number of directors is needed
                min_deep = dp[keys_needed].list[0].dlen;

                //LOST CASE, recovered here
                if(keys_needed == 1 && min_deep == 0) min_deep = INF;
            }
        }
        else min_deep = 0;

        if(min_deep == INF) printf("Desastre!\n");
        else printf("%d\n", min_deep);

        delete_directors(dir, qty);
    }

    delete_director_dp(dp, MAX_DIR, NUM_KEYS);

    return 0;
}
