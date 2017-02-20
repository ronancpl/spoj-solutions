#include <stdio.h>
#include <stdlib.h>

#define NUM 20

#define B_STACK_INIT 20
#define B_STACK_CACHE 2

#define B_NODES_MIN 2
#define B_NODES_MAX 4

typedef struct B_Item* B_Node;
typedef struct B_Item {
    unsigned int key;
} B_Item;

typedef struct B_Tree* B_Pointer;
typedef struct B_Tree {
    short n;

    //one empty slot, for split detection
    B_Node item[B_NODES_MAX + 1];
    B_Pointer child[B_NODES_MAX + 2];
} B_Tree;

typedef struct {
    B_Pointer *val;
    short *place;

    unsigned int cache;
    unsigned int len;
} B_PathStack;

void btree_dump(B_Pointer tree, int id) {
    if(tree == NULL) return;

    printf("\n%d -> (%d) ", id, tree->n);

    short i;
    for(i = 0; i < tree->n; i++)
        printf(" %d ", tree->item[i]->key);
    printf("\n");

    for(i = 0; i <= tree->n; i++) {
        btree_dump(tree->child[i], (id * 10) + i);
    }
}

void btree_branch_print(B_Pointer ptr) {
    if(ptr != NULL) {
        short i;
        for(i = 0; i < ptr->n; i++) {
            printf("%d ", ptr->item[i]->key);
        }
    }
    else printf("NULL");

    printf("\n");
}

void btree_stack_dump(B_PathStack *bps) {
    unsigned int i;
    for(i = 0; i < bps->len; i++) {
        btree_branch_print(bps->val[i]);
    }
}

B_PathStack* btree_stack_init() {
    B_PathStack *bps = (B_PathStack *)malloc(sizeof(B_PathStack));

    bps->val = (B_Pointer *)malloc(B_STACK_INIT * sizeof(B_Pointer));
    bps->place = (short *)malloc(B_STACK_INIT * sizeof(short));
    bps->cache = B_STACK_INIT;

    //first element on the track is the root, therefore it's parent is NULL.
    bps->val[0] = NULL;
    bps->place[0] = -1;
    bps->len = 1;

    return(bps);
}

void btree_stack_free(B_PathStack *bps) {
    free(bps->place);
    free(bps->val);
    free(bps);
}

void btree_stack_push(B_PathStack *bps, B_Pointer ptr, short pl) {
    if(bps->len == bps->cache) {
        bps->cache *= B_STACK_CACHE;
        bps->val = (B_Pointer *)realloc(bps->val, bps->cache * sizeof(B_Pointer));
        bps->place = (short *)realloc(bps->place, bps->cache * sizeof(short));
    }

    bps->val[bps->len] = ptr;
    bps->place[bps->len] = pl;
    (bps->len)++;
}

B_Pointer btree_stack_pop(B_PathStack *bps, short *pl) {
    (bps->len)--;
    *pl = bps->place[bps->len];
    return(bps->val[bps->len]);
}

void btree_init(B_Pointer *tree) {
    *tree = NULL;
}

B_Node btree_create_node(unsigned int key) {
    B_Node node = (B_Node)malloc(sizeof(B_Item));
    node->key = key;

    return(node);
}

void btree_delete_node(B_Node node) {
    free(node);
}

short btree_find(B_Node query, B_Pointer ptr, B_Node *ret) {
    //if node reaches null, query KEY could not be found
    if(ptr == NULL) {
        *ret = NULL;
        return(0);
    }

    short i = 0;

    //iterate over all pointers of this node
    while(i < ptr->n && query->key > ptr->item[i]->key) i++;

    if(i == ptr->n) {
        //key greater than any nodes on this page
        return(btree_find(query, ptr->child[i], ret));
    }

    //evaluates if the key has been found
    if(query->key == ptr->item[i]->key) {
        *ret = ptr->item[i];
        return(1);
    }

    return(btree_find(query, ptr->child[i], ret));
}

B_Pointer btree_branch_create(B_Pointer from, short st, short en) {
    B_Pointer node = (B_Pointer)malloc(sizeof(B_Tree));
    node->n = 0;

    if(from != NULL) {
        for(; st < en; st++, (node->n)++) {
            node->item[node->n] = from->item[st];
            node->child[node->n] = from->child[st];
        }

        node->child[node->n] = from->child[st];
    }
    else {
        node->child[0] = NULL;
    }

    return(node);
}

