#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALG 2       //maze dimensions must not pass 99

#define LIST_OUT    0
#define LIST_OPEN   1
#define LIST_CLOSED 2

#define MAZE_EXIT 0
#define MAZE_HERO 3
#define MAZE_WALK 5

#define MAZE_PATHS 4    //4 directions
#define MAZE_MAX 20

#define MAZE_PATH_START -1
#define MAZE_PATH_UPW   0
#define MAZE_PATH_DWN   1
#define MAZE_PATH_LFT   2
#define MAZE_PATH_RGT   3

#define MAZE_COST_INF 10000     //very high value
#define MAZE_COST_REG 1
#define MAZE_COST_END -1

typedef struct Slot {
    int x;
    int y;

    int weight;
    int dist;
    short int isEnd;    //objective slot
    short int isUsed;   //defines if it is either on open or closed list

    short int from;    //id of the move made to reach here
    struct Slot *obj;  //nearest objective slot
} Slot;

typedef struct {
    Slot **map;

    int x_len;
    int y_len;

    Slot **open_list;    //list of in progress tiles
    int open_len;
    int open_len_min;

    Slot **end_list;     //list of possible objective tiles
    int end_len;
    int end_max;

    Slot *last;
} Maze;

short int debugHeap(Slot **list, int len) {
    int at = 1;

    while(at <= len / 2) {
        if(2*at < len && list[at - 1]->dist > list[2*at - 1]->dist) return(0);
        if(2*at + 1 < len && list[at - 1]->dist > list[2*at]->dist) return(0);

        at++;
    }

    return(1);
}

void printHeap(Slot **list,int len) {
    int i;
    for(i = 0; i < len; i++) {
        printf("%d ",list[i]->dist);
    }
    printf("\n");
    system("pause");
}

void BinaryHeap_Swap(Slot **s1,Slot **s2) {
    Slot *t = *s1;
    *s1 = *s2;
    *s2 = t;
}

void BinaryHeap_Insert(Slot **list, int len, Slot *ins) {
    list[len] = ins;

    //binary heaps are based from 1
    len++;
    int val;
    while(len >= 2) {
        val = len;
        len /= 2;

        //lowest value on the start of the heap
        if(list[val - 1]->dist < list[len - 1]->dist) {
            BinaryHeap_Swap(&(list[val - 1]),&(list[len - 1]));
        }
        else break;
    }
}

int BinaryHeap_DistValue(int slot, Slot **list, int len) {
    return((slot < len) ? list[slot]->dist : MAZE_COST_INF);
}

short BinaryHeap_LeastValue(int val0,int val1,int val2) {
    short res = 0;
    int min = val0;

    if(val1 < min) {
        res = 1;
        min = val1;
    }
    if(val2 < min) {
        res = 2;
        min = val2;
    }

    return(res);
}

void BinaryHeap_Remove(Slot **list, int len) {
    int val = 1;
    int dist1, dist2;

    while(1) {
        dist1 = BinaryHeap_DistValue(2*val - 1, list, len);
        dist2 = BinaryHeap_DistValue(2*val, list, len);

        switch(BinaryHeap_LeastValue(list[val - 1]->dist,dist1,dist2)) {
            case 1:
                BinaryHeap_Swap(&(list[val - 1]),&(list[2*val - 1]));
                val = 2*val;
                break;

            case 2:
                BinaryHeap_Swap(&(list[val - 1]),&(list[2*val]));
                val = 2*val + 1;
                break;

            default:
                return;
        }
    }
}

void InsertToOpenList(Maze *maze,Slot *slot) {
    if(slot->x == -1 || slot->y == -1) return;

    BinaryHeap_Insert(maze->open_list,maze->open_len,slot);
    maze->open_len++;

    //printf("Insert '%d'\n",slot->dist);
    //printHeap(maze->open_list, maze->open_len);
    if(!debugHeap(maze->open_list, maze->open_len)) printf("HEAP ERROR: Insert\n");
}

