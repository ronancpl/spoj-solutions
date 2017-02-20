#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR 20
#define DOMINO_TOPVAL 7
#define TABLE_HEIGHT 7
#define TABLE_LENGTH 8

void get_order(short *v1, short *v2) {
    if(*v1 > *v2) {
        *v1 ^= *v2;
        *v2 ^= *v1;
        *v1 ^= *v2;
    }
}

short get_piece_id(short x, short y) {
    get_order(&x, &y);

    short id = 0, i;
    for(i = 0; i < x; i++)
        id += (DOMINO_TOPVAL - i);

    id += (y - x);
    return(id);
}

short is_piece_used(short *piece_list, short id) {
    return(piece_list[id] != 0);
}

void using_piece(short *piece_list, short id, short use) {
    piece_list[id] = use;
}

short is_table_slot_used(short **table, short px, short py) {
    return(table[px][py] > -1);
}

void color_table_slot(short **table, short px, short py, short piece_id) {
    table[px][py] = piece_id;
}

short get_table_map_value(short **table_map, short px, short py) {
    return(table_map[px][py]);
}

short get_pieces_quantity() {
    short count = 0, i;
    for(i = 0; i < TABLE_HEIGHT; i++)
        count += (DOMINO_TOPVAL - i);

    return(count);
}

short all_pieces_used(short *piece_list) {
    short i;

    for(i = 0; i < get_pieces_quantity(); i++)
        if(!is_piece_used(piece_list, i)) return(0);

    return(1);
}

void get_domino_configuration(short *count, short **table_map, short **table, short *piece_list, short state) {
    short px = state / TABLE_LENGTH, py = state % TABLE_LENGTH;

    if(px >= TABLE_HEIGHT) {
        if(all_pieces_used(piece_list)) (*count)++;
        return;
    }

    if(is_table_slot_used(table, px, py)) {
        get_domino_configuration(count, table_map, table, piece_list, state + 1);
        return;
    }

    short v1 = get_table_map_value(table_map, px, py);

    if(px + 1 < TABLE_HEIGHT) {
        if(!is_table_slot_used(table, px + 1, py)) {
            short v2 = get_table_map_value(table_map, px + 1, py);
            short piece_id = get_piece_id(v1, v2);

            if(!is_piece_used(piece_list, piece_id)) {
                // color vertical piece
                color_table_slot(table, px, py, piece_id);
                color_table_slot(table, px + 1, py, piece_id);
                using_piece(piece_list, piece_id, 1);

                get_domino_configuration(count, table_map, table, piece_list, state + 1);

                //uncolor piece
                color_table_slot(table, px, py, -1);
                color_table_slot(table, px + 1, py, -1);
                using_piece(piece_list, piece_id, 0);
            }
        }
    }

    if(py + 1 < TABLE_LENGTH) {
        if(!is_table_slot_used(table, px, py + 1)) {
            short v2 = get_table_map_value(table_map, px, py + 1);
            short piece_id = get_piece_id(v1, v2);

            if(!is_piece_used(piece_list, piece_id)) {
                //color horizontal piece
                color_table_slot(table, px, py, piece_id);
                color_table_slot(table, px, py + 1, piece_id);
                using_piece(piece_list, piece_id, 1);

                get_domino_configuration(count, table_map, table, piece_list, state + 1);

                //uncolor piece
                color_table_slot(table, px, py, -1);
                color_table_slot(table, px, py + 1, -1);
                using_piece(piece_list, piece_id, 0);
            }
        }
    }
}

short** init_table() {
    short i, j;

    short **table = (short **)malloc(TABLE_HEIGHT * sizeof(short *));
    for(i = 0; i < TABLE_HEIGHT; i++) {
        table[i] = (short *)malloc(TABLE_LENGTH * sizeof(short));

        for(j = 0; j < TABLE_LENGTH; j++)
            table[i][j] = -1;
    }

    return(table);
}

void destroy_table(short **table) {
    short i;
    for(i = 0; i < TABLE_HEIGHT; i++)
        free(table[i]);

    free(table);
}

void init_structures(short ***t, short **pl) {
    short **table = init_table();

    short qty = get_pieces_quantity(), i, j;
    short *piece_list = (short *)malloc(qty * sizeof(short));
    for(i = 0; i < qty; i++)
        piece_list[i] = 0;

    *t = table;
    *pl = piece_list;
}

void destroy_structures(short **table, short *piece_list) {
    destroy_table(table);
    free(piece_list);
}

short get_number_domino_mappings(short **table_map) {
    short count = 0;
    short **table, *piece_list;

    init_structures(&table, &piece_list);
    get_domino_configuration(&count, table_map, table, piece_list, 0);
    destroy_structures(table, piece_list);

    return(count);
}

void define_table_map(char *str, short **table_map) {
    short i, j;
    char *tok;

    for(i = 0; i < TABLE_HEIGHT; i++) {
        gets(str);
        tok = strtok(str, " ");

        for(j = 0; j < TABLE_LENGTH; j++) {
            table_map[i][j] = atoi(tok);
            tok = strtok(NULL, " ");
        }
    }
}

void define_table_map_from_file(char *str, short **table_map, FILE *f) {
    short i, j;
    char *tok;

    for(i = 0; i < TABLE_HEIGHT; i++) {
        fgets(str, MAX_STR - 1, f);
        tok = strtok(str, " ");

        for(j = 0; j < TABLE_LENGTH; j++) {
            table_map[i][j] = atoi(tok);
            tok = strtok(NULL, " ");
        }
    }
}

int main() {
    short instances, i;
    char str[MAX_STR], *tok;
    short **table_map;
    //FILE *f = fopen("input.txt", "r+t");

    //fgets(str, MAX_STR - 1, f);
    gets(str);

    tok = strtok(str, " ");
    instances = atoi(tok);

    table_map = init_table();

    for(i = 1; i <= instances; i++) {
        //define_table_map_from_file(str, table_map, f);
        define_table_map(str, table_map);

        printf("Teste %d\n%d\n\n", i, get_number_domino_mappings(table_map));
    }

    destroy_table(table_map);
    //fclose(f);

    return 0;
}