void btree_branch_delete(B_Pointer node) {
    free(node);
}

short btree_node_execute_split(B_Pointer parent, B_Pointer split, short slot) {
    if(parent == NULL) {
        //divide subtree
        short mid = split->n / 2;

        B_Pointer lft = btree_branch_create(split, 0, mid);
        B_Pointer rgt = btree_branch_create(split, mid + 1, split->n);

        split->n = 1;
        split->item[0] = split->item[mid];
        split->child[0] = lft;
        split->child[1] = rgt;

        return(0);
    }

    short v;

    //move pointers to right
    for(v = parent->n; v > slot; v--)
        parent->child[v + 1] = parent->child[v];

    //move items to right
    for(v = parent->n; v > slot; v--)
        parent->item[v] = parent->item[v - 1];

    //divide subtree
    short mid = split->n / 2;

    parent->item[slot] = split->item[mid];
    parent->child[slot + 1] = btree_branch_create(split, mid + 1, split->n);
    //parent->child[slot] = split BY DEFAULT, not needed to state it again!

    split->n = mid;
    (parent->n)++;

    return(parent->n > B_NODES_MAX);
}

short btree_insert_leaf_node(B_Pointer recipient, B_Node node, short slot) {
    short i;

    //not really needed to use bsearch anyways
    for(i = recipient->n; i > slot; i--)
        recipient->item[i] = recipient->item[i - 1];

    recipient->item[slot] = node;

    (recipient->n)++;
    recipient->child[recipient->n] = NULL;

    return(recipient->n > B_NODES_MAX);
}

void btree_insert(B_Node node, B_Pointer *root) {
    if(*root == NULL) {
        //root is empty, simply insert this item
        *root = btree_branch_create(NULL, -1, -1);
        btree_insert_leaf_node(*root, node, 0);
        return;
    }

    B_PathStack *bps = btree_stack_init();

    B_Pointer ptr = *root;
    while(ptr != NULL) {
        short i = 0;
        while(i < ptr->n && node->key > ptr->item[i]->key) i++;

        btree_stack_push(bps, ptr, i);
        ptr = ptr->child[i];
    }

    //bring the node which item can be inserted
    short slot;
    ptr = btree_stack_pop(bps, &slot);

    if(btree_insert_leaf_node(ptr, node, slot)) {
        B_Pointer split;

        do {
            split = ptr;
            ptr = btree_stack_pop(bps, &slot);
        } while(btree_node_execute_split(ptr, split, slot));
    }

    btree_stack_free(bps);
}

short btree_is_leaf_node(B_Pointer ptr) {
    return(ptr->child[0] == NULL);
}

B_Node btree_remove_item_node_left(B_Pointer root, short pos, B_Pointer *off) {
    B_Node item = root->item[pos];
    *off = root->child[pos];
    (root->n)--;

    short i;
    for(i = pos; i < root->n; i++) {
        root->item[i] = root->item[i + 1];
        root->child[i] = root->child[i + 1];
    }
    root->child[i] = root->child[i + 1];

    return(item);
}

B_Node btree_remove_item_node_right(B_Pointer root, short pos, B_Pointer *off) {
    B_Node item = root->item[pos];
    *off = root->child[pos + 1];
    (root->n)--;

    short i;
    for(i = pos; i < root->n; i++) {
        root->item[i] = root->item[i + 1];
        root->child[i + 1] = root->child[i + 2];
    }

    return(item);
}

void btree_insert_item_node_left(B_Pointer root, B_Node node, B_Pointer off, short pos) {
    short i;
    for(i = root->n; i > pos; i--) {
        root->item[i] = root->item[i - 1];
        root->child[i + 1] = root->child[i];
    }
    root->child[i + 1] = root->child[i];

    root->item[pos] = node;
    root->child[pos] = off;
    (root->n)++;
}

void btree_insert_item_node_right(B_Pointer root, B_Node node, B_Pointer off, short pos) {
    short i;
    for(i = root->n; i > pos; i--) {
        root->item[i] = root->item[i - 1];
        root->child[i + 1] = root->child[i];
    }

    root->item[pos] = node;
    root->child[pos + 1] = off;
    (root->n)++;
}