int IsMin(int val1,int val2) {
    if(val1 < val2) return(1);
    return(0);
}

Slot *RemoveFromOpenList(Maze *maze) {
    if(maze->open_len == 0) {
        //should NEVER be empty
        return(NULL);
    }

    Slot *ret = maze->open_list[0];

    (maze->open_len)--;
    maze->open_list[0] = maze->open_list[maze->open_len];
    BinaryHeap_Remove(maze->open_list, maze->open_len);

    //printf("Remove '%d'\n",ret->dist);
    //printHeap(maze->open_list, maze->open_len);
    if(!debugHeap(maze->open_list, maze->open_len)) printf("CRITICAL ERROR: Remove\n");

    return(ret);
}

Slot AddMazeSlot(int i,int j,int w) {
    Slot end;

    end.x = i;
    end.y = j;
    end.weight = w;

    end.isEnd = 0;
    end.isUsed = LIST_OUT;
    end.dist = MAZE_COST_INF;
    end.from = MAZE_PATH_START;

    end.obj = NULL;

    return(end);
}

int ReturnSlotWeight(int val) {
    switch(val) {
    case 0:
        return(MAZE_COST_END);

    case 2:
        return(MAZE_COST_INF);

    case 3:
        return(MAZE_HERO);

    default:
        return(MAZE_COST_REG);
    }
}

Maze* MakeMazeFromFile(char *file_name,Slot **hero) {
    FILE *file = fopen(file_name,"r+t");
    if(file == NULL) return(NULL);

    char str2[MAZE_COST_INF + 1],*tok2;
    int x,y;
    fgets(str2,MAZE_COST_INF,file);

    tok2 = strtok(str2," \n");
    x = atoi(tok2);

    tok2 = strtok(NULL," \n");
    y = atoi(tok2);

    if(x <= 0 || y <= 0) {
        fclose(file);
        return(NULL);
    }

    Maze *maze = (Maze *)calloc(1,sizeof(Maze));
    maze->x_len = x;
    maze->y_len = y;

    maze->open_list = (Slot **)malloc(maze->x_len * maze->y_len*sizeof(Slot *));
    maze->open_len = 0;
    maze->open_len_min = MAZE_COST_INF;

    maze->end_list = (Slot **)malloc(MAZE_MAX*sizeof(Slot *));
    maze->end_max = MAZE_MAX;
    maze->end_len = 0;

    maze->map = (Slot **)calloc(x,sizeof(Slot *));
    int i,j;

    for(i = 0; i < x; i++) {
        maze->map[i] = (Slot *)calloc(y,sizeof(Slot));
    }

    char *tok,*str = (char *)malloc((((MAX_ALG + 1)*y) + 1)*sizeof(char));

    for(i = 0; i < x; i++) {
        fgets(str,MAZE_COST_INF,file);
        tok = strtok(str," \n");

        for(j = 0; j < y && tok != NULL; j++) {
            maze->map[i][j] = AddMazeSlot(i,j,ReturnSlotWeight(atoi(tok)));

            if(maze->map[i][j].weight == MAZE_COST_END) {
                maze->map[i][j].weight = MAZE_COST_REG;
                maze->map[i][j].isEnd = 1;

                if(maze->end_max == maze->end_len) {
                    maze->end_max += MAZE_MAX;
                    maze->end_list = (Slot **)realloc(maze->end_list,maze->end_max*sizeof(Slot *));
                }

                maze->end_list[maze->end_len] = &(maze->map[i][j]);
                (maze->end_len)++;
            }
            else if(maze->map[i][j].weight == MAZE_HERO) {
                maze->map[i][j].weight = MAZE_COST_REG;
                maze->map[i][j].dist = 0;

                *hero = &(maze->map[i][j]);
            }

            tok = strtok(NULL," \n");
        }
    }
    free(str);

    fclose(file);

    return(maze);
}

