#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 20000000
#define MAX_STR 4200000
#define CORMEN_MAGIC_NUMBER 42

#define TMP_STR 20

typedef struct {
    int *aw_list_from;
    int *aw_list_to;

    int len;
} AdamWord_Stack;

AdamWord_Stack *create_stack(int size) {
    AdamWord_Stack *as = (AdamWord_Stack *)malloc(sizeof(AdamWord_Stack));
    as->aw_list_from = (int *)malloc(size * sizeof(int));
    as->aw_list_to = (int *)malloc(size * sizeof(int));

    as->len = 0;

    return(as);
}

void delete_stack(AdamWord_Stack *as) {
    free(as->aw_list_from);
    free(as->aw_list_to);
    free(as);
}

short is_stack_empty(AdamWord_Stack *as) {
    return(as->len == 0);
}

void pop_stack(AdamWord_Stack *as, int *from, int *to) {
    (as->len)--;

    *from = as->aw_list_from[as->len];
    *to = as->aw_list_to[as->len];
}

void push_stack(AdamWord_Stack *as, int from, int to) {
    as->aw_list_from[as->len] = from;
    as->aw_list_to[as->len] = to;

    (as->len)++;
}

void flush_stack(AdamWord_Stack *to, AdamWord_Stack *from) {
    int v1, v2;

    while(!is_stack_empty(from)) {
        pop_stack(from, &v1, &v2);
        push_stack(to, v1, v2);
    }
}

short get_next_adam_sequence(int *index, AdamWord_Stack *as, int *fetch) {
    if(*index == as->len) return(0);

    while(as->aw_list_from[*index] > *fetch) {
        (*index)++;
        if(*index == as->len) return(0);
    }
    if(as->aw_list_from[*index] < *fetch) return(0);

    //selects all sequences starting with fetch
    int i, st, en;
    st = *index;
    while(as->aw_list_from[*index] == *fetch) {
        (*index)++;
        if(*index == as->len) break;
    }
    en = *index;

    //picks the instance with the lesser length
    int seq_len = INF, dist, eligible = -1;
    for(i = st; i < en; i++) {
        dist = as->aw_list_from[i] - as->aw_list_to[i];
        if(dist < seq_len) {
            dist = seq_len;
            eligible = i;
        }
    }

    if(eligible > -1) {
        *fetch = as->aw_list_to[eligible];
        return(1);
    }

    return(0);
}

int retrieve_max_messages_from_stack(AdamWord_Stack *as, int msg_len) {
    int i = msg_len - 1, max_msg = 0, index = 0;

    while(i >= 0) {
        if(get_next_adam_sequence(&index, as, &i))
            max_msg++;

        i--;
    }

    return(max_msg);
}

int execute_cormen_algorithm(int *section, int section_len) {
    //values at 'sections' are NON-NEGATIVE
    AdamWord_Stack *aw_list = create_stack(section_len);
    AdamWord_Stack *temp = create_stack(section_len);

    int from = 0;
    int *sum = (int *)calloc(section_len + 1, sizeof(int));
    sum[section_len] = -1;

    int i, j;
    for(i = 0; i < section_len; i++) {
        for(j = from; j <= i; j++)
            sum[j] += section[i];

        j = from;
        while(sum[j] >= CORMEN_MAGIC_NUMBER) j++;

        int k = j - 1;
        while(k >= from && sum[k] == CORMEN_MAGIC_NUMBER) {
            //detected new message, from indexes 'k' to 'i'

            //the stack is devised to register data in reverse order
            push_stack(temp, i, k);
            k--;
        }

        from = j;
    }

    flush_stack(aw_list, temp);
    int messages = retrieve_max_messages_from_stack(aw_list, section_len);

    delete_stack(aw_list);
    delete_stack(temp);
    free(sum);

    return(messages);
}

int main() {
    char *str, *tok, *out_str, temp[TMP_STR];
    str = (char *)malloc(MAX_STR * sizeof(char));

    out_str = (char *)malloc(MAX_STR * sizeof(char));
    out_str[0] = 0;

    int i, j, sections, chars;
    int *sequence;

    gets(str);
    tok = strtok(str, " ");
    sections = atoi(tok);

    for(i = 0; i < sections; i++) {
        gets(str);
        tok = strtok(str, " ");
        chars = atoi(tok);
        if(chars <= 0) continue;

        sequence = (int *)malloc(chars * sizeof(int));

        gets(str);
        tok = strtok(str, " ");
        for(j = 0; j < chars; j++) {
            sequence[j] = atoi(tok);
            tok = strtok(NULL, " ");
        }

        sprintf(temp,"%d\n", execute_cormen_algorithm(sequence, chars));
        strcat(out_str, temp);

        free(sequence);
    }

    out_str[strlen(out_str)] = 0;
    printf("%s", out_str);

    free(str);
    free(out_str);
    return(0);
}
