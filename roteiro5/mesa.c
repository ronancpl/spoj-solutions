#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GRAPH_LINKS 100
#define GRAPH_CACHE 2

enum{
    COLOR_BLUE=0,
    COLOR_RED,
    COLOR_NIL
};

typedef struct {
    short color;
    int *links;

    int link_len;
    int link_cache;
} GraphNode;

typedef struct {
    GraphNode **list;
    int length;
} GraphTable;

void graph_include_edge(GraphTable *gt, int from, int to) {
    if(gt->list[from]->link_len == gt->list[from]->link_cache) {
        gt->list[from]->link_cache *= GRAPH_CACHE;
        gt->list[from]->links = realloc(gt->list[from]->links, gt->list[from]->link_cache * sizeof(int));
    }

    gt->list[from]->links[gt->list[from]->link_len] = to;
    (gt->list[from]->link_len)++;
}

GraphTable *graph_generate_table(int nodes, int relations, int **relation_list) {
    GraphTable *gt = (GraphTable *)malloc(sizeof(GraphTable));
    int i;

    gt->length = nodes;
    gt->list = (GraphNode **)malloc(nodes * sizeof(GraphNode *));

    for(i = 0; i < nodes; i++) {
        gt->list[i] = (GraphNode *)malloc(sizeof(GraphNode));
        gt->list[i]->links = (int *)malloc(GRAPH_LINKS * sizeof(int));

        gt->list[i]->link_cache = GRAPH_LINKS;
        gt->list[i]->link_len = 0;
        gt->list[i]->color = COLOR_NIL;
    }

    for(i = 0; i < relations; i++) {
        graph_include_edge(gt, relation_list[0][i], relation_list[1][i]);
        graph_include_edge(gt, relation_list[1][i], relation_list[0][i]);
    }

    return(gt);
}

void graph_finalize_table(GraphTable *gt) {
    int i;
    for(i = 0; i < gt->length; i++) {
        free(gt->list[i]->links);
        free(gt->list[i]);
    }

    free(gt->list);
    free(gt);
}

void worklist_push(int *wk, int *wk_len, int node) {
    wk[*wk_len] = node;
    (*wk_len)++;
}

int worklist_pop(int *wk, int *wk_len) {
    (*wk_len)--;
    return(wk[*wk_len]);
}

short graph_color_clash(GraphTable *gt, int node1, int node2) {
    return(gt->list[node1]->color == gt->list[node2]->color);
}

void graph_color_node(GraphTable *gt, int node, short color) {
    gt->list[node]->color = color;
}

short graph_get_color(GraphTable *gt, int node) {
    return(gt->list[node]->color);
}

short graph_no_color(GraphTable *gt, int node) {
    return(gt->list[node]->color == COLOR_NIL);
}

short graph_is_bipartite(GraphTable *gt) {
    int *work_list = (int *)malloc(gt->length * sizeof(int));
    int work_len = 0;
    int j;

    for(j = 0; j < gt->length; j++)
        graph_color_node(gt, j, COLOR_NIL);

    for(j = 0; j < gt->length; j++) {
        if(graph_get_color(gt, j) != COLOR_NIL) continue;

        worklist_push(work_list, &work_len, j);
        graph_color_node(gt, j, COLOR_BLUE);

        while(work_len > 0) {
            int node = worklist_pop(work_list, &work_len);
            short color = graph_get_color(gt, node);
            short other = (color + 1) % 2;

            int i;
            for(i = 0; i < gt->list[node]->link_len; i++) {
                int near = gt->list[node]->links[i];

                if(graph_color_clash(gt, node, near)) {
                    free(work_list);
                    return(0);
                }

                if(graph_no_color(gt, near)) {
                    graph_color_node(gt, near, other);
                    worklist_push(work_list, &work_len, near);
                }
            }
        }
    }

    free(work_list);
    return(1);
}

int **get_relation_list(int len) {
    if(len == 0) return(NULL);
    int **list = (int **)malloc(2 * sizeof(int *));

    list[0] = (int *)malloc(len * sizeof(int));
    list[1] = (int *)malloc(len * sizeof(int));

    int i;
    for(i = 0; i < len; i++) {
        scanf("%d", &(list[0][i]));
        scanf("%d", &(list[1][i]));
    }

    return(list);
}

void erase_relation_list(int **list) {
    if(list != NULL) {
        free(list[1]);
        free(list[0]);
        free(list);
    }
}

void run_instance(int id,int n,int m) {
    if(n < 1) {
        printf("Instancia %d\n",id);
        printf("nao\n\n");
        return;
    }

    int **list = get_relation_list(m);
    GraphTable *gt = graph_generate_table(n + 1, m, list);
    erase_relation_list(list);

    printf("Instancia %d\n",id);
    if(!graph_is_bipartite(gt)) printf("nao\n\n");
    else printf("sim\n\n");

    graph_finalize_table(gt);
}

int main() {
    int id = 1,n,m;

    while(1) {
        if(scanf("%d", &n) == EOF) break;
        if(scanf("%d", &m) == EOF) break;

        run_instance(id,n,m);
        id++;
    }

    return 0;
}