Maze* MakeMaze(Slot **hero) {
    char str2[MAZE_COST_INF + 1],*tok2;
    int x,y;
    gets(str2);

    tok2 = strtok(str2," \n");
    x = atoi(tok2);

    tok2 = strtok(NULL," \n");
    y = atoi(tok2);

    if(x <= 0 || y <= 0) {
        return(NULL);
    }

    Maze *maze = (Maze *)calloc(1,sizeof(Maze));
    maze->x_len = x;
    maze->y_len = y;

    maze->open_list = (Slot **)malloc(maze->x_len * maze->y_len*sizeof(Slot *));
    maze->open_len = 0;
    maze->open_len_min = MAZE_COST_INF;

    maze->end_list = (Slot **)malloc(MAZE_MAX*sizeof(Slot *));
    maze->end_max = MAZE_MAX;
    maze->end_len = 0;

    maze->map = (Slot **)calloc(x,sizeof(Slot *));
    int i,j;

    for(i = 0; i < x; i++) {
        maze->map[i] = (Slot *)calloc(y,sizeof(Slot));
    }

    char *tok,*str = (char *)malloc((((MAX_ALG + 1)*y) + 1)*sizeof(char));

    for(i = 0; i < x; i++) {
        gets(str);
        tok = strtok(str," \n");

        for(j = 0; j < y && tok != NULL; j++) {
            maze->map[i][j] = AddMazeSlot(i,j,ReturnSlotWeight(atoi(tok)));

            if(maze->map[i][j].weight == MAZE_COST_END) {
                maze->map[i][j].weight = MAZE_COST_REG;
                maze->map[i][j].isEnd = 1;

                if(maze->end_max == maze->end_len) {
                    maze->end_max += MAZE_MAX;
                    maze->end_list = (Slot **)realloc(maze->end_list,maze->end_max*sizeof(Slot *));
                }

                maze->end_list[maze->end_len] = &(maze->map[i][j]);
                (maze->end_len)++;
            }
            else if(maze->map[i][j].weight == MAZE_HERO) {
                maze->map[i][j].weight = MAZE_COST_REG;
                maze->map[i][j].dist = 0;

                *hero = &(maze->map[i][j]);
            }

            tok = strtok(NULL," \n");
        }
    }
    free(str);

    return(maze);
}

void DestroyMaze(Maze *maze) {
    free(maze->open_list);
    free(maze->end_list);

    int i;
    for(i = 0; i < maze->x_len; i++) free(maze->map[i]);

    free(maze->map);
    free(maze);
}

int Absolute(int val) {
    if(val < 0) val *= -1;
    return(val);
}

int ManhattanDist(int x,int y,Slot obj) {
    return(Absolute(obj.x - x) + Absolute(obj.y - y));
}

void RecalcBestExit(Maze *maze,Slot *hero) {
    int i;
    int val = MAZE_COST_INF,val2;

    for(i = 0; i < maze->end_len; i++) {
        val2 = ManhattanDist(hero->x,hero->y,*(maze->end_list[i]));

        if(!IsMin(val,val2)) {
            val = val2;
            hero->obj = maze->end_list[i];
        }
    }
}

void CalcSquare(Maze *maze,int orig_dist,short int pathid,int x,int y,int ox,int oy) {
    if(x < 0 || x >= maze->x_len || y < 0 || y >= maze->y_len || maze->map[x][y].weight == MAZE_COST_INF || maze->map[x][y].isUsed == LIST_CLOSED) return;

    //this tile
    Slot *slot = &(maze->map[x][y]);

    //objective of the parent tile
    Slot *obj = (maze->map[ox][oy]).obj;

    //distance is computed based on weight of the traveling thus far plus the weight of this tile.
    int actual_dist = orig_dist + slot->weight;

    //compares the encountered distance value for this tile with what it had before
    if(slot->dist > actual_dist) {
        slot->dist = actual_dist;
        slot->from = pathid;
    }

    //if searched tile became far than before, maybe there is other better exit?
    if(ManhattanDist(x,y,*obj) > ManhattanDist(ox,oy,*obj)) {
        RecalcBestExit(maze,slot);
    }
    else if(slot->isUsed == LIST_OUT) {
        slot->obj = obj;
    }

    //heuristic distance to the objective, TBD by the program
    //actual_dist += ManhattanDist(x,y,*(slot->obj));

    if(slot->isUsed == LIST_OUT) {
        slot->isUsed = LIST_OPEN;
        InsertToOpenList(maze,slot);
    }
}

