#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef CHESSCAT_INCLUDE_MISC_H
    #include "misc.h"
#endif

/*   Misc functions   */

bool _chesscat_same_squares(chesscat_Square s1, chesscat_Square s2)
{
    return s1.row == s2.row && s1.col == s2.col;
}

bool _chesscat_same_move(chesscat_Move m1, chesscat_Move m2)
{
    return _chesscat_same_squares(m1.from, m2.from) && _chesscat_same_squares(m1.to, m2.to);
}

bool _chesscat_is_valid_square(chesscat_Square square)
{
    return square.col > -1 && square.row > -1;
}

bool _chesscat_is_valid_move(chesscat_Move m)
{
    return _chesscat_is_valid_square(m.from) && _chesscat_is_valid_square(m.to);
}

/*   Position utility functions   */

/*
 * chesscat_square_in_bounds
 * 
 * Checks if a square is within the boundaries of the given position
 */
bool chesscat_square_in_bounds(chesscat_Position *position, chesscat_Square square)
{
    return square.row >= 0 && square.col >= 0 && square.row < position->game_rules.board_height && square.col < position->game_rules.board_width;
}

bool _chesscat_square_on_promotion_rank(chesscat_Position *position, chesscat_Square square, chesscat_EColor color)
{
    switch (color)
    {
    case White:
        return square.row == position->game_rules.board_height - 1;
    case Black:
        return square.row == 0;
    case Green:
        return square.col == position->game_rules.board_width - 1;
    case Red:
        return square.col == 0;
    }
    return false;
}

bool _chesscat_position_ignores_checks(chesscat_Position *position)
{
    if (position->game_rules.ignore_checks)
    {
        return true;
    }
    uint8_t num_colors = 0;
    if (position->color_data[White].is_in_game)
    {
        num_colors++;
    }
    if (position->color_data[Black].is_in_game)
    {
        num_colors++;
    }
    if (position->color_data[Red].is_in_game)
    {
        num_colors++;
    }
    if (position->color_data[Green].is_in_game)
    {
        num_colors++;
    }

    if (num_colors > 2)
    {
        return true;
    }
    return false;
}

void _chesscat_set_piece(chesscat_Position *position, int8_t row, int8_t col, chesscat_Piece piece)
{
    position->board[row][col] = piece;
}

void chesscat_set_piece_at_square(chesscat_Position *position, chesscat_Square square, chesscat_Piece piece)
{
    _chesscat_set_piece(position, square.row, square.col, piece);
}

chesscat_Piece _chesscat_get_piece(chesscat_Position *position, int8_t row, int8_t col)
{
    return position->board[row][col];
}

chesscat_Piece chesscat_get_piece_at_square(chesscat_Position *position, chesscat_Square square)
{
    return _chesscat_get_piece(position, square.row, square.col);
}

chesscat_Square _chesscat_find_king(chesscat_Position *position, chesscat_EColor color)
{
    for (uint16_t row = 0; row < position->game_rules.board_height; row++)
    {
        for (uint16_t col = 0; col < position->game_rules.board_width; col++)
        {
            chesscat_Square square = {.row = row, .col = col};
            chesscat_Piece piece = chesscat_get_piece_at_square(position, square);
            if (piece.type == King && piece.color == color)
            {
                return square;
            }
        }
    }
    chesscat_Square none = {.row = -1, .col = -1};
    return none;
}

chesscat_Square _chesscat_find_lower_rook(chesscat_Position *position, chesscat_EColor color)
{ // Finds lower-coordinate rook on the specified color's king's rank
    chesscat_Square none = {.row = -1, .col = -1};
    if (color == White || color == Black)
    {
        chesscat_Square king_square = _chesscat_find_king(position, color);
        if (!_chesscat_is_valid_square(king_square))
        {
            return none;
        }
        for (int8_t i = king_square.col - 1; i >= 0; i--)
        {
            chesscat_Square square = {.row = king_square.row, .col = i};
            chesscat_Piece piece = chesscat_get_piece_at_square(position, square);
            if (piece.type == Rook && piece.color == color)
            {
                return square;
            }
        }
    }
    if (color == Green || color == Red)
    {
        chesscat_Square king_square = _chesscat_find_king(position, color);
        if (!_chesscat_is_valid_square(king_square))
        {
            return none;
        }
        for (int8_t i = king_square.row - 1; i >= 0; i--)
        {
            chesscat_Square square = {.row = i, .col = king_square.col};
            chesscat_Piece piece = chesscat_get_piece_at_square(position, square);
            if (piece.type == Rook && piece.color == color)
            {
                return square;
            }
        }
    }
    return none;
}

chesscat_Square _chesscat_find_upper_rook(chesscat_Position *position, chesscat_EColor color)
{
    chesscat_Square none = {.row = -1, .col = -1};
    if (color == White || color == Black)
    {
        chesscat_Square king_square = _chesscat_find_king(position, color);
        if (!_chesscat_is_valid_square(king_square))
        {
            return none;
        }
        for (uint16_t i = king_square.col + 1; i < position->game_rules.board_width; i++)
        {
            chesscat_Square square = {.row = king_square.row, .col = i};
            chesscat_Piece piece = chesscat_get_piece_at_square(position, square);
            if (piece.type == Rook && piece.color == color)
            {
                return square;
            }
        }
    }
    if (color == Green || color == Red)
    {
        chesscat_Square king_square = _chesscat_find_king(position, color);
        if (!_chesscat_is_valid_square(king_square))
        {
            return none;
        }
        for (uint16_t i = king_square.row + 1; i < position->game_rules.board_height; i++)
        {
            chesscat_Square square = {.row = i, .col = king_square.col};
            chesscat_Piece piece = chesscat_get_piece_at_square(position, square);
            if (piece.type == Rook && piece.color == color)
            {
                return square;
            }
        }
    }
    return none;
}

