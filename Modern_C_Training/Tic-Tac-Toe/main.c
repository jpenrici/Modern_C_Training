/**
 * @file main.c
 * @brief Tic Tac Toe — game loop and user interaction.
 *
 * Responsibilities:
 *  - Alternate turns between PLAYER_X and PLAYER_O.
 *  - Read and validate moves through read_move(), retrying on bad input.
 *  - Detect end conditions (winner or full board) via the engine API.
 *  - Display the final board and result.
 */

#include "engine.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Reads a single board coordinate (1-based) from stdin.
 *
 * Uses fgets + atoi to avoid the format/type mismatch that occurs when
 * passing a uint8_t* directly to scanf or fgets.
 *
 * @param prompt  Label printed before reading (e.g. "Row (1-3): ").
 * @return Zero-based coordinate in [0, 2]. Out-of-range values are passed
 *         through; make_move() will reject them and trigger a retry.
 */
static uint8_t read_coord(const char *prompt)
{
    char buf[8];
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    return (uint8_t)(atoi(buf) - 1); /* convert to 0-based */
}

/**
 * @brief Prompts the active player for a complete move.
 *
 * @param player  Current player (PLAYER_X or PLAYER_O).
 * @return A Move struct ready to pass to make_move().
 */
static Move read_move(CellState player)
{
    printf("Player %c\n", cell_char(player));
    uint8_t row = read_coord("  Row (1-3): ");
    uint8_t col = read_coord("  Col (1-3): ");

    return (Move){ .pos = {row, col}, .player = player };
}

int main(void)
{
    Board     board;
    CellState current_player = PLAYER_X;
    CellState winner         = EMPTY;

    // Game loop
    init_board(&board);
    printf("=== Tic Tac Toe ===\n");

    while (true) {
        view_board(&board);

        Move move;
        do {
            move = read_move(current_player);
        } while (!make_move(&board, move));

        winner = check_winner(&board);
        if (winner != EMPTY)
            break;

        if (board_is_full(&board))
            break;

        // Alternate turns
        current_player = (current_player == PLAYER_X) ? PLAYER_O : PLAYER_X;
    }

    view_board(&board);

    if (winner == EMPTY)
        printf("\nIt's a tie!\n");
    else
        printf("\nWinner: %c\n", cell_char(winner));

    return 0;
}
