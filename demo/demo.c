#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../libchesscat.h"

char GetPieceChar(chesscat_Piece piece)
{
    char base;
    switch (piece.type)
    {
    case Pawn:
        base = 'p';
        break;
    case King:
        base = 'k';
        break;
    case Queen:
        base = 'q';
        break;
    case Rook:
        base = 'r';
        break;
    case Knight:
        base = 'n';
        break;
    case Bishop:
        base = 'b';
        break;
    default:
        base = ' ';
        break;
    }
    if (piece.color == White)
    {
        return toupper(base);
    }
    return base;
}

void PrintMove(chesscat_Position *position, chesscat_Move m)
{
    if (!chesscat_square_in_bounds(position, m.from) || !chesscat_square_in_bounds(position, m.from))
    {
        return;
    }
    char getCol[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    printf("%c%d to %c%d\n", getCol[m.from.col], m.from.row + 1, getCol[m.to.col], m.to.row + 1);
}

void PrintPosition(chesscat_Game *game)
{
    bool white = true;
    for (int8_t row = game->position.game_rules.board_height - 1; row >= 0; row--)
    {
        for (int8_t col = 0; col < game->position.game_rules.board_width; col++)
        {
            chesscat_Square checking = {.row = row, .col = col};
            if (white)
            {
                printf("\e[1;30;107m");
            }
            else
            {
                printf("\e[1;97;40m");
            }
            printf("%c", GetPieceChar(chesscat_get_piece_at_square(&(game->position), checking)));
            white = !white;
        }
        printf("\e[m\n");
        white = !white;
    }
}

/*   Main   */

int main(/*int argc, char* argv[]*/)
{
    printf("chesscat_Piece size: %lu\n", sizeof(chesscat_Piece));
    printf("chesscat_Game size: %lu\n", sizeof(chesscat_Game));
    printf("chesscat_Square size: %lu\n", sizeof(chesscat_Square));
    printf("chesscat_Move size: %lu\n", sizeof(chesscat_Move));

    chesscat_Game g;
    chesscat_set_default_game(&g);

    PrintPosition(&g);

    while (true)
    {
        char movestrbuf[16];
        fgets(movestrbuf, 9, stdin);
        movestrbuf[strlen(movestrbuf) - 1] = '\0'; // Remove newline
        chesscat_MovePromotion next = chesscat_get_move_from_string(&g.position, movestrbuf);
        if (chesscat_is_valid_move(next.move))
        {
            chesscat_make_move(&g.position, next.move, next.promotion);
            PrintPosition(&g);
            chesscat_EPositionState currentState = chesscat_get_current_state(&(g.position));
            if(currentState == Checkmated){
                printf("Mate!\n");
                return 0;
            }
            else if(currentState == Stalemated){
                printf("Stalemate!\n");
                return 0;
            }
            else if(currentState == Checked){
                printf("Check!\n");
            }
        }
        else if (movestrbuf[0] == '\n')
        {
            PrintPosition(&g);
        }
    }
}