void _chesscat_set_next_to_play(chesscat_Position *position)
{
    if (position->color_data[White].is_in_game == false &&
        position->color_data[Black].is_in_game == false &&
        position->color_data[Green].is_in_game == false &&
        position->color_data[Red].is_in_game == false)
    {
        return;
    }
    switch (position->to_move)
    {
    case White:
        position->to_move = Red;
        break;
    case Red:
        position->to_move = Black;
        break;
    case Black:
        position->to_move = Green;
        break;
    default:
        position->to_move = White;
        break;
    }
    while (position->color_data[position->to_move].is_in_game == false || !_chesscat_is_valid_square(_chesscat_find_king(position, position->to_move)))
    {
        switch (position->to_move)
        {
        case White:
            position->to_move = Red;
            break;
        case Red:
            position->to_move = Black;
            break;
        case Black:
            position->to_move = Green;
            break;
        default:
            position->to_move = White;
            break;
        }
    }
}

_chesscat_MoveCasleType _chesscat_move_castles(chesscat_Position *position, chesscat_Move move)
{
    int8_t coldist = move.to.col - move.from.col;
    int8_t rowdist = move.to.row - move.from.row;

    chesscat_Piece piece = chesscat_get_piece_at_square(position, move.from);
    if (piece.type == King && piece.is_royal && (abs(rowdist) > 1 || abs(coldist > 1)))
    {
        if (rowdist > 0 || coldist > 0)
        {
            return UpperCastle;
        }
        if (rowdist < 0 || coldist < 0)
        {
            return LowerCastle;
        }
    }
    return NotCastle;
}

bool _chesscat_color_can_capture_piece(chesscat_Position *position, chesscat_EColor color, chesscat_Piece piece)
{
    // NOTE: Position is used here because gamerules may change what colors can be captured
    return (piece.type == Empty || piece.color != color);
}

void _chesscat_add_move_to_buf(chesscat_Move move, chesscat_Move *moves_buf[], uint16_t *num_moves)
{
    if (*moves_buf != NULL)
    {
        (*moves_buf)[*num_moves] = move;
    }
    (*num_moves)++;
}

/*
 * chesscat_get_all_possible_moves
 * 
 * Writes all possible (not necessarily legal) moves from the given square for the current color to play to moves_buf
 */

