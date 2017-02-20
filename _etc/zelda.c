#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OUTSTR 2200
#define MAX_STR 50
#define MAX_NAME 20

enum {
    SEG_INTERSECT=0,
    SEG_MISS,
    SEG_PARALLEL,
    SEG_COLLINEAR
};

typedef struct {
    int x;
    int y;

    int inc_x;
    int inc_y;
} Line;

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} Rectangle;

typedef struct {
    Rectangle geometry;
    char name[MAX_NAME + 1];
} Temple;

int get_cross_product(int x1, int y1, int x2, int y2) {
    return((x1 * y2) - (x2 * y1));
}

short line_border_intersection(int line_x, int line_y, int line_dx, int line_dy, int border_x, int border_y, int border_dx, int border_dy) {
    int r_vec_s = get_cross_product(border_dx, border_dy, line_dx, line_dy);

    int q_minus_p_x = line_x - border_x;
    int q_minus_p_y = line_y - border_y;

    int q_minus_p_vec_s = get_cross_product(q_minus_p_x, q_minus_p_y, line_dx, line_dy);

    if(r_vec_s == 0) {
        if(q_minus_p_vec_s == 0) return(SEG_COLLINEAR);
        else return(SEG_PARALLEL);
    }

    float t = (float)q_minus_p_vec_s / (float)r_vec_s;

    int q_minus_p_vec_r = get_cross_product(q_minus_p_x, q_minus_p_y, border_dx, border_dy);
    float u = (float)q_minus_p_vec_r / (float)r_vec_s;

    //u max value is whatever, it is a full directed line description after all
    if(t >= 0.0 && t <= 1.0 && u >= 0.0) return(SEG_INTERSECT);
    return(SEG_MISS);
}

short line_intersects_rectangle(Line line, Rectangle rect) {
    int inc_x = rect.x2 - rect.x1;
    int inc_y = rect.y2 - rect.y1;

    if(!line_border_intersection(line.x, line.y, line.inc_x, line.inc_y, rect.x1, rect.y1, inc_x, 0)) return(1);
    if(!line_border_intersection(line.x, line.y, line.inc_x, line.inc_y, rect.x1, rect.y2, inc_x, 0)) return(1);
    if(!line_border_intersection(line.x, line.y, line.inc_x, line.inc_y, rect.x1, rect.y1, 0, inc_y)) return(1);
    if(!line_border_intersection(line.x, line.y, line.inc_x, line.inc_y, rect.x2, rect.y1, 0, inc_y)) return(1);

    return(0);
}

short is_temple_on_path(Line hero, Temple temple) {
    return(line_intersects_rectangle(hero, temple.geometry));
}

void print_on_output_string(char *out_str, int *out_str_cursor, char *name) {
    //space, providing initial output string is filled with whitespace
    (*out_str_cursor)++;

    int name_len = strlen(name), i;
    for(i = 0; i < name_len; i++)
        out_str[*out_str_cursor + i] = name[i];

    (*out_str_cursor) += name_len;
}

void init_output_string(char *out_str, int *out_str_cursor) {
    int i;
    for(i = 0; i <= *out_str_cursor; i++)
        out_str[i] = ' ';

    *out_str_cursor = -1;
}

void execute_dowsing_skill(char *out_str, int *out_str_cursor, Line hero, Temple *tlist, int tlen) {
    init_output_string(out_str, out_str_cursor);

    int i;
    for(i = 0; i < tlen; i++) {
        if(is_temple_on_path(hero, tlist[i]))
            print_on_output_string(out_str, out_str_cursor, tlist[i].name);
    }

    if(*out_str_cursor > 0) out_str[*out_str_cursor] = 0;
    else out_str[0] = 0;

    printf("%s\n", out_str);
}

Temple get_temple(char *str) {
    char *tok;
    Temple t;

    gets(str);

    tok = strtok(str, " ");
    strncpy(t.name, tok, MAX_NAME);

    tok = strtok(NULL, " ");
    t.geometry.x1 = atoi(tok);

    tok = strtok(NULL, " ");
    t.geometry.y1 = atoi(tok);

    tok = strtok(NULL, " ");
    t.geometry.x2 = atoi(tok);

    tok = strtok(NULL, " ");
    t.geometry.y2 = atoi(tok);

    return(t);
}

int main() {
    char str[MAX_STR], *tok;

    Line hero;
    Temple *temple_list;
    int temple_len;

    char out_str[MAX_OUTSTR + 1];
    int out_str_cursor = MAX_OUTSTR;

    while(1) {
        gets(str);
        tok = strtok(str, " ");

        temple_len = atoi(tok);
        if(temple_len == 0) break;

        temple_list = (Temple *)malloc(temple_len * sizeof(Temple));

        gets(str);
        tok = strtok(str, " ");
        hero.x = atoi(tok);

        tok = strtok(NULL, " ");
        hero.y = atoi(tok);

        gets(str);
        tok = strtok(str, " ");
        hero.inc_x = atoi(tok);

        tok = strtok(NULL, " ");
        hero.inc_y = atoi(tok);

        int i;
        for(i = 0; i < temple_len; i++)
            temple_list[i] = get_temple(str);

        execute_dowsing_skill(out_str, &out_str_cursor, hero, temple_list, temple_len);

        free(temple_list);
    }

    return 0;
}
