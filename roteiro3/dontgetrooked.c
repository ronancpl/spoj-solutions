#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WALL 999999
#define MAX_LEN 100

int IsBlocked(int ***map,int x,int y) {
    return((*map)[x][y] == WALL);
}

int Max(int val1,int val2) {
    if(val1 > val2) return(val1);
    return(val2);
}

int RookBacktrack(int actual_rook,int len,int ***map) {

    //busca nova posicao viavel
    int i,j;
    int max_rook = actual_rook;

    for(i = 0; i < len; i++) {
        for(j = 0; j < len; j++) {
            //busca o proximo slot ainda nao preenchido
            if((*map)[i][j] == 0) {
                int a,b;

                /* preenche locais que sao alvo de torre */
                ((*map)[i][j])++;

                for(a = i-1; a >= 0 && !IsBlocked(map,a,j); a--)
                    ((*map)[a][j])++;

                for(a = i+1; a < len && !IsBlocked(map,a,j); a++)
                    ((*map)[a][j])++;

                for(b = j-1; b >= 0 && !IsBlocked(map,i,b); b--)
                    ((*map)[i][b])++;

                for(b = j+1; b < len && !IsBlocked(map,i,b); b++)
                    ((*map)[i][b])++;

                int val = RookBacktrack(actual_rook + 1,len,map);
                max_rook = Max(val,max_rook);

                /* retrocedendo: apaga valores preenchidos anteriormente */

                ((*map)[i][j])--;

                for(a = i-1; a >= 0 && !IsBlocked(map,a,j); a--)
                    ((*map)[a][j])--;

                for(a = i+1; a < len && !IsBlocked(map,a,j); a++)
                    ((*map)[a][j])--;

                for(b = j-1; b >= 0 && !IsBlocked(map,i,b); b--)
                    ((*map)[i][b])--;

                for(b = j+1; b < len && !IsBlocked(map,i,b); b++)
                    ((*map)[i][b])--;
            }
        }
    }

    return(max_rook);
}

int **GetMap(int *length) {
    int len;
    char ch,str[MAX_LEN];

    gets(str);
    len = atoi(str);
    if(len <= 0) return(NULL);
    if(len > MAX_LEN) len = MAX_LEN;

    int **map = (int **)malloc(len * sizeof(int *));
    int i,j;


    for(i = 0; i < len; i++) {
        map[i] = (int *)calloc(len,sizeof(int));

        gets(str);
        for(j = 0; j < len; j++) {
            ch = str[j];

            switch(ch) {
            case 'X':
                map[i][j] = WALL;
                break;
            default:
                map[i][j] = 0;
            }
        }
    }

    *length = len;
    return(map);
}

void DestroyMap(int **map,int len) {
    int i;
    for(i = 0; i < len; i++) free(map[i]);
    free(map);
}

int main() {
    int len;
    int **map;

    while(1) {
        map = GetMap(&len);
        if(map == NULL) break;

        printf("%d\n",RookBacktrack(0,len,&map));
        DestroyMap(map,len);
    }

    return 0;
}