uint16_t chesscat_get_possible_moves_from(chesscat_Position *position, chesscat_Square square, chesscat_Move moves_buf[])
{
    bool moves_like_bishop = false;
    bool moves_like_rook = false;
    bool moves_like_knight = false;
    bool moves_like_king = false;
    bool moves_like_pawn = false;

    chesscat_Piece piece = chesscat_get_piece_at_square(position, square);

    if (piece.color != position->to_move || piece.type == Empty)
    {
        return 0;
    }

    uint16_t num_moves = 0;

    switch (piece.type)
    {
    case Pawn:
        moves_like_pawn = true;
        break;
    case King:
        moves_like_king = true;
        break;
    case Queen:
        moves_like_bishop = true;
        moves_like_rook = true;
        break;
    case Rook:
        moves_like_rook = true;
        break;
    case Knight:
        moves_like_knight = true;
        break;
    case Bishop:
        moves_like_bishop = true;
        break;
    default:
        break;
    }

    if (moves_like_king)
    {
        for (int8_t rowto = square.row - 1; rowto <= square.row + 1; rowto++)
        {
            if (rowto < 0 || rowto >= position->game_rules.board_height)
            {
                continue;
            }
            for (int8_t colto = square.col - 1; colto <= square.col + 1; colto++)
            {

                if (colto < 0 || colto >= position->game_rules.board_height)
                {
                    continue;
                }
                if (colto == square.col && rowto == square.row)
                {
                    continue;
                }

                chesscat_Piece target_piece = _chesscat_get_piece(position, rowto, colto);
                if (!_chesscat_color_can_capture_piece(position, piece.color, target_piece))
                {
                    continue;
                }

                chesscat_Square tosquare = {.col = colto, .row = rowto};
                chesscat_Move move = {.from = square, .to = tosquare};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (position->game_rules.allow_castle && piece.type == King && piece.is_royal && !position->color_data[piece.color].has_king_moved)
        {
            if (!position->color_data[piece.color].has_lower_rook_moved)
            {
                chesscat_Square lower_rook = _chesscat_find_lower_rook(position, piece.color);
                int8_t coldist = abs(square.col - lower_rook.col);
                int8_t rowdist = abs(square.row - lower_rook.row);
                if (_chesscat_is_valid_square(lower_rook) && (coldist > 1 || rowdist > 1))
                {
                    if (piece.color == White || piece.color == Black)
                    {
                        bool is_blocked = false;
                        for (int8_t col = lower_rook.col + 1; col < square.col; col++)
                        {
                            chesscat_Square checking = {.col = col, .row = square.row};
                            if (chesscat_get_piece_at_square(position, checking).type != Empty)
                            {
                                is_blocked = true;
                                break;
                            }
                        }
                        if (!is_blocked)
                        {
                            chesscat_Square tosquare = {.col = square.col - 2, .row = square.row};
                            chesscat_Move move = {.from = square, .to = tosquare};
                            _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
                        }
                    }
                    else
                    {
                        bool is_blocked = false;
                        for (int8_t row = lower_rook.row + 1; row < square.col; row++)
                        {
                            chesscat_Square checking = {.col = square.col, .row = row};
                            if (chesscat_get_piece_at_square(position, checking).type != Empty)
                            {
                                is_blocked = true;
                                break;
                            }
                        }
                        if (!is_blocked)
                        {
                            chesscat_Square tosquare = {.col = square.col, .row = square.row - 2};

                            chesscat_Move move = {.from = square, .to = tosquare};
                            _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
                        }
                    }
                }
            }
            if (!position->color_data[piece.color].has_upper_rook_moved)
            {
                chesscat_Square upper_rook = _chesscat_find_upper_rook(position, piece.color);
                int8_t coldist = abs(square.col - upper_rook.col);
                int8_t rowdist = abs(square.row - upper_rook.row);
                if (_chesscat_is_valid_square(upper_rook) && (coldist > 1 || rowdist > 1))
                {
                    if (piece.color == White || piece.color == Black)
                    {
                        bool is_blocked = false;
                        for (int8_t col = upper_rook.col - 1; col > square.col; col--)
                        {
                            chesscat_Square checking = {.col = col, .row = square.row};
                            if (chesscat_get_piece_at_square(position, checking).type != Empty)
                            {
                                is_blocked = true;
                                break;
                            }
                        }
                        if (!is_blocked)
                        {
                            chesscat_Square tosquare = {.col = square.col - 2, .row = square.row};

                            chesscat_Move move = {.from = square, .to = tosquare};
                            _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
                        }
                    }
                    else
                    {
                        bool is_blocked = false;
                        for (int8_t row = upper_rook.row - 1; row > square.row; row--)
                        {
                            chesscat_Square checking = {.col = square.col, .row = row};
                            if (chesscat_get_piece_at_square(position, checking).type != Empty)
                            {
                                is_blocked = true;
                                break;
                            }
                        }
                        if (!is_blocked)
                        {
                            chesscat_Square tosquare = {.col = square.col, .row = square.row - 2};

                            chesscat_Move move = {.from = square, .to = tosquare};
                            _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
                        }
                    }
                }
            }
        }
    }
    if (moves_like_knight)
    {
        // ordered as 2-square _ 1-square
        chesscat_Square up_left = {.col = square.col - 1, .row = square.row + 2};
        chesscat_Square up_right = {.col = square.col + 1, .row = square.row + 2};

        chesscat_Square right_up = {.col = square.col + 2, .row = square.row + 1};
        chesscat_Square right_down = {.col = square.col + 2, .row = square.row - 1};

        chesscat_Square down_right = {.col = square.col + 1, .row = square.row - 2};
        chesscat_Square down_left = {.col = square.col - 1, .row = square.row - 2};

        chesscat_Square left_down = {.col = square.col - 2, .row = square.row - 1};
        chesscat_Square left_up = {.col = square.col - 2, .row = square.row + 1};

        if (chesscat_square_in_bounds(position, up_left))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, up_left);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = up_left};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, up_right))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, up_right);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = up_right};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, right_up))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, right_up);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = right_up};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, right_down))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, right_down);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = right_down};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, down_right))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, down_right);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = down_right};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, down_left))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, down_left);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = down_left};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, left_down))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, left_down);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = left_down};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
        if (chesscat_square_in_bounds(position, left_up))
        {
            chesscat_Piece p = chesscat_get_piece_at_square(position, left_up);
            if (_chesscat_color_can_capture_piece(position, piece.color, p))
            {
                chesscat_Move move = {.from = square, .to = left_up};
                _chesscat_add_move_to_buf(move, &moves_buf, &num_moves);
            }
        }
    }
    if (moves_like_bishop)
    {
        chesscat_Square toSquare = square;
        while (true)
        { // Up-Right
            toSquare.row++;
            toSquare.col++;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Up-Left
            toSquare.row++;
            toSquare.col--;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Down-Right
            toSquare.row--;
            toSquare.col++;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Down-Left
            toSquare.row--;
            toSquare.col--;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }
    }
    if (moves_like_rook)
    {
        chesscat_Square toSquare = square;
        while (true)
        { // Up
            toSquare.row++;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Left
            toSquare.col--;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Down
            toSquare.row--;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true)
        { // Right
            toSquare.col++;
            if (!chesscat_square_in_bounds(position, toSquare))
            {
                break;
            }
            chesscat_Piece hitPiece = chesscat_get_piece_at_square(position, toSquare);
            if (hitPiece.type != Empty)
            {
                if (_chesscat_color_can_capture_piece(position, piece.color, hitPiece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if (moves_buf != NULL)
            {
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }
    }
    if (moves_like_pawn)
    {
        int8_t row_dist = 0;
        int8_t col_dist = 0;
        switch (piece.color)
        {
        case White:
            row_dist = 1;
            break;
        case Black:
            row_dist = -1;
            break;
        case Red:
            col_dist = -1;
            break;
        case Green:
            col_dist = 1;
            break;
        }

        chesscat_Square advance_square = {.row = square.row + row_dist, .col = square.col + col_dist};

        if (chesscat_square_in_bounds(position, advance_square))
        {
            chesscat_Square take_left_square = {.row = square.row + row_dist - col_dist, .col = square.col + col_dist - row_dist};
            chesscat_Square take_right_square = {.row = square.row + row_dist + col_dist, .col = square.col + col_dist + row_dist};

            chesscat_Piece advance_piece = chesscat_get_piece_at_square(position, advance_square);

            if (advance_piece.type == Empty)
            {
                if (moves_buf != NULL)
                {
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = advance_square;
                }
                num_moves++;
            }
            if (chesscat_square_in_bounds(position, take_left_square))
            {
                chesscat_Piece take_left_piece = chesscat_get_piece_at_square(position, take_left_square);
                if (take_left_piece.type != Empty && _chesscat_color_can_capture_piece(position, piece.color, take_left_piece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_left_square;
                    }
                    num_moves++;
                }
                if (_chesscat_same_squares(take_left_square, position->passantable_square))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_left_square;
                    }
                    num_moves++;
                }
            }
            if (chesscat_square_in_bounds(position, take_right_square))
            {
                chesscat_Piece take_right_piece = chesscat_get_piece_at_square(position, take_right_square);
                if (take_right_piece.type != Empty && _chesscat_color_can_capture_piece(position, piece.color, take_right_piece))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_right_square;
                    }
                    num_moves++;
                }
                if (_chesscat_same_squares(take_right_square, position->passantable_square))
                {
                    if (moves_buf != NULL)
                    {
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_right_square;
                    }
                    num_moves++;
                }
            }

            if (/*position->game_rules.torpedo_pawns ||*/
                (piece.color == White && square.row <= 1) ||
                (piece.color == Black && square.row >= position->game_rules.board_height - 2) ||
                (piece.color == Green && square.col >= 1) ||
                (piece.color == Red && square.col >= position->game_rules.board_width - 2))
            {
                chesscat_Square double_advance_square = {.row = square.row + row_dist * 2, .col = square.col + col_dist * 2};
                if (chesscat_square_in_bounds(position, double_advance_square))
                {
                    chesscat_Piece double_advance_piece = chesscat_get_piece_at_square(position, double_advance_square);
                    if (double_advance_piece.type == Empty)
                    {
                        if (moves_buf != NULL)
                        {
                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = double_advance_square;
                        }
                        num_moves++;
                    }
                }
            }

            // TODO: Add promotion
        }
        /*
        if(position->game_rules.sideways_pawns){
            chesscat_Square left_square = {.row = square.row - col_dist, .col = square.col - row_dist};
            chesscat_Square right_square = {.row = square.row + col_dist, .col = square.col + row_dist};

            if(chesscat_square_in_bounds(position, left_square)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = left_square;
                }
                num_moves++;
            }
            if(chesscat_square_in_bounds(position, right_square)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = right_square;
                }
                num_moves++;
            }
        }
        */
    }

    return num_moves;
}

