// ./include/engine.h
#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum : uint8_t {
    EMPTY = 0,
    PLAYER_X = 1,
    PLAYER_O = 2
} CellState;

typedef struct {
    uint8_t row;
    uint8_t col;
} Position;

typedef struct {
    Position pos;
    CellState player;
} Move;

typedef struct {
    CellState grid[3][3];
    uint8_t move_count;
} Board;

void init_board(Board* board);
bool make_move(Board* board, Move move);
CellState check_winner(const Board* board);

char player(const Board* board, uint8_t row, uint8_t col);
void view(const Board* board);
