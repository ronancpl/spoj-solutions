#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------- HEAP V1.0 ------------------------

#define ARC_INF 2147483647

typedef struct heapData {
    //item holder data

    long int place_dist;
    long int place_id;
} heapData;

typedef struct heapNode {
    long int value;

    //item holder
    heapData data;
} heapNode;

typedef struct PQ {
    heapNode* heap;
    long int size;
} PQ;

void insert(heapNode aNode, heapNode* heap, int size) {
    int idx;
    heapNode tmp;
    idx = size + 1;
    heap[idx] = aNode;
    while (heap[idx].value < heap[idx/2].value && idx > 1) {
        tmp = heap[idx];
        heap[idx] = heap[idx/2];
        heap[idx/2] = tmp;
        idx /= 2;
    }
}

void shiftdown(heapNode* heap, int size, int idx) {
    int cidx;        //index for child
    heapNode tmp;
    for (;;) {
        cidx = idx*2;
        if (cidx > size) {
            break;   //it has no child
        }
        if (cidx < size) {
            if (heap[cidx].value > heap[cidx+1].value) {
                ++cidx;
            }
        }
        //swap if necessary
        if (heap[cidx].value < heap[idx].value) {
            tmp = heap[cidx];
            heap[cidx] = heap[idx];
            heap[idx] = tmp;
            idx = cidx;
        } else {
            break;
        }
    }
}

heapNode removeMin(heapNode* heap, int size) {
    int cidx;
    heapNode rv = heap[1];

    heap[1] = heap[size];
    --size;
    shiftdown(heap, size, 1);
    return rv;
}

void enqueue(heapNode node, PQ *q) {
    insert(node, q->heap, q->size);
    ++q->size;
}

heapNode dequeue(PQ *q) {
   heapNode rv = removeMin(q->heap, q->size);
   --q->size;
   return rv;
}

heapData GetHeapData(long int dist,long int place_id) {
    heapData hd;
    hd.place_dist = dist;
    hd.place_id = place_id;

    return hd;
}

heapNode GetHeapNode(long int dist,long int place_id) {
    heapNode hn;

    hn.data = GetHeapData(dist,place_id);
    hn.value = dist;

    return hn;
}

PQ *initQueue(long int n) {
   PQ *q;

   q = (PQ *)malloc(sizeof(PQ));
   q->size = 0;
   q->heap = (heapNode*)malloc(sizeof(heapNode)*(n+1));

   long int i;
   for(i = 0; i < n+1; i++) {
        q->heap[i].value = ARC_INF;
        q->heap[i].data = GetHeapData(ARC_INF,ARC_INF);
   }

   return(q);
}

short isQueueEmpty(PQ *q) {
    return(q->size == 0);
}

void freeQueue(PQ *q) {
    free(q->heap);
    free(q);
}

void SWAP(heapNode *vector,long int i,long int j) {
    heapNode temp = vector[i];
    vector[i] = vector[j];
    vector[j] = temp;
}

void min_heapify(heapNode *vector,long int size,long int parent) {
    long int left = 2*parent, right = 2*parent + 1;
    long int lowest;

    if(left <= size && vector[left].value < vector[parent].value) {
        lowest = left;
    }
    else {
        lowest = parent;
    }

    if(right <= size && vector[right].value < vector[lowest].value) {
        lowest = right;
    }

    if(lowest != parent) {
        SWAP(vector,parent,lowest);
        min_heapify(vector,size,lowest);
    }
}

void build_min(heapNode *vector,long int size) {
    long int i;
    for(i = size / 2; i > 0; i--) {
        min_heapify(vector,size,i);
    }
}

void remakeHeap(heapNode *vector,long int size) {
    build_min(vector,size);
}

void updateQueueElement(long int new_dist,long int place_id,PQ *q) {
    long int i;
    for(i = 1; i <= q->size; i++) {
        if(place_id == q->heap[i].data.place_id) {
            q->heap[i].data.place_dist = new_dist;
            q->heap[i].value = new_dist;

            remakeHeap(q->heap,q->size);
            break;
        }
    }
}

void dumpQueue(PQ *q) {
    long int i;

    printf("Queued %ld elements\n\n",q->size);
    for(i = 1; i <= q->size; i++) {
        printf("dist:%ld id:%ld\n",q->heap[i].data.place_dist,q->heap[i].data.place_id);
    }
}

// ----------------------------------------------------

// ---------------- HASHSET V1.0 ----------------------

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

inline unsigned int hashset_get_count(HashSet *hs) {
    return(hs->count);
}

unsigned int hashset_maptable(HashSet *hs, int item) {
    return(item % hs->length);
}

