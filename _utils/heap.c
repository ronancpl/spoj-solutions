#include <stdio.h>
#include <stdlib.h>

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

/*
    PRIORITY QUEUE:

    PQ *initQueue(long int n);
    heapNode GetHeapNode(long int dist,long int place_id);
    short isQueueEmpty(PQ *q);
    heapNode dequeue(PQ *q);
    void enqueue(heapNode node, PQ *q);
    void freeQueue(PQ *q);
    void updateQueueElement(long int place_id,long int new_dist,PQ *q);
*/

int main() {
    PQ *q = initQueue(4);   //max 4 items for this heap.

    heapNode hn1 = GetHeapNode(4,5);    // first parameter is the priority of a key/node, second is it's id.
    heapNode hn2 = GetHeapNode(2,2);    // nodes with lower first parameter has higher priority for this priority queue.
    heapNode hn3 = GetHeapNode(3,4);

    enqueue(hn1, q);        // will stack overflow if too much items are inserted.
    enqueue(hn2, q);
    enqueue(hn3, q);

    dumpQueue(q);

    updateQueueElement(1,5,q);

    dumpQueue(q);

    heapNode dq = dequeue(q);
    printf("Dequeued Item: %ld, cost: %ld\n",dq.data.place_id,dq.data.place_dist);

    freeQueue(q);
    return 0;
}
