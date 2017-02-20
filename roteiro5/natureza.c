#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ------------------ GRAPH v1.0 -------------------------------

#define GRAPH_LINKS 40
#define GRAPH_CACHE 3

enum {
    GRAPH_NO_UNIQUE_LINKS=0,
    GRAPH_UNIQUE_LINKS
};

typedef struct NodeGraph NodeGraph;
typedef struct {
    int weight;
    NodeGraph *node;
} NodePointerGraph;

struct NodeGraph {
    unsigned int id;
    int value;

    NodePointerGraph **unique_links;
    unsigned int unique_links_len;
    NodePointerGraph **links;
    unsigned int links_len;
    unsigned int links_cache;
};

typedef struct {
    NodeGraph **nodes;
    unsigned int length;
} GraphTAD;

NodeGraph* graph_init_node(unsigned int id) {
    NodeGraph *node = (NodeGraph *)malloc(sizeof(NodeGraph));

    //assume id as the index in the graph's list
    node->id = id;
    node->value = 0;

    node->links = (NodePointerGraph **)malloc(GRAPH_LINKS * sizeof(NodePointerGraph *));
    node->links_len = 0;
    node->links_cache = GRAPH_LINKS;

    unsigned int i;
    for(i = 0; i < GRAPH_LINKS; i++)
        node->links[i] = (NodePointerGraph *)malloc(sizeof(NodePointerGraph));

    node->unique_links = NULL;
    node->unique_links_len = 0;

    return(node);
}

GraphTAD* graph_create(unsigned int len) {
    GraphTAD *graph = (GraphTAD *)malloc(sizeof(GraphTAD));
    graph->nodes = (NodeGraph **)malloc(len * sizeof(NodeGraph *));
    graph->length = len;

    unsigned int i;
    for(i = 0; i < len; i++)
        graph->nodes[i] = graph_init_node(i);

    return(graph);
}

void graph_delete_node(NodeGraph *node) {
    unsigned int i;
    for(i = 0; i < node->links_cache; i++)
        free(node->links[i]);

    if(node->unique_links != NULL)
        free(node->unique_links);

    free(node->links);
    free(node);
}

void graph_destroy(GraphTAD *graph) {
    unsigned int i;
    for(i = 0; i < graph->length; i++)
        graph_delete_node(graph->nodes[i]);

    free(graph->nodes);
    free(graph);
}

NodeGraph* graph_get_node_pointer(GraphTAD *graph, unsigned int id) {
    return(graph->nodes[id]);
}

unsigned int graph_get_node_id(NodeGraph *node) {
    return(node->id);
}

void graph_set_node_value(GraphTAD *graph, unsigned int id, int value) {
    graph->nodes[id]->value = value;
}

void graph_set_node_link(GraphTAD *graph, unsigned int id, unsigned int to, int weight) {
    if(graph->nodes[id]->links_len == graph->nodes[id]->links_cache) {
        unsigned int old = graph->nodes[id]->links_cache, i;

        graph->nodes[id]->links_cache *= GRAPH_CACHE;
        graph->nodes[id]->links = (NodePointerGraph **)realloc(graph->nodes[id]->links, graph->nodes[id]->links_cache * sizeof(NodePointerGraph *));
        for(i = old; i < graph->nodes[id]->links_cache; i++)
            graph->nodes[id]->links[i] = (NodePointerGraph *)malloc(sizeof(NodePointerGraph));
    }

    graph->nodes[id]->links[graph->nodes[id]->links_len]->node = graph_get_node_pointer(graph, to);
    graph->nodes[id]->links[graph->nodes[id]->links_len]->weight = weight;

    (graph->nodes[id]->links_len)++;
}

void graph_qsort_partition(int left, int right, int *i, int *j, NodePointerGraph **A) {
    NodePointerGraph *x, *w;

    *i = left;
    *j = right;
    x = A[(*i + *j) / 2];
    do {
        while (graph_get_node_id(x->node) > graph_get_node_id(A[*i]->node)) (*i)++;
        while (graph_get_node_id(x->node) < graph_get_node_id(A[*j]->node)) (*j)--;
        if (*i <= *j) {
            w = A[*i];
            A[*i] = A[*j];
            A[*j] = w;

            (*i)++;
            (*j)--;
        }
    } while (*i <= *j);
}

