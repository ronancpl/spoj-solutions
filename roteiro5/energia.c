#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//----------------- HASHSET V1.0 ---------------------
//NOTE: should the HASH_MAXITEM or HASH_NUMBUCK value be too small, program will crash by SIG_SEGV
#define HASH_HIVALUE 2147483647     //32-BIT integer

#define HASH_MAXITEM 20
#define HASH_NUMBUCK 70

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

unsigned int hashset_get_count(HashSet *hs) {
    return(hs->count);
}

short hashset_is_empty(HashSet *hs) {
    return(hs->count == 0);
}

void hashset_make_empty(HashSet *hs) {
    unsigned int i;
    for(i = 0; i < hs->length; i++) {
        hs->table[i]->first = HASH_HIVALUE;
        hs->table[i]->count = 0;
    }

    hs->count = 0;
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

void hashset_merge(HashSet *hs1, HashSet *hs2) {
    //add values from hs2 to hs1

    unsigned int i, j;
    for(i = 0; i < hs2->length; i++) {
        for(j = 0; j < hs2->table[i]->count; j++) {
            hashset_insert(hs1, hs2->table[i]->list[j]);
        }
    }
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

/*
    HASHSET:

    HashSet* hashset_create();
    void hashset_destroy(HashSet *hs);
    short hashset_contains(HashSet *hs, int item, unsigned int *bucket);
    void hashset_insert(HashSet *hs, int item);
    void hashset_remove(HashSet *hs, int item);
    unsigned int hashset_get_count(HashSet *hs);
    short hashset_is_empty(HashSet *hs);
    void hashset_make_empty(HashSet *hs);
    int hashset_remove_first(HashSet *hs);
    void hashset_merge(HashSet *hs1, HashSet *hs2);
    void hashset_dump(HashSet *hs);
*/
//----------------------------------------------------

//------------------ GRAPH V1.0 ----------------------
#define GRAPH_LINKS 50
#define GRAPH_CACHE 2

enum {
    GRAPH_COLOR_WHITE=0,
    GRAPH_COLOR_GREY,
    GRAPH_COLOR_BLACK
};

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

    short flag;
    HashSet *reaches;

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

    node->reaches = hashset_create();

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

    hashset_destroy(node->reaches);

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

short graph_get_node_flag(GraphTAD *graph, unsigned int i) {
    return(graph->nodes[i]->flag);
}

void graph_set_node_flag(GraphTAD *graph, unsigned int i, short flag) {
    graph->nodes[i]->flag = flag;
}

void graph_clear_flags(GraphTAD *graph) {
    unsigned int i;

    for(i = 0; i < graph->length; i++)
        graph_set_node_flag(graph, i, GRAPH_COLOR_WHITE);
}

void graph_init_connected(GraphTAD *graph) {
    //graph_clear_flags(graph);

    unsigned int i;
    for(i = 0; i < graph->length; i++) {
        hashset_make_empty(graph->nodes[i]->reaches);
        hashset_insert(graph->nodes[i]->reaches, i);
    }
}

void graph_run_connected(unsigned int from, unsigned int actual, GraphTAD *graph) {
    hashset_insert(graph->nodes[from]->reaches, actual);

    unsigned int i, dummy;
    for(i = 0; i < graph->nodes[actual]->unique_links_len; i++) {
        unsigned int link = graph_get_node_id(graph->nodes[actual]->unique_links[i]->node);

        if(!hashset_contains(graph->nodes[from]->reaches, link, &dummy))
            graph_run_connected(from, link, graph);
    }
}

short graph_is_connected(GraphTAD *graph) {
    graph_init_connected(graph);
    graph_run_connected(0, 0, graph);

    if(hashset_get_count(graph->nodes[0]->reaches) < graph->length) return(0);
    return(1);
}

//_overall: answer structure supports detailed information about disconnections, however is much slower to run.
void graph_run_connected_overall(unsigned int actual, GraphTAD *graph, unsigned int *path, unsigned int *path_len) {
    unsigned int i, dummy;
    for(i = 0; i < *path_len; i++)
        hashset_merge(graph->nodes[path[i]]->reaches, graph->nodes[actual]->reaches);

    path[*path_len] = actual;
    (*path_len)++;

    for(i = 0; i < graph->nodes[actual]->unique_links_len; i++) {
        unsigned int link = graph_get_node_id(graph->nodes[actual]->unique_links[i]->node);

        if(!hashset_contains(graph->nodes[path[0]]->reaches, link, &dummy))
            graph_run_connected_overall(link, graph, path, path_len);
    }

    (*path_len)--;
}

short graph_is_connected_overall(GraphTAD *graph) {
    graph_init_connected(graph);

    unsigned int *path = (unsigned int *)malloc(graph->length * sizeof(unsigned int));
    unsigned int path_len;

    unsigned int i;
    for(i = 0; i < graph->length; i++) {
        path_len = 0;
        graph_run_connected_overall(i, graph, path, &path_len);
    }

    free(path);

    //verifies existence of disconnection between two nodes
    for(i = 0; i < graph->length; i++)
        if(hashset_get_count(graph->nodes[i]->reaches) < graph->length) return(0);

    return(1);
}

/*
    Please REFER to the graph's STRUCTS to learn about iterating inside the graph's nodes.
    In other words, the own user must iterate over the nodes' links.

    'Unique Links' version of the graph eliminates multiples directed links between same pair of nodes,
    however the weight used is taken as the first from-to link instantiated by the user.

    GraphTAD REQUIRES graphs to be statically committed (via graph_commit function) before functioning.
    After instantiating the graph and inserting all links, make it ready for use with graph_commit.

    GRAPH:

    GraphTAD* graph_create(unsigned int len);
    GraphTAD* graph_create_transposed_graph(GraphTAD *graph);
    void graph_dump(GraphTAD *graph);
    void graph_destroy(GraphTAD *graph);

    void graph_set_node_value(GraphTAD *graph, unsigned int id, int value);
    void graph_set_node_link(GraphTAD *graph, unsigned int id, unsigned int to, int weight);
    short graph_have_node_link(GraphTAD *graph, unsigned int from, unsigned int to);
    void graph_commit(GraphTAD *graph, short unique_links);
    short graph_get_node_flag(GraphTAD *graph, unsigned int i);
    void graph_set_node_flag(GraphTAD *graph, unsigned int i, short flag);
    void graph_clear_flags(GraphTAD *graph);
    short graph_is_connected(GraphTAD *graph);
    short graph_is_connected_overall(GraphTAD *graph);
*/

//----------------------------------------------------

#define MAX_STR 20

void execute_entry(char *str, unsigned int stations, unsigned int tracks) {
    char *tok;
    GraphTAD *graph = graph_create(stations);

    unsigned int i;
    for(i = 0; i < tracks; i++) {
        gets(str);
        tok = strtok(str, " ");
        unsigned int s1 = atoi(tok) - 1;

        tok = strtok(NULL, " ");
        unsigned int s2 = atoi(tok) - 1;

        graph_set_node_link(graph, s1, s2, 1);
        graph_set_node_link(graph, s2, s1, 1);
    }
    graph_commit(graph, GRAPH_UNIQUE_LINKS);

    if(graph_is_connected_overall(graph)) printf("normal\n");
    else printf("falha\n");

    graph_destroy(graph);
}

int main() {
    char str[MAX_STR],*tok;
    unsigned int instance = 1;

    while(1) {
        gets(str);
        tok = strtok(str, " ");
        unsigned int stations = atoi(tok);

        tok = strtok(NULL, " ");
        unsigned int tracks = atoi(tok);

        if(stations == 0) break;

        printf("Teste %d\n", instance);
        execute_entry(str, stations, tracks);
        printf("\n");

        instance++;
    }

    return 0;
}