short int NextWalk(Maze *maze,int *x,int *y) {
    switch(maze->map[*x][*y].from) {
        case MAZE_PATH_LFT:
            (*x)++;
            break;

        case MAZE_PATH_RGT:
            (*x)--;
            break;

        case MAZE_PATH_UPW:
            (*y)++;
            break;

        case MAZE_PATH_DWN:
            (*y)--;
            break;

        default: return(0);
    }

    return(1);
}

void DumpPath(Maze *maze) {
    int x,y;

    x = maze->last->x;
    y = maze->last->y;
    do {
        maze->map[x][y].weight = MAZE_WALK;
    } while(NextWalk(maze,&x,&y));

    printf("\nMAP RESULT:\n");
    for(x = 0; x < maze->x_len; x++) {
        for(y = 0; y < maze->y_len; y++) {
            if(maze->map[x][y].weight < MAZE_COST_INF) printf("%d ",maze->map[x][y].weight);
            else printf("X ");
        }
        printf("\n");
    }
}

void DebugState(Maze *maze,Slot data) {
    printf("---- STATE DETAILS ----\n");

    printf("Hero on (%d %d)\n",data.x,data.y);

    int i;
    for(i = 0; i < maze->open_len; i++) {
        printf("(%d %d) ",maze->open_list[i]->x,maze->open_list[i]->y);
    }
    printf("\n");

    printf("----  END  DETAILS ----\n");
    system("pause");
}

void MazePathfinder(Maze *maze,Slot *hero) {
    int actual_dist = 0;
    Slot *data;

    RecalcBestExit(maze,hero);

    InsertToOpenList(maze,hero);

    while(1) {
        if(maze->open_len == 0) {
            //no path

            maze->last = NULL;
            break;
        }

        data = RemoveFromOpenList(maze);

        if(maze->map[data->x][data->y].isEnd == 1) {
            actual_dist = maze->map[data->x][data->y].dist;   //the removed path completes the search
            maze->last = &(maze->map[data->x][data->y]);
            break;
        }
        else if(maze->map[data->x][data->y].weight == MAZE_COST_INF) continue;

        //mark visited
        maze->map[data->x][data->y].isUsed = LIST_CLOSED;
        actual_dist = maze->map[data->x][data->y].dist;

        //insert adjacent tiles to the open list
        CalcSquare(maze,actual_dist,MAZE_PATH_LFT,data->x - 1,data->y,data->x,data->y);
        CalcSquare(maze,actual_dist,MAZE_PATH_RGT,data->x + 1,data->y,data->x,data->y);
        CalcSquare(maze,actual_dist,MAZE_PATH_UPW,data->x,data->y - 1,data->x,data->y);
        CalcSquare(maze,actual_dist,MAZE_PATH_DWN,data->x,data->y + 1,data->x,data->y);
    }

    //DumpPath(maze);
    printf("%d",actual_dist);
}

int main() {
    Slot* hero = NULL;
    //Maze* maze = MakeMazeFromFile("maze2.txt",&hero);
    Maze* maze = MakeMaze(&hero);

    if(hero != NULL && maze != NULL) {
        MazePathfinder(maze,hero);
        DestroyMaze(maze);
    }
    else if(maze != NULL) {
        DestroyMaze(maze);
    }

    return 0;
}