/*
 * chesscat_get_all_possible_moves
 * 
 * Writes all possible (not necessarily legal) moves for the current color to play to moves_buf
 */
uint16_t chesscat_get_all_possible_moves(chesscat_Position *position, chesscat_Move moves_buf[])
{
    uint16_t move_count = 0;
    for (uint8_t row = 0; row < position->game_rules.board_height; row++)
    {
        for (uint8_t col = 0; col < position->game_rules.board_width; col++)
        {
            chesscat_Square square = {.row = row, .col = col};
            chesscat_Move *buf_pos = NULL;
            if (moves_buf != NULL)
            {
                buf_pos = &(moves_buf[move_count]);
            }
            move_count += chesscat_get_possible_moves_from(position, square, buf_pos);
        }
    }
    return move_count;
}

bool _chesscat_can_royal_be_captured(chesscat_Position *position)
{ // Whether a royal can be captured in the given position
    chesscat_Move moves[chesscat_get_all_possible_moves(position, NULL)];
    uint16_t num_moves = chesscat_get_all_possible_moves(position, moves); // TODO: Replace this with something more efficient

    for (uint16_t i = 0; i < num_moves; i++)
    {
        chesscat_Move move = moves[i];
        if (chesscat_get_piece_at_square(position, move.to).is_royal)
        {
            return true;
        }
    }
    return false;
}

/*
 * chesscat_move_pieces
 * 
 * Sets the new positions of pieces according to a move, including captures, castles, and en passants
 * Does not set other positional data such as the color to play or pieces captured
 */