void btree_merge_nodes(B_Node mid, B_Pointer first, B_Pointer second) {
    //merge 'first' and 'second', with 'mid' as intersecting node; results at the merged 'first'
    first->item[first->n] = mid;

    short i = first->n + 1, j;
    for(j = 0; j < second->n; j++) {
        first->item[i + j] = second->item[j];
        first->child[i + j] = second->child[j];
    }
    first->child[i + j] = second->child[j];

    first->n += (second->n + 1);
    btree_branch_delete(second);
}

void btree_swap_item_nodes(B_Pointer parent, short p, B_Pointer node, short n) {
    B_Node temp = parent->item[p];
    parent->item[p] = node->item[n];
    node->item[n] = temp;
}

B_Pointer btree_get_branch(B_Pointer node, short pos) {
    return(node->child[pos]);
}

void btree_set_branch(B_Pointer node, short pos, B_Pointer branch) {
    node->child[pos] = branch;
}

void btree_rebalance(B_PathStack *bps, B_Pointer parent, short parent_i, B_Pointer leaf, B_Pointer *root) {
    B_Pointer par = parent, ptr = leaf;
    short par_i = parent_i;

    while(par != NULL) {
        if(ptr->n > B_NODES_MAX) {
            btree_node_execute_split(par, ptr, par_i);
        }

        else if(ptr->n < B_NODES_MIN) {
            B_Pointer off;

            if(par_i > 0 && par->child[par_i - 1]->n > B_NODES_MIN) {
                B_Node swap = btree_remove_item_node_right(par->child[par_i - 1], par->child[par_i - 1]->n - 1, &off);
                btree_insert_item_node_left(par->child[par_i], par->item[par_i - 1], off, 0);
                par->item[par_i - 1] = swap;
            }
            else if(par_i < par->n && par->child[par_i + 1]->n > B_NODES_MIN) {
                B_Node swap = btree_remove_item_node_left(par->child[par_i + 1], 0, &off);
                btree_insert_item_node_right(par->child[par_i], par->item[par_i], off, par->child[par_i]->n);
                par->item[par_i] = swap;
            }

            else {
                if(par_i - 1 >= 0) {
                    B_Pointer first = par->child[par_i - 1], second = par->child[par_i];
                    B_Node mid = btree_remove_item_node_right(par, par_i - 1, &off);
                    btree_merge_nodes(mid, first, second);
                }

                else {
                    B_Pointer first = par->child[par_i], second = par->child[par_i + 1];
                    B_Node mid = btree_remove_item_node_right(par, par_i, &off);
                    btree_merge_nodes(mid, first, second);
                }
            }
        }

        ptr = par;
        par = btree_stack_pop(bps, &par_i);
    }

    if((*root)->n > B_NODES_MAX) {
        btree_node_execute_split(NULL, *root, -1);
    }
    else {
        //move root node inwards while root is found empty
        while((*root)->n == 0) {
            if((*root)->child[0] == NULL) {
                btree_branch_delete(*root);
                *root = NULL;
                break;
            }

            ptr = *root;
            *root = (*root)->child[0];
            btree_branch_delete(ptr);
        }
    }
}

