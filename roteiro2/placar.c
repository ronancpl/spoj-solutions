#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_STR 100
#define NAME_LEN 20

typedef struct {
    char name[NAME_LEN + 1];
    int solved;
} Student;

void Partition(int Esq,int Dir,int *i,int *j,Student **A,short mode) {
    Student *x, *w;

    *i = Esq;
    *j = Dir;
    x = A[(*i + *j) / 2];
    do {
        if(mode) {
            while (strcmp(x->name,A[*i]->name) < 0) (*i)++;
            while (strcmp(x->name,A[*j]->name) > 0) (*j)--;
        }
        else {
            while (x->solved > A[*i]->solved) (*i)++;
            while (x->solved < A[*j]->solved) (*j)--;
        }
        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void Quicksort(int Esq,int Dir,Student **A,bool mode) {
    int i,j;

    if(Esq == Dir) return;

    Partition(Esq, Dir, &i, &j, A, mode);
    if (Esq < j) Quicksort(Esq, j, A, mode);
    if (i < Dir) Quicksort(i, Dir, A, mode);
}

void quicksort_student(Student **A, int len) {
    //sort everything according to their score
    Quicksort(0, len - 1, A, 0);

    int *unique_scores = (int *)malloc((len + 1) * sizeof(int));
    int us_index = 1, i;

    unique_scores[0] = 0;
    for(i = 0; i < len - 1; i++) {
        if((A[i])->solved != (A[i + 1])->solved) {
            unique_scores[us_index] = i + 1;
            us_index++;
        }
    }
    unique_scores[us_index] = i + 1;

    //sorts lexicographically those with same score
    for(i = 0; i < us_index; i++) {
        Quicksort(unique_scores[i], unique_scores[i + 1] - 1, A, 1);
    }

    free(unique_scores);
}

Student** generate_student_list(char *str, int len) {
    char *tok;
    int i;

    Student **stud = (Student **)malloc(len * sizeof(Student *));
    for(i = 0; i < len; i++) {
        stud[i] = (Student *)malloc(sizeof(Student));
        gets(str);

        tok = strtok(str, " ");
        strncpy((stud[i])->name, tok, NAME_LEN);

        tok = strtok(NULL, " ");
        (stud[i])->solved = atoi(tok);
    }

    return(stud);
}

void free_student_list(Student **stud, int len) {
    int i;
    for(i = 0; i < len; i++)
        free(stud[i]);

    free(stud);
}

short get_string(char *str) {
    int i = 0;
    char ch;

    while(1) {
        ch = getchar();

        if(ch == EOF) {
            str[i] = 0;
            return(0);
        }
        else if(ch == '\n') {
            str[i] = 0;
            return(1);
        }

        str[i] = ch;
        i++;
    }
}

int main() {
    char str[MAX_STR], *tok;
    int instance = 1;

    while(1) {
        if(!get_string(str)) break;

        int len = atoi(str);
        if(len > 0) {
            Student **stud = generate_student_list(str, len);
            quicksort_student(stud, len);

            printf("Instancia %d\n%s\n\n", instance, (stud[0])->name);
            free_student_list(stud, len);
        }

        instance++;
    }

    return 0;
}