void graph_qsort(int left, int right, NodePointerGraph **A) {
    if(left >= right) return;

    int i,j;
    graph_qsort_partition(left, right, &i, &j, A);
    if (left < j) graph_qsort(left, j, A);
    if (i < right) graph_qsort(i, right, A);
}

void graph_commit(GraphTAD *graph, short unique_links) {
    //get the nodes ready for bsearch algorithm and extraction of unique links

    unsigned int i, j, v;
    for(i = 0; i < graph->length; i++)
        graph_qsort(0, graph->nodes[i]->links_len - 1, graph->nodes[i]->links);

    if(unique_links) {
        //NOTE: this piece of code will NOT GRANT which value of weight will be brought with the unique link!

        for(i = 0; i < graph->length; i++) {
            graph->nodes[i]->unique_links = (NodePointerGraph **)malloc(graph->nodes[i]->links_len * sizeof(NodePointerGraph *));

            unsigned int check = -1;
            v = 0;
            for(j = 0; j < graph->nodes[i]->links_len; j++) {
                if(check != graph_get_node_id(graph->nodes[i]->links[j]->node)) {
                    check = graph_get_node_id(graph->nodes[i]->links[j]->node);

                    graph->nodes[i]->unique_links[v] = graph->nodes[i]->links[j];
                    v++;
                }
            }

            graph->nodes[i]->unique_links_len = v;
        }
    }
}

unsigned int graph_bsearch(GraphTAD *graph, unsigned int node, unsigned int key) {
     int st = 0, en = graph->nodes[node]->links_len - 1, slot;

     while (st <= en) {
          slot = (st + en) / 2;
          if (key == graph_get_node_id(graph->nodes[node]->links[slot]->node)) return(slot);

          if (key < graph_get_node_id(graph->nodes[node]->links[slot]->node)) en = slot - 1;
          else st = slot + 1;
     }

     return -1;   // not found
}

short graph_have_node_link(GraphTAD *graph, unsigned int from, unsigned int to) {
    return(graph_bsearch(graph, from, to) != -1);
}

GraphTAD* graph_create_transposed_graph(GraphTAD *graph) {
    GraphTAD *tr = graph_create(graph->length);

    unsigned int i, j;
    for(i = 0; i < graph->length; i++) {
        for(j = 0; j < graph->nodes[i]->links_len; j++) {
            graph_set_node_link(tr, graph_get_node_id(graph->nodes[i]->links[j]->node), i, graph->nodes[i]->links[j]->weight);
        }
    }

    graph_commit(tr, GRAPH_UNIQUE_LINKS);
    return(tr);
}

void graph_dump(GraphTAD *graph) {
    unsigned int i, j;
    printf("Graph v1.0 -- nodes:%d", graph->length);
    for(i = 0; i < graph->length; i++) {
        printf("\n(%d) v:%d\tAdj: ", graph->nodes[i]->id, graph->nodes[i]->value);

        for(j = 0; j < graph->nodes[i]->links_len; j++) {
            printf("%d(w:%d) ", graph_get_node_id(graph->nodes[i]->links[j]->node), graph->nodes[i]->links[j]->weight);
        }

        printf("\n\tUnique Adj: ");
        if(graph->nodes[i]->unique_links != NULL) {
            for(j = 0; j < graph->nodes[i]->unique_links_len; j++) {
                printf("%d ", graph_get_node_id(graph->nodes[i]->unique_links[j]->node));
            }
        }
        printf("\n");
    }
    printf("\n");
}

// -------------------------------------------------------------

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

#define MAX_STR 200
#define NAME_LEN 40

typedef struct {
    unsigned long id;
    char name[NAME_LEN + 1];
} NatureElement;

enum {
    NODE_UNUSED=0,
    NODE_OPEN,
    NODE_CLOSED
};