short btree_remove(B_Node node, B_Pointer *root) {
    short i, j, par_i;
    B_Pointer ptr = *root, par = NULL, off;

    B_PathStack *bps = btree_stack_init();
    btree_stack_push(bps, NULL, -1);

    //reach node with the key
    while(ptr != NULL) {
        i = 0;
        while(i < ptr->n && node->key > ptr->item[i]->key) i++;

        if(i < ptr->n && node->key == ptr->item[i]->key) {
            //reached key value to be removed
            break;
        }

        btree_stack_push(bps, ptr, i);
        ptr = ptr->child[i];
    }

    if(ptr == NULL) {
        btree_stack_free(bps);
        return(0);
    }

    //pass the target node down the tree until it reaches a leaf node
    while(!btree_is_leaf_node(ptr)) {
        if(ptr->child[i]->n > B_NODES_MIN) {
            j = ptr->child[i]->n - 1;
            btree_swap_item_nodes(ptr, i, ptr->child[i], j);

            btree_stack_push(bps, ptr, i);
            ptr = ptr->child[i];
        }

        else if(ptr->child[i + 1]->n > B_NODES_MIN) {
            j = 0;
            btree_swap_item_nodes(ptr, i, ptr->child[i + 1], j);

            btree_stack_push(bps, ptr, i + 1);
            ptr = ptr->child[i + 1];
        }

        else {
            j = ptr->child[i]->n;
            B_Pointer first = ptr->child[i], second = ptr->child[i + 1];

            B_Node mid = btree_remove_item_node_right(ptr, i, &off);
            btree_merge_nodes(mid, first, second);

            btree_stack_push(bps, ptr, i);
            ptr = ptr->child[i];
        }

        i = j;
    }

    //loads the parent node
    par = btree_stack_pop(bps, &par_i);

    if(par != NULL && ptr->n <= B_NODES_MIN) {
        if(par_i > 0 && par->child[par_i - 1]->n > B_NODES_MIN) {
            B_Node swap = btree_remove_item_node_right(par->child[par_i - 1], par->child[par_i - 1]->n - 1, &off);
            btree_insert_item_node_left(ptr, par->item[par_i - 1], off, 0);

            par->item[par_i - 1] = swap;
            i++;
        }
        else if(par_i < par->n && par->child[par_i + 1]->n > B_NODES_MIN) {
            B_Node swap = btree_remove_item_node_left(par->child[par_i + 1], 0, &off);
            btree_insert_item_node_right(ptr, par->item[par_i], off, ptr->n);

            par->item[par_i] = swap;
        }
        else {
            if(par_i - 1 >= 0) {
                B_Pointer first = par->child[par_i - 1], second = par->child[par_i];
                B_Node mid = btree_remove_item_node_right(par, par_i - 1, &off);

                i += (first->n + 1);
                btree_merge_nodes(mid, first, second);
                ptr = par->child[par_i - 1];
            }

            else {
                B_Pointer first = par->child[par_i], second = par->child[par_i + 1];
                B_Node mid = btree_remove_item_node_right(par, par_i, &off);

                btree_merge_nodes(mid, first, second);
                //ptr = par->child[par_i] BY DEFAULT, not needed to state it again!
            }
        }
    }

    btree_remove_item_node_right(ptr, i, &off);
    btree_rebalance(bps, par, par_i, ptr, root);

    btree_stack_free(bps);
    return(1);
}

void btree_delete_tree(B_Pointer root) {
    if(root == NULL) return;

    short i;
    for(i = 0; i < root->n; i++) {
        btree_delete_node(root->item[i]);
        btree_delete_tree(root->child[i]);
    }
    btree_delete_tree(root->child[i]);

    btree_branch_delete(root);
}

void btree_delete(B_Pointer *root) {
    btree_delete_tree(*root);
    *root = NULL;
}

void btree_set_key_to_query(B_Node query, unsigned int key) {
    query->key = key;
}

/*
    BTREE:

    void btree_init(B_Pointer *tree);
    void btree_dump(B_Pointer tree, 1);
    short btree_find(B_Node query, B_Pointer ptr, B_Node *ret);
    void btree_insert(B_Node node, B_Pointer *root);
    short btree_remove(B_Node node, B_Pointer *root);
    void btree_delete(B_Pointer *root);
    void btree_set_key_to_query(B_Node query, unsigned int key);
*/

int main() {
    int list[NUM] = {14, 25, 48, 65, 77, 777, 7777, 670, 832, 843, 128, 9993, 33310, 32104, 1, -23, -14752, -123, -1445, 0};
    B_Pointer tree;
    B_Item query;

    B_Node ret;

    btree_init(&tree);

    int i;
    for(i = 0; i < NUM; i++)
        btree_insert(btree_create_node(list[i]), &tree);

    for(i = NUM - 1; i >= 0; i--) {
        btree_set_key_to_query(&query, i);
        if(!btree_find(&query, tree, &ret)) {
            printf("NOPE at %d\n", i);
        }
    }

    for(i = NUM - 1; i >= 0; i--) {
        btree_set_key_to_query(&query, list[i]);
        if(!btree_remove(&query, &tree)) {
            printf("CRITICAL remove error at %d\n", i);
            //btree_dump(tree, 1);
            system("pause");
        }
    }

    btree_insert(btree_create_node(7778), &tree);
    btree_set_key_to_query(&query, 7778);
    if(!btree_find(&query, tree, &ret)) {
        printf("NOPE at %d\n", 7778);
    }

    btree_delete(&tree);

    return 0;
}

