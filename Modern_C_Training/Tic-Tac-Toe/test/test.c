/**
 * @file test.c
 * @brief Unit test suite for the Tic Tac Toe engine.
 *
 * No external framework required — a lightweight macro-based runner is
 * defined here.  Each test is a static function returning bool; the
 * RUN() macro registers it, prints a result line, and tallies pass/fail.
 *
 * Build (from the project root, after cmake):
 *   cmake --build build
 *   ./build/test_engine
 *
 * Exit code: 0 if all tests pass, 1 if any fail.
 */

#include "engine.h"
#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * Minimal test runner
 * ---------------------------------------------------------------------- */

/** Fails the current test, printing the condition and source location. */
#define ASSERT(cond)                                                        \
    do {                                                                    \
        if (!(cond)) {                                                      \
            printf("    FAIL  %s:%d  →  %s\n", __FILE__, __LINE__, #cond);  \
            return false;                                                   \
        }                                                                   \
    } while (0)

/** Same as ASSERT but also prints the two compared values. */
#define ASSERT_EQ(a, b)                                                     \
    do {                                                                    \
        if ((a) != (b)) {                                                   \
            printf("    FAIL  %s:%d  →  %s == %s  (%d != %d)\n",            \
                   __FILE__, __LINE__, #a, #b, (int)(a), (int)(b));         \
            return false;                                                   \
        }                                                                   \
    } while (0)

static int g_passed = 0;
static int g_failed = 0;

/** Runs a test function and records the result. */
#define RUN(fn)                                                             \
    do {                                                                    \
        printf("  %-45s", #fn);                                             \
        if (fn()) { printf("ok\n"); g_passed++; }                           \
        else       {                              g_failed++; }             \
    } while (0)

/* -------------------------------------------------------------------------
* Helpers
* ---------------------------------------------------------------------- */

/**
* @brief Fills the board with a sequence of alternating moves.
*
* @param board   Board to populate (must already be initialised).
* @param cells   Flat array of CellState values in row-major order
*                (index 0 = grid[0][0], index 8 = grid[2][2]).
*                Use EMPTY to leave a cell unplayed.
*/
static void fill_board(Board *board, const CellState cells[9])
{
    init_board(board);
    for (int i = 0; i < 9; i++) {
        if (cells[i] != EMPTY) {
            Move m = { .pos = { (uint8_t)(i / 3), (uint8_t)(i % 3) },
                       .player = cells[i]
                     };
            /* Return value intentionally ignored: fill_board is a test
             * fixture that only places known-valid, non-overlapping moves.
             * The individual make_move tests verify the return value.    */
            (void)make_move(board, m);
        }
    }
}

/* -------------------------------------------------------------------------
 * init_board
 * ---------------------------------------------------------------------- */

static bool test_init_clears_grid(void)
{
    Board board = {
                   .grid = {
                            {PLAYER_X, PLAYER_O, PLAYER_X},
                            {PLAYER_O, PLAYER_X, PLAYER_O},
                            {PLAYER_X, PLAYER_O, PLAYER_X},
                            },
                   .move_count = 9,
                   };

    init_board(&board);

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            ASSERT_EQ(board.grid[r][c], EMPTY);

    return true;
}

static bool test_init_resets_move_count(void)
{
    Board board = { .move_count = 9 };
    init_board(&board);
    ASSERT_EQ(board.move_count, 0);
    return true;
}

/* -------------------------------------------------------------------------
 * make_move
 * ---------------------------------------------------------------------- */

static bool test_make_move_valid(void)
{
    Board board;
    init_board(&board);
    Move m = { .pos = {1, 1}, .player = PLAYER_X };

    bool result = make_move(&board, m);

    ASSERT(result == true);
    ASSERT_EQ(board.grid[1][1], PLAYER_X);
    ASSERT_EQ(board.move_count, 1);
    return true;
}

static bool test_make_move_increments_count(void)
{
    Board board;
    init_board(&board);

    (void)make_move(&board, (Move) {.pos = {0, 0}, .player = PLAYER_X});
    (void)make_move(&board, (Move) {.pos = {1, 1}, .player = PLAYER_O});
    (void)make_move(&board, (Move) {.pos = {2, 2}, .player = PLAYER_X});

    ASSERT_EQ(board.move_count, 3);
    return true;
}

static bool test_make_move_rejects_occupied_cell(void)
{
    Board board;
    init_board(&board);
    Move first  = { .pos = {0, 0}, .player = PLAYER_X };
    Move second = { .pos = {0, 0}, .player = PLAYER_O };

    (void)make_move(&board, first);
    bool result = make_move(&board, second);

    ASSERT(result == false);
    ASSERT_EQ(board.grid[0][0], PLAYER_X); /* original value preserved */
    ASSERT_EQ(board.move_count, 1);
    return true;
}

static bool test_make_move_rejects_row_out_of_range(void)
{
    Board board;
    init_board(&board);
    Move m = { .pos = {3, 0}, .player = PLAYER_X };
    ASSERT(make_move(&board, m) == false);
    ASSERT_EQ(board.move_count, 0);
    return true;
}

static bool test_make_move_rejects_col_out_of_range(void)
{
    Board board;
    init_board(&board);
    Move m = { .pos = {0, 3}, .player = PLAYER_X };
    ASSERT(make_move(&board, m) == false);
    ASSERT_EQ(board.move_count, 0);
    return true;
}

/**
 * Coordinates are uint8_t (unsigned). Passing -1 wraps to 255, which is
 * >= 3, so make_move must reject it without undefined behaviour.
 */
static bool test_make_move_rejects_underflow_coords(void)
{
    Board board;
    init_board(&board);
    Move m = { .pos = {(uint8_t) -1, 0}, .player = PLAYER_X };
    ASSERT(make_move(&board, m) == false);
    ASSERT_EQ(board.move_count, 0);
    return true;
}

/* -------------------------------------------------------------------------
 * check_winner — all 8 winning lines
 * ---------------------------------------------------------------------- */

static bool test_winner_row_0(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, PLAYER_X, PLAYER_X,
        PLAYER_O, PLAYER_O, EMPTY,
        EMPTY,    EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_row_1(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_O, PLAYER_O, EMPTY,
        PLAYER_X, PLAYER_X, PLAYER_X,
        EMPTY,    EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_row_2(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_O, PLAYER_O, EMPTY,
        EMPTY,    EMPTY,    EMPTY,
        PLAYER_X, PLAYER_X, PLAYER_X,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_col_0(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, PLAYER_O, EMPTY,
        PLAYER_X, PLAYER_O, EMPTY,
        PLAYER_X, EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_col_1(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_O, PLAYER_X, EMPTY,
        PLAYER_O, PLAYER_X, EMPTY,
        EMPTY,    PLAYER_X, EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_col_2(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_O, EMPTY, PLAYER_X,
        PLAYER_O, EMPTY, PLAYER_X,
        EMPTY,    EMPTY, PLAYER_X,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_main_diagonal(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, PLAYER_O, EMPTY,
        EMPTY,    PLAYER_X, PLAYER_O,
        EMPTY,    EMPTY,    PLAYER_X,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_anti_diagonal(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        EMPTY,    PLAYER_O, PLAYER_X,
        EMPTY,    PLAYER_X, PLAYER_O,
        PLAYER_X, EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_X);
    return true;
}

static bool test_winner_player_o(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_O, PLAYER_O, PLAYER_O,
        PLAYER_X, PLAYER_X, EMPTY,
        EMPTY,    EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), PLAYER_O);
    return true;
}

static bool test_no_winner_mid_game(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, EMPTY,    EMPTY,
        EMPTY,    PLAYER_O, EMPTY,
        EMPTY,    EMPTY,    EMPTY,
    });
    ASSERT_EQ(check_winner(&board), EMPTY);
    return true;
}

static bool test_no_winner_empty_board(void)
{
    Board board;
    init_board(&board);
    ASSERT_EQ(check_winner(&board), EMPTY);
    return true;
}

/* -------------------------------------------------------------------------
 * check_winner — tie (full board, no winner)
 * ---------------------------------------------------------------------- */

static bool test_no_winner_tie(void)
{
    /* Classic drawn game:
     *   X | O | X
     *   X | O | O
     *   O | X | X   */
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, PLAYER_O, PLAYER_X,
        PLAYER_X, PLAYER_O, PLAYER_O,
        PLAYER_O, PLAYER_X, PLAYER_X,
    });
    ASSERT_EQ(check_winner(&board), EMPTY);
    return true;
}

/* -------------------------------------------------------------------------
 * board_is_full
 * ---------------------------------------------------------------------- */

static bool test_board_is_full_false_on_empty(void)
{
    Board board;
    init_board(&board);
    ASSERT(board_is_full(&board) == false);
    return true;
}

static bool test_board_is_full_false_on_partial(void)
{
    Board board;
    init_board(&board);
    (void)make_move(&board, (Move) {
        .pos = {0, 0}, .player = PLAYER_X
    });
    ASSERT(board_is_full(&board) == false);
    return true;
}

static bool test_board_is_full_true_after_9_moves(void)
{
    Board board;
    fill_board(&board, (CellState[9]) {
        PLAYER_X, PLAYER_O, PLAYER_X,
        PLAYER_X, PLAYER_O, PLAYER_O,
        PLAYER_O, PLAYER_X, PLAYER_X,
    });
    ASSERT(board_is_full(&board) == true);
    return true;
}

/* -------------------------------------------------------------------------
 * cell_char
 * ---------------------------------------------------------------------- */

static bool test_cell_char_x(void)
{
    ASSERT_EQ(cell_char(PLAYER_X), 'X');
    return true;
}

static bool test_cell_char_o(void)
{
    ASSERT_EQ(cell_char(PLAYER_O), 'O');
    return true;
}

static bool test_cell_char_empty(void)
{
    ASSERT_EQ(cell_char(EMPTY), ' ');
    return true;
}

/* -------------------------------------------------------------------------
 * Entry point
 * ---------------------------------------------------------------------- */

int main(void)
{
    printf("\n=== Tic Tac Toe — engine unit tests ===\n\n");

    printf("init_board\n");
    RUN(test_init_clears_grid);
    RUN(test_init_resets_move_count);

    printf("\nmake_move\n");
    RUN(test_make_move_valid);
    RUN(test_make_move_increments_count);
    RUN(test_make_move_rejects_occupied_cell);
    RUN(test_make_move_rejects_row_out_of_range);
    RUN(test_make_move_rejects_col_out_of_range);
    RUN(test_make_move_rejects_underflow_coords);

    printf("\ncheck_winner — winning lines\n");
    RUN(test_winner_row_0);
    RUN(test_winner_row_1);
    RUN(test_winner_row_2);
    RUN(test_winner_col_0);
    RUN(test_winner_col_1);
    RUN(test_winner_col_2);
    RUN(test_winner_main_diagonal);
    RUN(test_winner_anti_diagonal);
    RUN(test_winner_player_o);

    printf("\ncheck_winner — no winner\n");
    RUN(test_no_winner_mid_game);
    RUN(test_no_winner_empty_board);
    RUN(test_no_winner_tie);

    printf("\nboard_is_full\n");
    RUN(test_board_is_full_false_on_empty);
    RUN(test_board_is_full_false_on_partial);
    RUN(test_board_is_full_true_after_9_moves);

    printf("\ncell_char\n");
    RUN(test_cell_char_x);
    RUN(test_cell_char_o);
    RUN(test_cell_char_empty);

    printf("\n---------------------------------------\n");
    printf("  %d passed   %d failed\n\n", g_passed, g_failed);

    return g_failed > 0 ? 1 : 0;
}
