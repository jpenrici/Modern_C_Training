/**
 * @file engine.h
 * @brief Tic Tac Toe game engine — types and public API.
 *
 * Provides the Board, Move, and CellState types together with the
 * functions needed to initialise a board, apply moves, query the
 * winner, and render the current state to stdout.
 *
 * Requires a C23-capable compiler (-std=c23) for:
 *   - typed enum  (enum : uint8_t)
 *   - [[nodiscard]] attribute on make_move
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Represents the content of a single board cell.
 *
 * The underlying type is uint8_t so that the 3×3 grid fits in 9 bytes.
 */
typedef enum : uint8_t {
    EMPTY    = 0, /**< Cell has not been played yet.   */
    PLAYER_X = 1, /**< Cell is owned by player X.      */
    PLAYER_O = 2, /**< Cell is owned by player O.      */
} CellState;

/**
 * @brief Zero-based (row, col) coordinates inside the 3×3 grid.
 *
 * Valid range for each field: [0, 2].
 */
typedef struct {
    uint8_t row;
    uint8_t col;
} Position;

/**
 * @brief A single game move: a position and the player who plays it.
 */
typedef struct {
    Position  pos;    /**< Target cell on the board.          */
    CellState player; /**< Must be PLAYER_X or PLAYER_O.      */
} Move;

/**
 * @brief Complete, self-contained game state.
 *
 * @note move_count is updated by make_move() and can be compared
 *       against 9 to detect a full board without extra variables.
 */
typedef struct {
    CellState grid[3][3]; /**< Row-major cell contents.          */
    uint8_t   move_count; /**< Number of moves made so far.      */
} Board;

/**
 * @brief Resets every cell to EMPTY and sets move_count to 0.
 * @param board  Non-null pointer to the board to initialise.
 */
void init_board(Board *board);

/**
 * @brief Attempts to apply @p move to @p board.
 *
 * The move is rejected when the target cell is out of range or already
 * occupied. On success the cell is updated and move_count is incremented.
 *
 * @param board  Non-null pointer to the current board state.
 * @param move   Move to apply; player must be PLAYER_X or PLAYER_O.
 * @return true  Move was valid and has been applied.
 * @return false Move was invalid; the board is unchanged.
 *
 * @note Marked [[nodiscard]]: the caller must check the return value and
 *       re-prompt the user on failure.
 */
[[nodiscard]] bool make_move(Board *board, Move move);

/**
 * @brief Checks all rows, columns, and diagonals for a winning line.
 *
 * @param board  Non-null pointer to the board to inspect.
 * @return The CellState of the winner (PLAYER_X or PLAYER_O),
 *         or EMPTY when no winner exists yet.
 */
CellState check_winner(const Board *board);

/**
 * @brief Returns true when all 9 cells have been played.
 *
 * Relies on Board.move_count so it runs in O(1) without scanning the grid.
 *
 * @param board  Non-null pointer to the board to inspect.
 */
bool board_is_full(const Board *board);

/**
 * @brief Converts a cell state to its display character.
 *
 * @param s  CellState value.
 * @return 'X', 'O', or ' ' for EMPTY.
 */
static inline char cell_char(CellState s)
{
    return s == PLAYER_X ? 'X' : s == PLAYER_O ? 'O' : ' ';
}

/**
 * @brief Prints the current board to stdout in a human-readable grid.
 * @param board  Non-null pointer to the board to display.
 */
void view_board(const Board *board);