void Partition(int Esq,int Dir,int *i,int *j,NatureElement *A) {
    NatureElement x, w;

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

void Quicksort(int Esq,int Dir,NatureElement *A) {
    int i,j;

    if(Esq == Dir) return;

    Partition(Esq, Dir, &i, &j, A);
    if (Esq < j) Quicksort(Esq, j, A);
    if (i < Dir) Quicksort(i, Dir, A);
}

unsigned int Bsearch(NatureElement *A, unsigned int len, char *str, char *query) {
     strncpy(query, str, NAME_LEN);

     int st = 0, en = len - 1, slot;
     unsigned long key = jdb2(query);

     while (st <= en) {
          slot = (st + en) / 2;
          if (key == A[slot].id) return(slot);

          if (key < A[slot].id) en = slot - 1;
          else st = slot + 1;
     }

     return -1;   // not found
}

int Max(int v1, int v2) {
    return((v1 > v2) ? v1 : v2);
}

void dfs_get_component_path(unsigned int *component_count, GraphTAD *graph, short *node_flag, unsigned int node) {
    node_flag[node] = NODE_OPEN;
    (*component_count)++;

    unsigned int i;
    for(i = 0; i < graph->nodes[node]->unique_links_len; i++) {
        unsigned int nlink = graph_get_node_id(graph->nodes[node]->unique_links[i]->node);
        if(node_flag[nlink] == NODE_UNUSED) {
            dfs_get_component_path(component_count, graph, node_flag, nlink);
        }
    }

    node_flag[node] = NODE_CLOSED;
}

unsigned int get_greatest_component(GraphTAD *graph) {
    short *node_flag = (short *)malloc(graph->length * sizeof(short));

    unsigned int i;
    for(i = 0; i < graph->length; i++) node_flag[i] = NODE_UNUSED;

    unsigned int max_count = 0, component_count;
    for(i = 0; i < graph->length; i++) {
        if(node_flag[i] == NODE_UNUSED) {
            //counts the number of nodes reachable by this node
            component_count = 0;
            dfs_get_component_path(&component_count, graph, node_flag, i);

            max_count = Max(max_count, component_count);
        }
    }
    free(node_flag);

    return(max_count);
}

NatureElement* get_name_list(char *str, unsigned int types) {
    char *tok;
    NatureElement *ne = (NatureElement *)malloc(types * sizeof(NatureElement));

    unsigned int i;
    for(i = 0; i < types; i++) {
        gets(str);
        tok = strtok(str, " ");

        strncpy(ne[i].name, tok, NAME_LEN);
        ne[i].id = jdb2(ne[i].name);
    }

    Quicksort(0, types - 1, ne);
    return(ne);
}

void execute_nature_cycle(char *str, unsigned int types, unsigned int relations) {
    char *tok, query[NAME_LEN + 1];
    GraphTAD *graph = graph_create(types);
    NatureElement *ne = get_name_list(str, types);

    unsigned int i;
    for(i = 0; i < relations; i++) {
        gets(str);

        //bsearch gets the relative index of a string key on a array of sorted string keys
        tok = strtok(str, " ");
        unsigned int independent = Bsearch(ne, types, tok, query);
        if(independent == -1) continue;

        tok = strtok(NULL, " ");
        unsigned int dependent = Bsearch(ne, types, tok, query);
        if(dependent == -1) continue;

        graph_set_node_link(graph, dependent, independent, 1);
        graph_set_node_link(graph, independent, dependent, 1);
    }

    graph_commit(graph, GRAPH_UNIQUE_LINKS);

    printf("%u\n", get_greatest_component(graph));

    free(ne);
    graph_destroy(graph);
}

int main() {
    char str[MAX_STR], *tok;

    while(1) {
        gets(str);

        tok = strtok(str, " ");
        unsigned int types = atoi(tok);

        tok = strtok(NULL, " ");
        unsigned int relations = atoi(tok);

        if(types == 0) break;

        execute_nature_cycle(str, types, relations);
        gets(str);  //empty line
    }

    return 0;
}
