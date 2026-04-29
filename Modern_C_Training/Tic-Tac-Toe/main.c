// ./main.c
#include "engine.h"
#include <stdio.h>

int main()
{
    constexpr int MAX_MOVES = 9;

    Board board;
    init_board(&board);

    // TEST
    printf("Game Started!\n");
    Move moviment = {.pos = {1, 1}, .player = PLAYER_X};

    if (make_move(&board, moviment)) {
        view(&board);
    }

    return 0;
}