void chesscat_move_pieces(chesscat_Position *position, chesscat_Move move)
{ // Sets piece positions only
    chesscat_Piece empty = {.color = White, .is_royal = false, .type = Empty};
    chesscat_Piece moving = chesscat_get_piece_at_square(position, move.from);

    if (moving.type == King && moving.is_royal && position->game_rules.allow_castle &&
        !position->color_data[moving.color].has_king_moved)
    { // Castling logic
        if (moving.color == White || moving.color == Black)
        {
            int8_t move_dist = move.to.col - move.from.col;
            if (move_dist > 1 && !position->color_data[moving.color].has_upper_rook_moved)
            {
                chesscat_Square rookSquare = _chesscat_find_upper_rook(position, moving.color);
                chesscat_Move rookMove;
                rookMove.from = rookSquare;
                chesscat_Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col - 1};
                rookMove.to = rookSquare2;
                if (_chesscat_is_valid_square(rookSquare))
                {
                    chesscat_move_pieces(position, rookMove);
                }
            }
            if (move_dist < -1 && !position->color_data[moving.color].has_lower_rook_moved)
            {
                chesscat_Square rookSquare = _chesscat_find_lower_rook(position, moving.color);
                chesscat_Move rookMove;
                rookMove.from = rookSquare;
                chesscat_Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col + 1};
                rookMove.to = rookSquare2;
                if (_chesscat_is_valid_square(rookSquare))
                {
                    chesscat_move_pieces(position, rookMove);
                }
            }
        }
        else
        {
            int8_t move_dist = move.from.row - move.to.row;
            if (move_dist > 1 && !position->color_data[moving.color].has_upper_rook_moved)
            {
                chesscat_Square rookSquare = _chesscat_find_upper_rook(position, moving.color);
                chesscat_Move rookMove;
                rookMove.from = rookSquare;
                chesscat_Square rookSquare2 = {.row = move.to.col - 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if (_chesscat_is_valid_square(rookSquare))
                {
                    chesscat_move_pieces(position, rookMove);
                }
            }
            if (move_dist < -1 && position->color_data[moving.color].has_lower_rook_moved)
            {
                chesscat_Square rookSquare = _chesscat_find_upper_rook(position, moving.color);
                chesscat_Move rookMove;
                rookMove.from = rookSquare;
                chesscat_Square rookSquare2 = {.row = move.to.col + 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if (_chesscat_is_valid_square(rookSquare))
                {
                    chesscat_move_pieces(position, rookMove);
                }
            }
        }
    }
    if (moving.type == Pawn)
    {
        if (_chesscat_same_squares(move.to, position->passantable_square))
        {
            chesscat_set_piece_at_square(position, position->passant_target_square, empty);
        }
    }
    chesscat_set_piece_at_square(position, move.to, chesscat_get_piece_at_square(position, move.from));
    chesscat_set_piece_at_square(position, move.from, empty);
}

/*
 * chesscat_make_move
 * 
 * Plays a move in the given position, setting all positional data as required
 */
