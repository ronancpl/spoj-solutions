#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INF 9999999
#define MAX_STR 1000

int Max(int i, int j) {
    return((i > j) ? i : j);
}

int run_grid_max(int **grid, int h, int w) {
    int i, j;
    int *h_max = (int *)calloc(h, sizeof(int));
    int *w_max = (int *)calloc(w, sizeof(int));

    for(i = 0; i < h; i++) {
        for(j = 0; j < w; j++) {
            h_max[i] += grid[i][j];
            w_max[j] += grid[i][j];
        }
    }

    int val = -INF;
    for(i = 0; i < h; i++)
        val = Max(val, h_max[i]);

    for(j = 0; j < w; j++)
        val = Max(val, w_max[j]);

    free(h_max);
    free(w_max);

    return(val);
}

int **generate_grid(char *str, int h, int w) {
    int i, j;
    char *tok;
    int **grid = (int **)malloc(h * sizeof(int *));

    for(i = 0; i < h; i++) {
        grid[i] = (int *)malloc(w * sizeof(int));

        gets(str);
        tok = strtok(str," ");
        for(j = 0; j < w; j++) {
            grid[i][j] = atoi(tok);
            tok = strtok(NULL," ");
        }
    }

    return(grid);
}

void destroy_grid(int **grid, int h) {
    int i;
    for(i = 0; i < h; i++)
        free(grid[i]);

    free(grid);
}

int main() {
    char str[MAX_STR], *tok;
    int height, width;
    int **grid;

    gets(str);

    tok = strtok(str, " ");
    height = atoi(tok);

    tok = strtok(NULL, " ");
    width = atoi(tok);

    grid = generate_grid(str, height, width);
    printf("%d", run_grid_max(grid, height, width));
    destroy_grid(grid, height);

    return 0;
}
