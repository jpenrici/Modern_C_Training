/**
 * @file engine.c
 * @brief Tic Tac Toe engine implementation.
 */

#include "engine.h"
#include <stdio.h>

void init_board(Board *board)
{
    *board = (Board){};
}

bool make_move(Board *board, Move move)
{
    if (move.pos.row >= 3 || move.pos.col >= 3)
        return false;

    if (board->grid[move.pos.row][move.pos.col] != EMPTY)
        return false;

    board->grid[move.pos.row][move.pos.col] = move.player;
    board->move_count++;

    return true;
}

bool board_is_full(const Board *board)
{
    return board->move_count >= 9;
}

/**
 * @brief Static table of all 8 winning lines.
 *
 * Each entry holds the (row, col) coordinates of the three cells that
 * form one line. Encoding the lines as data keeps check_winner() as a
 * simple, uniform loop with no special-cased diagonal branches.
 *
 * Layout:
 *   [0-2]  three rows
 *   [3-5]  three columns
 *   [6]    main diagonal  (top-left  → bottom-right)
 *   [7]    anti-diagonal  (top-right → bottom-left)
 */
static const uint8_t WINNING_LINES[8][3][2] = {
    /* rows */
    {{0,0},{0,1},{0,2}},
    {{1,0},{1,1},{1,2}},
    {{2,0},{2,1},{2,2}},
    /* columns */
    {{0,0},{1,0},{2,0}},
    {{0,1},{1,1},{2,1}},
    {{0,2},{1,2},{2,2}},
    /* diagonals */
    {{0,0},{1,1},{2,2}},
    {{0,2},{1,1},{2,0}},
};

CellState check_winner(const Board *board)
{
    for (size_t i = 0; i < 8; i++) {
        CellState a = board->grid[WINNING_LINES[i][0][0]][WINNING_LINES[i][0][1]];
        CellState b = board->grid[WINNING_LINES[i][1][0]][WINNING_LINES[i][1][1]];
        CellState c = board->grid[WINNING_LINES[i][2][0]][WINNING_LINES[i][2][1]];

        if (a != EMPTY && a == b && b == c)
            return a;
    }
    return EMPTY;
}

void view_board(const Board *board)
{
    printf("\n");
    for (int r = 0; r < 3; r++) {
        printf(" %c | %c | %c\n",
               cell_char(board->grid[r][0]),
               cell_char(board->grid[r][1]),
               cell_char(board->grid[r][2]));
        if (r < 2)
            printf("-----------\n");
    }

}
