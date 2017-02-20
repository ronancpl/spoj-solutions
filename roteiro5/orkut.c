#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------- DJB2 v1.0 -------------------------------

unsigned long jdb2(unsigned char *str) {
    // src: http://www.cse.yorku.ca/~oz/hash.html

    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// -------------------------------------------------------------

// ---------------------- HASHSET v1.0 -------------------------

//NOTE: should the HASH_MAXITEM or HASH_NUMBUCK value be too small, program will crash by SIG_SEGV
#define HASH_MAXITEM 200
#define HASH_NUMBUCK 135
#define HASH_HIVALUE 2147483647     //32-BIT integer

#define HASH_REHTHRE 0.75
#define HASH_REHRATE 2

typedef struct {
    int list[HASH_MAXITEM];
    int first;

    unsigned int count;
} HastSetIndex;

typedef struct {
    HastSetIndex **table;

    unsigned int threshold;
    unsigned int length;
    unsigned int count;
} HashSet;

void hashset_create_table(HashSet *hs) {
    hs->table = (HastSetIndex **)malloc(hs->length * sizeof(HastSetIndex *));
    hs->threshold = (unsigned int)(HASH_REHTHRE * hs->length);

    unsigned int i;
    for(i = 0; i < hs->length; i++) {
        hs->table[i] = (HastSetIndex *)malloc(sizeof(HastSetIndex));
        hs->table[i]->count = 0;
        hs->table[i]->first = HASH_HIVALUE;
    }
}

HashSet* hashset_create() {
    HashSet *hs = (HashSet *)malloc(sizeof(HashSet));
    hs->count = 0;
    hs->length = HASH_NUMBUCK;

    hashset_create_table(hs);
    return(hs);
}

void hashset_destroy(HashSet *hs) {
    unsigned int i;
    for(i = 0; i < hs->length; i++)
        free(hs->table[i]);

    free(hs->table);
    free(hs);
}

unsigned int hashset_maptable(HashSet *hs, int item) {
    return(item % hs->length);
}

unsigned int hashset_slot(HashSet *hs, int item, unsigned int *bucket) {
    *bucket = hashset_maptable(hs, item);

    unsigned int i;
    for(i = 0; i < hs->table[*bucket]->count; i++) {
        if(hs->table[*bucket]->list[i] == item)
            return(i);
    }

    return(-1);
}

short hashset_contains(HashSet *hs, int item, unsigned int *bucket) {
    return(hashset_slot(hs, item, bucket) != -1);
}

short hashset_insertinto(HashSet *hs, int item) {
    unsigned int bucket;

    if(!hashset_contains(hs, item, &bucket)) {
        if(hs->table[bucket]->first > item)
            hs->table[bucket]->first = item;

        hs->table[bucket]->list[hs->table[bucket]->count] = item;

        (hs->count)++;
        (hs->table[bucket]->count)++;
        if(hs->table[bucket]->count > hs->threshold) return(1);
    }

    return(0);
}

void hashset_rehash(HashSet *hs) {
    int *temp = (int *)malloc(hs->count * sizeof(int));
    unsigned int temp_cursor = 0, i, j;

    for(i = 0; i < hs->length; i++) {
        for(j = 0; j < hs->table[i]->count; j++) {
            temp[temp_cursor] = hs->table[i]->list[j];
            temp_cursor++;
        }
    }

    for(i = 0; i < hs->length; i++)
        free(hs->table[i]);
    free(hs->table);

    hs->count = 0;
    hs->length *= HASH_REHRATE;
    hashset_create_table(hs);

    for(i = 0; i < temp_cursor; i++)
        hashset_insertinto(hs, temp[i]);

    free(temp);
}

void hashset_insert(HashSet *hs, int item) {
    if(hashset_insertinto(hs, item)) {
        hashset_rehash(hs);
    }
}

int hashset_recalc_first(HashSet *hs, int bucket) {
    int i, val = HASH_HIVALUE;
    for(i = 0; i < hs->table[bucket]->count; i++) {
        if(val > hs->table[bucket]->list[i])
            val = hs->table[bucket]->list[i];
    }

    return(val);
}

void hashset_remove(HashSet *hs, int item) {
    unsigned int bucket;
    unsigned int slot = hashset_slot(hs, item, &bucket);

    if(slot != -1) {
        (hs->count)--;
        (hs->table[bucket]->count)--;
        hs->table[bucket]->list[slot] = hs->table[bucket]->list[hs->table[bucket]->count];

        if(item == hs->table[bucket]->first)
            hs->table[bucket]->first = hashset_recalc_first(hs, bucket);
    }
}

short hashset_is_empty(HashSet *hs) {
    return(hs->count == 0);
}

int hashset_remove_first(HashSet *hs) {
    int i, take = HASH_HIVALUE;
    for(i = 0; i < hs->length; i++) {
        if(take > hs->table[i]->first)
            take = hs->table[i]->first;
    }

    hashset_remove(hs, take);
    return(take);
}

// -------------------------------------------------------------

#define NAME_LEN 15
#define MAX_STR 510
#define SCH_INIT 1000
#define SCH_CACHE 10

typedef struct {
    unsigned int id;
    char name[NAME_LEN + 1];
} PersonTask;

typedef struct {
    int task_count;    //number of tasks pending before this can be scheduled to run

    int *dependency;    //list of all entries which relies on the execution of this first
    unsigned int dlen;
    unsigned int dcache;
} SchedulerTask;

void Partition(int Esq,int Dir,int *i,int *j,PersonTask *A) {
    PersonTask x, w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2];
    do {
        while (x.id > A[*i].id) (*i)++;
        while (x.id < A[*j].id) (*j)--;

        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Quicksort(int Esq,int Dir,PersonTask *A) {
    int i,j;

    if(Esq == Dir) return;

    Partition(Esq, Dir, &i, &j, A);
    if (Esq < j) Quicksort(Esq, j, A);
    if (i < Dir) Quicksort(i, Dir, A);
}

unsigned int Bsearch(PersonTask *A, unsigned int len, char *str, char *query) {
     strncpy(query, str, NAME_LEN);

     int st = 0, en = len - 1, slot;
     unsigned int key = jdb2(query);

     while (st <= en) {
          slot = (st + en) / 2;
          if (key == A[slot].id) return(slot);

          if (key < A[slot].id) en = slot - 1;
          else st = slot + 1;
     }

     return -1;   // not found
}

void scheduler_insert_dependency(SchedulerTask *st, int task) {
    if(st->dlen == st->dcache) {
        st->dcache *= SCH_CACHE;
        st->dependency = (int *)realloc(st->dependency, st->dcache * sizeof(int));
    }

    st->dependency[st->dlen] = task;
    (st->dlen)++;
}

SchedulerTask **generate_scheduler_dependencies(char *str, PersonTask *pt, unsigned int tasks) {
    SchedulerTask **st = (SchedulerTask **)malloc(tasks * sizeof(SchedulerTask *));

    char *tok, query[NAME_LEN];
    unsigned int i, j;
    for(i = 0; i < tasks; i++) {
        st[i] = (SchedulerTask *)malloc(sizeof(SchedulerTask));
        st[i]->dependency = (int *)malloc(SCH_INIT * sizeof(int));

        st[i]->dcache = SCH_INIT;
        st[i]->dlen = 0;

        st[i]->task_count = 0;
    }

    for(i = 0; i < tasks; i++) {
        gets(str);
        tok = strtok(str, " ");

        //bsearch gets the relative index of a string key on a array of sorted string keys
        unsigned int dependent = Bsearch(pt, tasks, tok, query);
        if(dependent == -1) continue;

        tok = strtok(NULL, " ");
        unsigned int relations = atoi(tok);

        for(j = 0; j < relations; j++) {
            tok = strtok(NULL, " ");
            unsigned int independent = Bsearch(pt, tasks, tok, query);
            if(independent == -1) continue;

            scheduler_insert_dependency(st[independent], dependent);
            (st[dependent]->task_count)++;
        }
    }

    return(st);
}

void finalize_scheduler_dependencies(SchedulerTask **st, unsigned int tasks) {
    unsigned int i;
    for(i = 0; i < tasks; i++) {
        free(st[i]->dependency);
        free(st[i]);
    }

    free(st);
}

void get_initial_tasks(HashSet *hs, SchedulerTask **st, unsigned int st_len) {
    unsigned int i;
    for(i = 0; i < st_len; i++) {
        if(st[i]->task_count == 0) {
            hashset_insert(hs, i);
        }
    }
}

PersonTask* get_name_list(char *str, unsigned int tasks) {
    char *tok;
    PersonTask *pt = (PersonTask *)malloc(tasks * sizeof(PersonTask));

    gets(str);
    tok = strtok(str, " ");

    unsigned int i;
    for(i = 0; i < tasks; i++) {
        strncpy(pt[i].name, tok, NAME_LEN);
        pt[i].id = jdb2(pt[i].name);

        tok = strtok(NULL, " ");
    }

    Quicksort(0, tasks - 1, pt);
    return(pt);
}

void execute_scheduler(char *str, unsigned int tasks, unsigned int instance) {
    //init name list structure
    PersonTask *pt = get_name_list(str, tasks);

    int *order = (int *)malloc(tasks * sizeof(int));
    unsigned int order_len = 0;

    HashSet *hs = hashset_create();
    SchedulerTask **st = generate_scheduler_dependencies(str, pt, tasks);

    get_initial_tasks(hs, st, tasks);

    while(!hashset_is_empty(hs)) {
        int task = hashset_remove_first(hs);

        //pick task, execute it, and remove it from the other tasks' dependencies
        unsigned int i;
        for(i = 0; i < st[task]->dlen; i++) {
            int task_relieved = st[task]->dependency[i];

            (st[task_relieved]->task_count)--;
            if(st[task_relieved]->task_count == 0) {
                hashset_insert(hs, task_relieved);
            }
        }

        order[order_len] = task;
        order_len++;
    }

    printf("Teste %u\n", instance);
    if(order_len == tasks) {
        //could retrieve full list of friends

        unsigned int i;
        for(i = 0; i < order_len; i++) {
            printf("%s ", pt[order[i]].name);
        }
        printf("\n");
    }
    else {
        printf("impossivel\n");
    }
    printf("\n");

    hashset_destroy(hs);
    finalize_scheduler_dependencies(st, tasks);

    free(order);
    free(pt);
}

int main() {
    char str[MAX_STR], *tok;
    unsigned int tasks, instance = 1;

    while(1) {
        gets(str);

        tok = strtok(str, " ");
        tasks = atoi(tok);
        if(tasks == 0) break;

        execute_scheduler(str, tasks, instance);
        instance++;
    }

    return 0;
}
