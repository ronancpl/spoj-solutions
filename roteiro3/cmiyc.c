#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct {
    int64_t rel_speed;
} Player;

int64_t CMIYC_Instance(int64_t cpoints,Player *player) {
    if(cpoints % (int64_t)((player[0]).rel_speed + (player[1]).rel_speed) == 0)
        return(cpoints / (int64_t)((player[0]).rel_speed + (player[1]).rel_speed));

    return(0);
}

Player* InitPlayers() {
    Player *player = (Player *)malloc(2*sizeof(Player));
    (player[0]).rel_speed = (int64_t)1;
    (player[1]).rel_speed = (int64_t)2;

    return(player);
}

void DestroyPlayers(Player *player) {
    free(player);
}

int main() {
    int64_t inst=0,i;
    int64_t cpoints;

    scanf("%"PRId64"",&inst);
    Player *player = InitPlayers();

    for(i = 0; i < inst; i++) {
        scanf("%"PRId64"",&cpoints);
        printf("%"PRId64"\n",CMIYC_Instance(cpoints,player));
    }

    DestroyPlayers(player);

    return 0;
}