unsigned int hashset_slot(HashSet *hs, int item, unsigned int *b) {
    unsigned int bucket = hashset_maptable(hs, item);
    if(b != NULL) *b = bucket;

    unsigned int i;
    for(i = 0; i < hs->table[bucket]->count; i++) {
        if(hs->table[bucket]->list[i] == item)
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

// ----------------------------------------------------

// ------------------ GRAPH V1.0 ----------------------

#define GRAPH_INF 1000000000
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
    int **min_cost;
    unsigned int **min_path;
    unsigned int *user_path;    // path queried by the user

    unsigned int length;
    short using_unique_links;
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
    graph->min_cost = NULL;
    graph->min_path = NULL;
    graph->user_path = NULL;

    graph->length = len;
    graph->using_unique_links = 0;

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

    if(graph->min_cost != NULL) {
        for(i = 0; i < graph->length; i++) {
            free(graph->min_cost[i]);
            free(graph->min_path[i]);
        }

        free(graph->min_cost);
        free(graph->min_path);
        free(graph->user_path);
    }

    free(graph->nodes);
    free(graph);
}

NodeGraph* graph_get_node_pointer(GraphTAD *graph, unsigned int id) {
    return(graph->nodes[id]);
}

unsigned int graph_get_node_id(NodeGraph *node) {
    return(node->id);
}

unsigned int graph_get_node_value(NodeGraph *node) {
    return(node->value);
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
        graph->using_unique_links = 1;

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

unsigned int graph_bsearch(GraphTAD *graph, unsigned int node, unsigned int key, int *weight) {
    int st = 0, en = graph->nodes[node]->links_len - 1, slot;

    while (st <= en) {
        slot = (st + en) / 2;
        if (key == graph_get_node_id(graph->nodes[node]->links[slot]->node)) {
            *weight = graph->nodes[node]->links[slot]->weight;
            return(slot);
        }

        if (key < graph_get_node_id(graph->nodes[node]->links[slot]->node)) en = slot - 1;
        else st = slot + 1;
    }

    *weight = GRAPH_INF;
    return -1;   // not found
}

short graph_have_node_link(GraphTAD *graph, unsigned int from, unsigned int to, int *weight) {
    return(graph_bsearch(graph, from, to, weight) != -1);
}

unsigned int graph_bsearch_unique(GraphTAD *graph, unsigned int node, unsigned int key, int *weight) {
    int st = 0, en = graph->nodes[node]->unique_links_len - 1, slot;

    while (st <= en) {
        slot = (st + en) / 2;
        if (key == graph_get_node_id(graph->nodes[node]->unique_links[slot]->node)) {
            *weight = graph->nodes[node]->unique_links[slot]->weight;
            return(slot);
        }

        if (key < graph_get_node_id(graph->nodes[node]->unique_links[slot]->node)) en = slot - 1;
        else st = slot + 1;
    }

    *weight = GRAPH_INF;
    return -1;   // not found
}

short graph_have_node_link_unique(GraphTAD *graph, unsigned int from, unsigned int to, int *weight) {
    return(graph_bsearch_unique(graph, from, to, weight) != -1);
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

void graph_generate_shortest_ranges_from_node(GraphTAD *graph,unsigned int start) {
    if(graph->length <= 0) return;

    // uses Dijkstra's algorithm to query distances from the starting point for every node

    // minimal distances encountered will be stored at a node's VALUE component.
    // path trace for the given minimal distance will be stored at a node's FLAG component.

    PQ *frontier = initQueue(graph->length);
    HashSet *discovered = hashset_create();

    unsigned int i;
    for(i = 0; i < graph->length; i++) {
        enqueue(GetHeapNode(GRAPH_INF,i),frontier);
        graph_set_node_value(graph,i,GRAPH_INF);
        graph_set_node_flag(graph,i,-1);
    }

    updateQueueElement(0,start,frontier);
    graph_set_node_value(graph,start,0);

    while(!isQueueEmpty(frontier)) {
        heapNode step = dequeue(frontier);
        hashset_insert(discovered,step.data.place_id);

        if(step.data.place_dist >= GRAPH_INF) continue;     // no use to try to compute minimal values from this step since it's current distance is infinite.

        for(i = 0; i < graph->nodes[step.data.place_id]->unique_links_len; i++) {
            unsigned int neighbor = graph_get_node_id(graph->nodes[step.data.place_id]->unique_links[i]->node);
            if(!hashset_contains(discovered,neighbor,NULL)) {
                unsigned int new_cost = step.data.place_dist + graph->nodes[step.data.place_id]->unique_links[i]->weight;

                if(new_cost < graph_get_node_value(graph->nodes[neighbor])) {
                    graph_set_node_value(graph,neighbor,new_cost);
                    graph_set_node_flag(graph,neighbor,step.data.place_id);

                    updateQueueElement(new_cost,neighbor,frontier);
                }
            }
        }
    }

    hashset_destroy(discovered);
    freeQueue(frontier);
}

void graph_generate_shortest_paths(GraphTAD *graph) {
    // uses Floyd-Warshall algorithm for calculating shortest paths between every pair of nodes

    // will register minimal cost for a route between two nodes in the min_cost[from][to] matrix system.
    // will register next node of minimal cost for a route between two nodes in the min_path[from][to] matrix system (used within graph_get_shortest_path_from_two_nodes).

    if(graph->using_unique_links) {
        graph->using_unique_links = 2;

        unsigned int i, j, k;
        int cost;

        if(graph->min_cost == NULL) {
            graph->min_cost = (int **)malloc(graph->length * sizeof(int *));
            for(i = 0; i < graph->length; i++)
                graph->min_cost[i] = (int *)malloc(graph->length * sizeof(int));

            graph->min_path = (unsigned int **)malloc(graph->length * sizeof(unsigned int *));
            for(i = 0; i < graph->length; i++)
                graph->min_path[i] = (unsigned int *)malloc(graph->length * sizeof(unsigned int));

            graph->user_path = (unsigned int *)malloc((graph->length + 1) * sizeof(unsigned int));
        }

        for(i = 0; i < graph->length; i++) {
            for(j = 0; j < graph->length; j++) {
                graph_bsearch_unique(graph,i,j,&cost);
                graph->min_cost[i][j] = cost;
                graph->min_path[i][j] = (cost < GRAPH_INF ? j : -1);
            }
        }

        // node can reach self with no cost whatsoever
        for(i = 0; i < graph->length; i++) {
            graph->min_cost[i][i] = 0;
            graph->min_path[i][i] = -1;
        }

        for(k = 0; k < graph->length; k++) {
            for(i = 0; i < graph->length; i++) {
                for(j = 0; j < graph->length; j++) {
                    int new_dist = graph->min_cost[i][k] + graph->min_cost[k][j];
                    if(graph->min_cost[i][j] > new_dist) {
                        graph->min_cost[i][j] = new_dist;
                        graph->min_path[i][j] = graph->min_path[i][k];
                    }
                }
            }
        }
    }
}

unsigned int *graph_get_shortest_path_from_two_nodes(GraphTAD *graph,unsigned int from,unsigned int to) {
    // DO NOT simply FREE the vector returned by this function after use, it will be freed along when freeing the graph structure.
    // REQUIRES graph_generate_shortest_paths call beforehand.

    // returns a list containing nodes used on the minimal cost path between the given two nodes

    if(graph->using_unique_links == 2) {
        unsigned int i = 0, u;

        if(graph->min_path[from][to] != -1) {
            graph->user_path[0] = from;

            u = from;
            i = 1;
            while(u != to) {
                u = graph->min_path[u][to];
                graph->user_path[i] = u;
                i++;
            }
        }

        graph->user_path[i] = -1;
        return(graph->user_path);
    }

    return(NULL);
}

// ----------------------------------------------------

#define STR_LEN 100

GraphTAD *generate_mice_maze(int *exit_cell,int *timer) {
    char str[STR_LEN + 1],*tok;

    gets(str);
    int nodes = atoi(str);
    if(nodes <= 0) return(NULL);

    GraphTAD *graph = graph_create(nodes + 1);

    gets(str);
    *exit_cell = atoi(str);

    gets(str);
    *timer = atoi(str);

    gets(str);
    int connections = atoi(str);

    int i,n1,n2,p;
    for(i = 0; i < connections; i++) {
        gets(str);

        tok = strtok(str," ");
        n1 = atoi(tok);

        tok = strtok(NULL," ");
        n2 = atoi(tok);

        tok = strtok(NULL," ");
        p = atoi(tok);

        graph_set_node_link(graph,n1,n2,p);
    }

    graph_commit(graph,GRAPH_UNIQUE_LINKS);
    return(graph);
}

int run_mice_maze(GraphTAD *graph,int exit_cell,int timer) {
    graph_generate_shortest_paths(graph);

    int count = 0, i;
    for(i = 0; i < graph->length; i++) {
        if(graph->min_cost[i][exit_cell] <= timer) {
            count++;
        }
    }

    return(count);
}

int main() {
    int nodes, exit_cell, timer;
    GraphTAD *graph = generate_mice_maze(&exit_cell,&timer);

    if(graph != NULL) {
        printf("%d",run_mice_maze(graph,exit_cell,timer));
        graph_destroy(graph);
    }
    else {
        printf("0");
    }

    return 0;
}