void chesscat_make_move(chesscat_Position *position, chesscat_Move move, chesscat_EPieceType pawn_promotion)
{
    chesscat_Piece piece = chesscat_get_piece_at_square(position, move.from);
    chesscat_Square none = {.row = -1, .col = -1};
    chesscat_move_pieces(position, move);
    bool pawn_promotes = false;
    position->passant_target_square = none;
    position->passantable_square = none;
    if (piece.type == King)
    {
        _chesscat_MoveCasleType castle_type = _chesscat_move_castles(position, move);
        position->color_data[position->to_move].has_king_moved = true;
        if (castle_type == UpperCastle)
        {
            position->color_data[position->to_move].has_upper_rook_moved = true;
        }
        else if (castle_type == LowerCastle)
        {
            position->color_data[position->to_move].has_lower_rook_moved = true;
        }
    }
    else if (piece.type == Rook)
    {
        chesscat_Square lower_rook = _chesscat_find_lower_rook(position, position->to_move);
        chesscat_Square upper_rook = _chesscat_find_upper_rook(position, position->to_move);
        if (_chesscat_same_squares(lower_rook, move.from))
        {
            position->color_data[position->to_move].has_lower_rook_moved = true;
        }
        else if (_chesscat_same_squares(upper_rook, move.from))
        {
            position->color_data[position->to_move].has_upper_rook_moved = true;
        }
    }
    else if (piece.type == Pawn)
    {
        int8_t col_dist = move.to.col - move.from.col;
        int8_t row_dist = move.to.row - move.from.row;
        if (col_dist > 1)
        {
            chesscat_Square passant = {.row = move.from.row, .col = move.from.col + 1};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        else if (col_dist < -1)
        {
            chesscat_Square passant = {.row = move.from.row, .col = move.from.col - 1};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        if (row_dist > 1)
        {
            chesscat_Square passant = {.row = move.from.row + 1, .col = move.from.col};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        else if (row_dist < -1)
        {
            chesscat_Square passant = {.row = move.from.row - 1, .col = move.from.col};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        if (_chesscat_square_on_promotion_rank(position, move.to, piece.color))
        {
            pawn_promotes = true;
        }
    }
    // TODO: Do Captured_Pieces here
    // TODO: Do Num_Checks here
    if (pawn_promotes)
    {
        chesscat_Piece promotion = {.color = piece.color, .is_royal = false, .type = pawn_promotion};
        chesscat_set_piece_at_square(position, move.to, promotion);
    }
    _chesscat_set_next_to_play(position);
}

/*
 * chesscat_moves_into_check
 * 
 * Returns whether the given move moves into check in the given position
 */
bool chesscat_moves_into_check(chesscat_Position *position, chesscat_Move move)
{
    if (_chesscat_position_ignores_checks(position))
    {
        return false;
    }

    chesscat_Position position_copy = *position;

    chesscat_make_move(&position_copy, move, Pawn);

    return _chesscat_can_royal_be_captured(&position_copy);
}

/*
 * chesscat_is_move_legal
 * 
 * Returns whether the given move is legal in the given position
 * Does not check if a move is actually *possible* - assumes it is
 */
bool chesscat_is_move_legal(chesscat_Position *position, chesscat_Move move, chesscat_EPieceType promotion)
{ // Check if a move meets qualifications to be legal. Does not check if it is POSSIBLE
    chesscat_MovePromotion move_p = {.move = move, .promotion = promotion};
    if (chesscat_moves_into_check(position, move_p.move))
    {
        return false;
    }
    chesscat_Piece moving = chesscat_get_piece_at_square(position, move_p.move.from);
    if (moving.type == Pawn && _chesscat_square_on_promotion_rank(position, move_p.move.to, position->to_move))
    {
        // Promotes
        if (move_p.promotion == King || move_p.promotion == Empty)
        {
            return false;
        }
    }
    // TODO: Check if castle is possible
    return true;
}

/*   chesscat_Game utility functions   */

void chesscat_game_make_move(chesscat_Game *game, chesscat_Move move, chesscat_EPieceType pawn_promotion)
{
    chesscat_make_move(&(game->position), move, pawn_promotion);
    // TODO: chesscat_Move Logging (including pawn promotion log)
}

/*   chesscat_Game creation functions   */

void _chesscat_set_default_rules(chesscat_GameRules *rules)
{
    rules->allow_castle = true;
    rules->allow_passant = true;
    rules->board_height = 8;
    rules->board_width = 8;
    rules->ignore_checks = false;
    rules->allow_castle = true;
    rules->allow_passant = true;
}

void chesscat_set_default_game(chesscat_Game *game)
{
    chesscat_Piece wPawn = {.color = White, .is_royal = false, .type = Pawn};
    chesscat_Piece wKing = {.color = White, .is_royal = true, .type = King};
    chesscat_Piece wQueen = {.color = White, .is_royal = false, .type = Queen};
    chesscat_Piece wRook = {.color = White, .is_royal = false, .type = Rook};
    chesscat_Piece wKnight = {.color = White, .is_royal = false, .type = Knight};
    chesscat_Piece wBishop = {.color = White, .is_royal = false, .type = Bishop};

    chesscat_Piece bPawn = {.color = Black, .is_royal = false, .type = Pawn};
    chesscat_Piece bKing = {.color = Black, .is_royal = true, .type = King};
    chesscat_Piece bQueen = {.color = Black, .is_royal = false, .type = Queen};
    chesscat_Piece bRook = {.color = Black, .is_royal = false, .type = Rook};
    chesscat_Piece bKnight = {.color = Black, .is_royal = false, .type = Knight};
    chesscat_Piece bBishop = {.color = Black, .is_royal = false, .type = Bishop};
    chesscat_Piece empty = {.color = White, .is_royal = false, .type = Empty};

    _chesscat_set_default_rules(&(game->position.game_rules));
    for (uint8_t col = 0; col <= 7; col++)
    {
        _chesscat_set_piece(&(game->position), 1, col, wPawn);
        _chesscat_set_piece(&(game->position), 6, col, bPawn);
        _chesscat_set_piece(&(game->position), 2, col, empty);
        _chesscat_set_piece(&(game->position), 3, col, empty);
        _chesscat_set_piece(&(game->position), 4, col, empty);
        _chesscat_set_piece(&(game->position), 5, col, empty);
    }

    _chesscat_set_piece(&(game->position), 0, 0, wRook);
    _chesscat_set_piece(&(game->position), 0, 1, wKnight);
    _chesscat_set_piece(&(game->position), 0, 2, wBishop);
    _chesscat_set_piece(&(game->position), 0, 3, wQueen);
    _chesscat_set_piece(&(game->position), 0, 4, wKing);
    _chesscat_set_piece(&(game->position), 0, 5, wBishop);
    _chesscat_set_piece(&(game->position), 0, 6, wKnight);
    _chesscat_set_piece(&(game->position), 0, 7, wRook);

    _chesscat_set_piece(&(game->position), 7, 0, bRook);
    _chesscat_set_piece(&(game->position), 7, 1, bKnight);
    _chesscat_set_piece(&(game->position), 7, 2, bBishop);
    _chesscat_set_piece(&(game->position), 7, 3, bQueen);
    _chesscat_set_piece(&(game->position), 7, 4, bKing);
    _chesscat_set_piece(&(game->position), 7, 5, bBishop);
    _chesscat_set_piece(&(game->position), 7, 6, bKnight);
    _chesscat_set_piece(&(game->position), 7, 7, bRook);

    game->position.num_checks[White] = 0;
    game->position.num_checks[Black] = 0;

    chesscat_Square none = {.row = -1, .col = -1};
    game->position.passantable_square = none;
    game->position.color_data[White].is_in_game = true;
    game->position.color_data[White].has_king_moved = false;
    game->position.color_data[White].has_lower_rook_moved = false;
    game->position.color_data[White].has_upper_rook_moved = false;

    game->position.color_data[Black].is_in_game = true;
    game->position.color_data[Black].has_king_moved = false;
    game->position.color_data[Black].has_lower_rook_moved = false;
    game->position.color_data[Black].has_upper_rook_moved = false;
}

/*   Input   */

chesscat_Piece chesscat_get_piece_from_char(char c)
{
    chesscat_Piece making = {.type = Empty, .is_royal = false, .color = White};
    switch (tolower(c))
    {
    case 'p':
        making.type = Pawn;
        break;
    case 'k':
        making.type = King;
        break;
    case 'q':
        making.type = Queen;
        break;
    case 'r':
        making.type = Rook;
        break;
    case 'n':
        making.type = Knight;
        break;
    case 'b':
        making.type = Bishop;
        break;
    default:
        making.type = Empty;
        break;
    }
    if (!isupper(c))
    {
        making.color = Black;
    }
    return making;
}

chesscat_Square chesscat_get_square_from_string(char *str)
{
    chesscat_Square square = {.row = -1, .col = -1};

    int len = strlen(str);
    if (len == 2)
    {
        int rownum = str[1] - '0';
        if (rownum < 1 || rownum > 9)
        {
            return square;
        }
        int colnum = str[0] - 'a';
        if (colnum < 0 || colnum > 25)
        {
            return square;
        }

        square.row = rownum - 1;
        square.col = colnum;
    }
    else if (len == 3)
    {
        int colnum = str[0] - 'a';
        if (colnum < 0 || colnum > 25)
        {
            return square;
        }

        int rownum = atoi(str + 1);
        if (rownum < 1)
        {
            return square;
        }

        square.row = rownum - 1;
        square.col = colnum;
    }

    return square;
}

chesscat_MovePromotion chesscat_get_move_from_string(chesscat_Position *position, char *str)
{
    chesscat_Square none = {.row = -1, .col = -1};
    chesscat_MovePromotion move_p;
    move_p.move.to = none;
    move_p.move.from = none;
    move_p.promotion = Empty;

    chesscat_MovePromotion move_p_final = move_p; // Copy to this before returning. If there is an error in parsing, return without copying

    int len = strlen(str);

    if (len < 2 || len > 9)
    {
        return move_p_final;
    }

    chesscat_Square to_square;

    int16_t read_pos = len;
    if (strchr(str, '=') != NULL)
    {
        // If the string contains an =, set the promotion piece type
        char promo_char = str[len - 1];
        chesscat_Piece promote_to = chesscat_get_piece_from_char(promo_char);
        if (promote_to.type != Empty)
        {
            move_p.promotion = promote_to.type;
        }
        else
        {
            printf("Invalid to promote to %c!\n", promo_char);
            return move_p_final;
        }
        read_pos -= 2;
    }
    read_pos -= 2;
    if (read_pos < 0)
    {
        return move_p_final;
    }
    char square_copy_buf[4];
    strncpy(square_copy_buf, str + read_pos, 2);
    square_copy_buf[2] = '\0';
    to_square = chesscat_get_square_from_string(square_copy_buf);
    if (!_chesscat_is_valid_square(to_square))
    {
        read_pos--;
        if (read_pos < 0)
        {
            return move_p_final;
        }
        strncpy(square_copy_buf, str + read_pos, 3);
        square_copy_buf[3] = '\0';
        to_square = chesscat_get_square_from_string(square_copy_buf);
    }
    if (!_chesscat_is_valid_square(to_square))
    {
        return move_p_final;
    }
len_2:
    if (read_pos == 0)
    {
        // Iterate through possible moves, only one should result in this destination square and be a pawn move.
        chesscat_Move moves[chesscat_get_all_possible_moves(position, NULL)];
        uint16_t num_moves = chesscat_get_all_possible_moves(position, moves); // TODO: Make this more efficient

        for (uint16_t i = 0; i < num_moves; i++)
        {
            chesscat_Move current = moves[i];
            if (!chesscat_is_move_legal(position, current, move_p.promotion))
            {
                continue;
            }
            if (chesscat_get_piece_at_square(position, current.from).type != Pawn)
            {
                continue;
            }
            if (_chesscat_same_squares(current.to, to_square))
            {
                if (_chesscat_is_valid_square(move_p.move.from))
                {
                    // Multiple moves can move to the same square
                    return move_p_final;
                }
                move_p.move = current;
            }
        }

        goto pawn_validate;
    }
    read_pos--;
    if (str[read_pos] == 'x')
    {
        if (read_pos == 0)
        {
            goto len_2;
        }
        read_pos--;
    }
    if (read_pos == 0)
    {
        char lowered = tolower(str[0]);
        if (lowered - 'a' > 25 || lowered - 'a' < 0)
        {
            // Not an alphabet character
            return move_p_final;
        }

        // Iterate through possible moves, this char (first in string) could be a column OR piece.
        // If it's ambiguous, reject the move
        int16_t colhint = -1;
        chesscat_EPieceType piecehint = Empty;

        if (lowered == str[0])
        { // If it's not upper case
            colhint = lowered - 'a';
        }
        piecehint = chesscat_get_piece_from_char(lowered).type;

        if (piecehint == Empty && colhint == -1)
        {
            return move_p_final;
        }

        chesscat_Move moves[chesscat_get_all_possible_moves(position, NULL)];
        uint16_t num_moves = chesscat_get_all_possible_moves(position, moves); // TODO: Make this more efficient

        for (uint16_t i = 0; i < num_moves; i++)
        {
            chesscat_Move current = moves[i];
            if (!chesscat_is_move_legal(position, current, move_p.promotion))
            {
                continue;
            }
            chesscat_Piece moving_piece = chesscat_get_piece_at_square(position, current.from);
            if (move_p.promotion != Empty && moving_piece.type != Pawn)
            {
                continue;
            }
            if (_chesscat_same_squares(current.to, to_square) && (moving_piece.type == piecehint || (current.from.col == colhint && moving_piece.type == Pawn)))
            {
                if (_chesscat_is_valid_square(move_p.move.from))
                {
                    // Multiple moves can move to the same square
                    return move_p_final;
                }
                move_p.move = current;
            }
        }

        goto pawn_validate;
    }
    read_pos--;
    if (read_pos == 0)
    {
        // Iterate through possible moves, first 2 chars in string could be a square OR piece-col OR piece-row.
        // If it's ambiguous or invalid, reject the move
        char char1_lowered = tolower(str[0]);
        if (char1_lowered - 'a' > 25 || char1_lowered - 'a' < 0)
        {
            // Not an alphabet character
            return move_p_final;
        }

        char char2_lowered = tolower(str[1]);
        if (char2_lowered - 'a' <= 25 && char2_lowered - 'a' >= 0)
        {
            // Alphabet character; order must be piece-col
            int16_t colhint = char2_lowered - 'a';
            chesscat_EPieceType piecehint = chesscat_get_piece_from_char(char1_lowered).type;

            if (piecehint == Empty || colhint < 0 || colhint > 25)
            {
                return move_p_final;
            }

            // Iterate through moves to find those with matching cols and piece types
            chesscat_Move moves[chesscat_get_all_possible_moves(position, NULL)];
            uint16_t num_moves = chesscat_get_all_possible_moves(position, moves); // TODO: Make this more efficient

            for (uint16_t i = 0; i < num_moves; i++)
            {
                chesscat_Move current = moves[i];
                if (!chesscat_is_move_legal(position, current, move_p.promotion))
                {
                    continue;
                }
                chesscat_Piece moving_piece = chesscat_get_piece_at_square(position, current.from);
                if (move_p.promotion != Empty && moving_piece.type != Pawn)
                {
                    continue;
                }
                if (_chesscat_same_squares(current.to, to_square) && moving_piece.type == piecehint && current.from.col == colhint)
                {
                    if (_chesscat_is_valid_square(move_p.move.from))
                    {
                        // Multiple moves can move to the same square
                        return move_p_final;
                    }
                    move_p.move = current;
                }
            }

            goto pawn_validate;
        }
        else if (char2_lowered - '0' <= 9 && char2_lowered >= 0)
        {
            // Numerical character; order must be piece-row or col-row
            int16_t colhint = char1_lowered - 'a';
            chesscat_EPieceType piecehint = chesscat_get_piece_from_char(char1_lowered).type;

            if (piecehint == Empty && (colhint < 0 || colhint > 25))
            {
                return move_p_final;
            }

            int16_t rowhint = char2_lowered - '0' - 1;

            if (rowhint < 0 || rowhint > 8)
            {
                return move_p_final;
            }

            // Iterate through moves to find those with matching cols or piece types, and matching rows
            chesscat_Move moves[chesscat_get_all_possible_moves(position, NULL)];
            uint16_t num_moves = chesscat_get_all_possible_moves(position, moves); // TODO: Make this more efficient

            for (uint16_t i = 0; i < num_moves; i++)
            {
                chesscat_Move current = moves[i];
                if (!chesscat_is_move_legal(position, current, move_p.promotion))
                {
                    continue;
                }
                chesscat_Piece moving_piece = chesscat_get_piece_at_square(position, current.from);
                if (move_p.promotion != Empty && moving_piece.type != Pawn)
                {
                    continue;
                }
                if (_chesscat_same_squares(current.to, to_square) && (moving_piece.type == piecehint || current.from.col == colhint) && current.from.row == rowhint)
                {
                    if (_chesscat_is_valid_square(move_p.move.from))
                    {
                        // Multiple moves can move to the same square
                        return move_p_final;
                    }
                    move_p.move = current;
                }
            }
            goto pawn_validate;
        }
        // Invalid order
        return move_p_final;
    }
    read_pos--;
    if (read_pos == 0)
    {
        // First part could be piece-col-row or col-2row (2row is 2-digit row num)
        // TODO: Iterate moves here
        return move_p_final;
    }
    read_pos--;
    if (read_pos == 0)
    {
        // Should always equal 0 because 9-len strings would have to be pawn promotions which wouldn't get this far.
        // The first part here should always be piece-col-2row
    }

pawn_validate:
    if (move_p.promotion != Empty && (chesscat_get_piece_at_square(position, move_p.move.from).type != Pawn || !_chesscat_square_on_promotion_rank(position, move_p.move.to, position->to_move)))
    {
        return move_p_final;
    }
    if (chesscat_get_piece_at_square(position, move_p.move.from).type == Pawn && _chesscat_square_on_promotion_rank(position, move_p.move.to, position->to_move) && move_p.promotion == Empty)
    {
        return move_p_final;
    }
    move_p_final = move_p;
    return move_p_final;
}

/*   Output   */

char chesscat_get_piece_char(chesscat_Piece piece)
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

void _chesscat_print_move(chesscat_Move m)
{
    if (!_chesscat_is_valid_square(m.from) || !_chesscat_is_valid_square(m.to))
    {
        return;
    }
    char getCol[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    printf("%c%d to %c%d\n", getCol[m.from.col], m.from.row + 1, getCol[m.to.col], m.to.row + 1);
}

void _chesscat_print_position(chesscat_Game *game)
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
            printf("%c", chesscat_get_piece_char(chesscat_get_piece_at_square(&(game->position), checking)));
            white = !white;
        }
        printf("\e[m\n");
        white = !white;
    }
}