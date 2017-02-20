#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//---------------- GRAPH V1.0 -------------------------------------

enum {
    GRAPH_NO_UNIQUE_LINKS=0,
    GRAPH_UNIQUE_LINKS
};

#define GRAPH_LINKS 100
#define GRAPH_CACHE 2

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
    node->unique_links = NULL;
    node->unique_links_len = 0;

    unsigned int i;
    for(i = 0; i < GRAPH_LINKS; i++)
        node->links[i] = (NodePointerGraph *)malloc(sizeof(NodePointerGraph));

    node->links_len = 0;
    node->links_cache = GRAPH_LINKS;

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

//-----------------------------------------------------------------

//--------------- HASHSET V1.0 ------------------------------------

void qsort_partition(int left, int right, int *i, int *j, int *A) {
    int x, w;

    *i = left;
    *j = right;
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

void qsort_c(int left, int right, int *A) {
    int i,j;

    qsort_partition(left, right, &i, &j, A);
    if (left < j) qsort_c(left, j, A);
    if (i < right) qsort_c(i, right, A);
}

//NOTE: should the HASH_MAXITEM or HASH_NUMBUCK value be too small, program will crash by SIG_SEGV
#define HASH_MAXITEM 20
#define HASH_NUMBUCK 15
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

void hashset_dump(HashSet *hs) {
    printf("HASHSET v1.0 -- count: %d, buckets: %d, threshold: %d\n", hs->count, hs->length, hs->threshold);

    unsigned int i, j;
    for(i = 0; i < hs->length; i++) {
        printf("\n%d -> ", i);
        for(j = 0; j < hs->table[i]->count; j++) {
            printf("%d ", hs->table[i]->list[j]);
        }
        printf("$");
    }
    printf("\n");
}

void hashset_serialize_set(HashSet *hs, int **vect, int *vect_len) {
    int *vector = (int *)malloc(hs->count * sizeof(int));
    int v = 0, i, j;

    for(i = 0; i < hs->length; i++) {
        for(j = 0; j < hs->table[i]->count; j++) {
            vector[v] = hs->table[i]->list[j];
            v++;
        }
    }

    qsort_c(0, v - 1, vector);

    *vect = vector;
    *vect_len = v;
}

//-----------------------------------------------------------------

#define TOLL_WEIGHT 1
#define MAX_STR 20

void travel_links(GraphTAD *graph, unsigned int *best, HashSet *list, unsigned int start, unsigned int jumps) {
    hashset_insert(list, start);
    best[start] = jumps;

    if(jumps > 0) {
        unsigned int i, b;
        for(i = 0; i < graph->nodes[start]->unique_links_len; i++) {
            unsigned int next = graph_get_node_id(graph->nodes[start]->unique_links[i]->node);

            if(best[next] < jumps)
                travel_links(graph, best, list, next, jumps - 1);
        }
    }
}

void execute_toll(char *str,unsigned int points,unsigned int links,unsigned int start,unsigned int jumps) {
    GraphTAD *graph = graph_create(points + 1);
    HashSet *list = hashset_create();
    unsigned int *best = (unsigned int *)calloc(points + 1, sizeof(unsigned int));

    char *tok;
    unsigned int i;
    for(i = 0; i < links; i++) {
        gets(str);

        tok = strtok(str, " ");
        unsigned int v1 = atoi(tok);

        tok = strtok(NULL, " ");
        unsigned int v2 = atoi(tok);

        graph_set_node_link(graph, v1, v2, TOLL_WEIGHT);
        graph_set_node_link(graph, v2, v1, TOLL_WEIGHT);
    }

    graph_commit(graph, GRAPH_UNIQUE_LINKS);

    //execute traveling
    travel_links(graph, best, list, start, jumps);

    //remove initial city from the answer
    hashset_remove(list, start);

    int *found, found_len;
    hashset_serialize_set(list, &found, &found_len);

    for(i = 0; i < found_len; i++)
        printf("%d ", found[i]);
    printf("\n");

    free(found);
    free(best);
    hashset_destroy(list);
    graph_destroy(graph);
}

int main() {
    char str[MAX_STR], *tok;
    unsigned int instance = 1;

    while(1) {
        unsigned int points, links, start, jumps;
        gets(str);

        tok = strtok(str, " ");
        points = atoi(tok);

        tok = strtok(NULL, " ");
        links = atoi(tok);

        tok = strtok(NULL, " ");
        start = atoi(tok);

        tok = strtok(NULL, " ");
        jumps = atoi(tok);

        if(points == 0 && links == 0 && start == 0 && jumps == 0) break;

        printf("Teste %d\n", instance);
        execute_toll(str, points, links, start, jumps);
        printf("\n", instance);
        instance++;
    }

    return 0;
}
