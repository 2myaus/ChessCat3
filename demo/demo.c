#include <stdio.h>
#include <string.h>
#include "../libchesscat.h"

/*   Main   */

int main(/*int argc, char* argv[]*/)
{
    printf("chesscat_Piece size: %lu\n", sizeof(chesscat_Piece));
    printf("chesscat_Game size: %lu\n", sizeof(chesscat_Game));
    printf("chesscat_Square size: %lu\n", sizeof(chesscat_Square));
    printf("chesscat_Move size: %lu\n", sizeof(chesscat_Move));

    chesscat_Game g;
    chesscat_set_default_game(&g);
    chesscat_Move move_buf[2048];
    uint16_t num_moves = chesscat_get_all_possible_moves(&g.position, move_buf);
    printf("Moves: %d\n", num_moves);
    for (uint16_t i = 0; i < num_moves; i++)
    {
        _chesscat_print_move(move_buf[i]);
    }

    _chesscat_print_position(&g);

    while (true)
    {
        char movestrbuf[16];
        fgets(movestrbuf, 9, stdin);
        movestrbuf[strlen(movestrbuf) - 1] = '\0'; // Remove newline
        chesscat_MovePromotion next = chesscat_get_move_from_string(&g.position, movestrbuf);
        if (_chesscat_is_valid_move(next.move))
        {
            chesscat_make_move(&g.position, next.move, next.promotion);
            _chesscat_print_position(&g);
        }
        else if (movestrbuf[0] == '\n')
        {
            _chesscat_print_position(&g);
        }
    }
}