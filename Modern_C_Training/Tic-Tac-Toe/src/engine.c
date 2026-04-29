// ./src/engine.c
#include "engine.h"
#include <stdio.h>
#include <string.h>

void init_board(Board* board) {
    *board = (Board){};
}

[[ nodiscard ]] bool make_move(Board* board, Move move) {
    if (move.pos.row < 0 || move.pos.col < 0) {
        return false;
    }
    if (move.pos.row >= 3 || move.pos.col >= 3) {
        return false;
    }
    if (board->grid[move.pos.row][move.pos.col] == EMPTY) {
        board->grid[move.pos.row][move.pos.col] = move.player;
        board->move_count++;
        return true;
    }
    return false;
}

CellState check_winner(const Board* board){
    for (size_t i = 0; i < 3; i++) {
        // Check row
        if (board->grid[i][0] != EMPTY &&
            board->grid[i][0] == board->grid[i][1] &&
            board->grid[i][1] == board->grid[i][2])
        {
            return board->grid[i][0];
        }
        // Check col
        if (board->grid[0][i] != EMPTY &&
            board->grid[0][i] == board->grid[1][i] &&
            board->grid[1][i] == board->grid[2][i])
        {
            return board->grid[0][i];
        }
    }

    // Check diagonal
    if (board->grid[0][0] != EMPTY &&
        board->grid[0][0] == board->grid[1][1] &&
        board->grid[1][1] == board->grid[2][2])
    {
        return board->grid[0][0];
    }
    if (board->grid[0][2] != EMPTY &&
        board->grid[0][2] == board->grid[1][1] &&
        board->grid[1][1] == board->grid[2][0])
    {
        return board->grid[0][2];
    }

    return EMPTY;
}

char player(const Board* board, uint8_t row, uint8_t col)
{
    if (board->grid[row][col] == PLAYER_X) {
        return 'X';
    }
    if (board->grid[row][col] == PLAYER_O) {
        return 'O';
    }
    return ' ';
}

void view(const Board* board)
{
    printf(" %c | %c | %c\n", player(board, 0,0), player(board, 0,1), player(board, 0,2));
    printf("-----------\n");
    printf(" %c | %c | %c\n", player(board, 1,0), player(board, 1,1), player(board, 1,2));
    printf("-----------\n");
    printf(" %c | %c | %c\n", player(board, 2,0), player(board, 2,1), player(board, 2,2));
}
