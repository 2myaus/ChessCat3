#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "misc.h"

char GetChar(Piece piece){
    char base;
    switch (piece.type) {
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
    if(piece.color == White){
        return toupper(base);
    }
    return base;
}

bool InBounds(Game *game, Square square){
    return square.row >= 0 && square.col >= 0
        && square.row < game->game_rules.board_height && square.col < game->game_rules.board_width;
}

bool IsValidSquare(Square square){
    return square.col > -1 && square.row > -1;
}

void SetPieceAtPos(UniversalPosition *position, int8_t row, int8_t col, Piece piece){
    position->board[row][col] = piece;
}

void SetPiece(UniversalPosition *position, Square square, Piece piece){
    SetPieceAtPos(position, square.row, square.col, piece);
}

Piece GetPieceAtPos(UniversalPosition position, int8_t row, int8_t col){
    return position.board[row][col];
}

Piece GetPiece(UniversalPosition position, Square square){
    return GetPieceAtPos(position, square.row, square.col);
}

Square FindKing(Game *game, Color color){
    for(uint row = 0; row < game->game_rules.board_height; row++){
        for(uint col = 0; col < game->game_rules.board_width; col++){
            Square square = {.row = row, .col = col};
            Piece piece = GetPiece(game->position, square);
            if(piece.type == King && piece.color == color){
                return square;
            }
        }
    }
    Square none = {.row = -1, .col = -1};
    return none;
}

Square FindLowerRook(Game *game, Color color){ //Finds lower-coordinate rook on the specified color's king's rank
    Square none = {.row = -1, .col = -1};
    if(color == White || color == Black){
        Square king_square = FindKing(game, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(int8_t i = king_square.col - 1; i >= 0; i--){
            Square square = {.row = king_square.row, .col = i};
            Piece piece = GetPiece(game->position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    if(color == Green || color == Red){
        Square king_square = FindKing(game, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(int8_t i = king_square.row - 1; i >= 0; i--){
            Square square = {.row = i, .col = king_square.col};
            Piece piece = GetPiece(game->position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    return none;
}

Square FindUpperRook(Game *game, Color color){
    Square none = {.row = -1, .col = -1};
    if(color == White || color == Black){
        Square king_square = FindKing(game, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(uint16_t i = king_square.col + 1; i < game->game_rules.board_width; i++){
            Square square = {.row = king_square.row, .col = i};
            Piece piece = GetPiece(game->position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    if(color == Green || color == Red){
        Square king_square = FindKing(game, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(uint16_t i = king_square.row + 1; i < game->game_rules.board_height; i++){
            Square square = {.row = i, .col = king_square.col};
            Piece piece = GetPiece(game->position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    return none;
}

bool ColorCanCapture(Game *game, Color color, Piece piece){
    return (piece.type == Empty ||
    game->game_rules.capture_own ||
    (piece.color != color)
    ) &&
    piece.type != Duck;
}

uint16_t GetPossiblePieceMoves(Game *game, Square square, Move moves_buf[]){
    bool moves_like_bishop = false;
    bool moves_like_rook = false;
    bool moves_like_knight = false;
    bool moves_like_king = false;
    bool moves_like_pawn = false;

    Piece piece = GetPiece(game->position, square);

    if(piece.color != game->position.to_move || piece.type == Empty){
        return 0;
    }

    uint16_t num_moves = 0;

    switch(piece.type){
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
    
    if(moves_like_king){
        for(int8_t rowto = square.row - 1; rowto <= square.row + 1; rowto++){
            if(rowto < 0 || rowto >= game->game_rules.board_height) {continue;}
            for(int8_t colto = square.col - 1; colto <= square.col + 1; colto++){

                if(colto < 0 || colto >= game->game_rules.board_height) {continue;}
                if(colto == square.col && rowto == square.row) {continue;}
                
                Piece target_piece = GetPieceAtPos(game->position, rowto, colto);
                if(!ColorCanCapture(game, piece.color, target_piece)) {continue;}

                Square tosquare = {.col = colto, .row = rowto};

                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = tosquare;
                num_moves++;
            }
        }
        if(game->game_rules.allow_castle && piece.type == King && piece.is_royal && !game->position.color_data[piece.color].has_king_moved){
            if(!game->position.color_data[piece.color].has_lower_rook_moved){
                Square lower_rook = FindLowerRook(game, piece.color);
                int8_t coldist = abs(square.col - lower_rook.col);
                int8_t rowdist = abs(square.row - lower_rook.row);
                if(IsValidSquare(lower_rook) && (coldist > 1 || rowdist > 1)){
                    if(piece.color == White || piece.color == Black){
                        bool is_blocked = false;
                        for(int8_t col = lower_rook.col + 1; col < square.col; col++){
                            Square checking = {.col = col, .row = square.row};
                            if(GetPiece(game->position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col - 2, .row = square.row};

                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = tosquare;
                            num_moves++;     
                        }                   
                    }
                    else{
                        bool is_blocked = false;
                        for(int8_t row = lower_rook.row + 1; row < square.col; row++){
                            Square checking = {.col = square.col, .row = row};
                            if(GetPiece(game->position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col, .row = square.row - 2};

                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = tosquare;
                            num_moves++;    
                        }
                    }
                }
            }
            if(!game->position.color_data[piece.color].has_upper_rook_moved){
                Square upper_rook = FindUpperRook(game, piece.color);
                int8_t coldist = abs(square.col - upper_rook.col);
                int8_t rowdist = abs(square.row - upper_rook.row);
                if(IsValidSquare(upper_rook) && (coldist > 1 || rowdist > 1)){
                    if(piece.color == White || piece.color == Black){
                        bool is_blocked = false;
                        for(int8_t col = upper_rook.col - 1; col > square.col; col--){
                            Square checking = {.col = col, .row = square.row};
                            if(GetPiece(game->position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col - 2, .row = square.row};

                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = tosquare;
                            num_moves++;     
                        }                   
                    }
                    else{
                        bool is_blocked = false;
                        for(int8_t row = upper_rook.row - 1; row > square.row; row--){
                            Square checking = {.col = square.col, .row = row};
                            if(GetPiece(game->position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col, .row = square.row - 2};

                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = tosquare;
                            num_moves++;    
                        }
                    }
                }
            }
        }
    }
    if(moves_like_knight){
        //ordered as 2-square _ 1-square
        Square up_left = {.col = square.col - 1, .row = square.row + 2};
        Square up_right = {.col = square.col + 1, .row = square.row + 2};

        Square right_up = {.col = square.col + 2, .row = square.row + 1};
        Square right_down = {.col = square.col + 2, .row = square.row - 1};

        Square down_right = {.col = square.col + 1, .row = square.row - 2};
        Square down_left = {.col = square.col - 1, .row = square.row - 2};

        Square left_down = {.col = square.col - 2, .row = square.row - 1};
        Square left_up = {.col = square.col - 2, .row = square.row + 1};

        if(InBounds(game, up_left)) {
            Piece p = GetPiece(game->position, up_left);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = up_left;
                num_moves++;
            }
        }
        if(InBounds(game, up_right)) {
            Piece p = GetPiece(game->position, up_right);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = up_right;
                num_moves++;
            }
        }
        if(InBounds(game, right_up)) {
            Piece p = GetPiece(game->position, right_up);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = right_up;
                num_moves++;
            }
        }
        if(InBounds(game, right_down)) {
            Piece p = GetPiece(game->position, right_down);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = right_down;
                num_moves++;
            }
        }
        if(InBounds(game, down_right)) {
            Piece p = GetPiece(game->position, down_right);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = down_right;
                num_moves++;
            }
        }
        if(InBounds(game, down_left)) {
            Piece p = GetPiece(game->position, down_left);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = down_left;
                num_moves++;
            }
        }
        if(InBounds(game, left_down)) {
            Piece p = GetPiece(game->position, left_down);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = left_down;
                num_moves++;
            }
        }
        if(InBounds(game, left_up)) {
            Piece p = GetPiece(game->position, left_up);
            if(ColorCanCapture(game, piece.color, p)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = left_up;
                num_moves++;
            }
        }
    }
    if(moves_like_bishop){
        Square toSquare = square;
        while (true) { //Up-Right
            toSquare.row++;
            toSquare.col++;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Up-Left
            toSquare.row++;
            toSquare.col--;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down-Right
            toSquare.row--;
            toSquare.col++;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down-Left
            toSquare.row--;
            toSquare.col--;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }
    }
    if(moves_like_rook){
        Square toSquare = square;
        while (true) { //Up
            toSquare.row++;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Left
            toSquare.col--;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down
            toSquare.row--;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }

        toSquare = square;
        while (true) { //Right
            toSquare.col++;
            if (!InBounds(game, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(game->position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(game, piece.color, hitPiece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = toSquare;
                    num_moves++;
                }
                break;
            }
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = toSquare;
            num_moves++;
        }
    }
    if(moves_like_pawn){
        int8_t row_dist = 0;
        int8_t col_dist = 0;
        switch(piece.color){
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

        Square advance_square = {.row = square.row + row_dist, .col = square.col + col_dist};

        if(InBounds(game, advance_square)){
            Square take_left_square = {.row = square.row + row_dist - col_dist, .col = square.col + col_dist - row_dist};
            Square take_right_square = {.row = square.row + row_dist + col_dist, .col = square.col + col_dist + row_dist};

            Piece advance_piece = GetPiece(game->position, advance_square);

            if(advance_piece.type == Empty){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = advance_square;
                num_moves++;
            }
            if(InBounds(game, take_left_square)){
                Piece take_left_piece = GetPiece(game->position, take_left_square);
                if(take_left_piece.type != Empty && ColorCanCapture(game, piece.color, take_left_piece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = take_left_square;
                    num_moves++;
                }
                //TODO: Add en passant
            }
            if(InBounds(game, take_right_square)){
                Piece take_right_piece = GetPiece(game->position, take_right_square);
                if(take_right_piece.type != Empty && ColorCanCapture(game, piece.color, take_right_piece)){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = take_right_square;
                    num_moves++;
                }
                //TODO: Add en passant
            }
            if(game->game_rules.torpedo_pawns ||
            (piece.color == White && square.row <= 1) ||
            (piece.color == Black && square.row >= game->game_rules.board_height - 2) ||
            (piece.color == Green && square.col >= 1) ||
            (piece.color == Red && square.col >= game->game_rules.board_width - 2)
            ){
                Square double_advance_square = {.row = square.row + row_dist * 2, .col = square.col + col_dist * 2};
                if(InBounds(game, double_advance_square)){
                    Piece double_advance_piece = GetPiece(game->position, double_advance_square);
                    if(double_advance_piece.type == Empty){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = double_advance_square;
                        num_moves++;
                    }
                }
            }
            //TODO: Add promotion
        }
        if(game->game_rules.sideways_pawns){
            Square left_square = {.row = square.row - col_dist, .col = square.col - row_dist};
            Square right_square = {.row = square.row + col_dist, .col = square.col + row_dist};

            if(InBounds(game, left_square)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = left_square;
                num_moves++;
            }
            if(InBounds(game, right_square)){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = right_square;
                num_moves++;
            }
        }
    }
    
    return num_moves;
}

uint16_t GetPossibleMoves(Game game, Move moves_buf[]){
    uint16_t move_count = 0;
    for(uint8_t row = 0; row < game.game_rules.board_height; row++){
        for(uint8_t col = 0; col < game.game_rules.board_width; col++){
            Square square = {.row = row, .col = col};
            move_count += GetPossiblePieceMoves(&game, square, &(moves_buf[move_count]));
        }
    }
    return move_count;
}

bool ShouldIgnoreChecks(Game *game){
    return game->game_rules.ignore_checks ||
    game->game_rules.has_duck ||
    game->game_rules.capture_all ||
    game->game_rules.giveaway_mode;
}

void MoveSetPieces(Game *game, Move move) { // Sets piece positions only
    Piece empty = {.color = White, .is_royal = false, .type = Empty};
    Piece moving = game->position.board[move.from.row][move.from.col];
    
    if (moving.type == King && moving.is_royal && game->game_rules.allow_castle &&
        !game->position.color_data[moving.color].has_king_moved) { //Castling logic
        if(moving.color == White || moving.color == Black){
            int8_t move_dist = move.to.col - move.from.col;
            if(move_dist > 1 && !game->position.color_data[moving.color].has_upper_rook_moved){
                Square rookSquare = FindUpperRook(game, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col - 1};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(game, rookMove);
                }
            }
            if(move_dist < -1 && !game->position.color_data[moving.color].has_lower_rook_moved){
                Square rookSquare = FindLowerRook(game, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col + 1};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(game, rookMove);
                }
            }
        }
        else{
            int8_t move_dist = move.from.row - move.to.row;
            if(move_dist > 1 && !game->position.color_data[moving.color].has_upper_rook_moved){
                Square rookSquare = FindUpperRook(game, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = move.to.col - 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(game, rookMove);
                }
            }
            if(move_dist < -1 && !game->position.color_data[moving.color].has_lower_rook_moved){
                Square rookSquare = FindUpperRook(game, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = move.to.col + 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(game, rookMove);
                }
            }
        }
    }
    game->position.board[move.to.row][move.to.col] =
        game->position.board[move.from.row][move.from.col];
    game->position.board[move.from.row][move.from.col] = empty;
}

bool IsMoveLegal(Game *game, Move move){
    if(ShouldIgnoreChecks(game)){
        return true;
    }
    Game game_copy = *game;
    MoveSetPieces(&game_copy, move);
    //TODO: Finish this
    //TODO: REMEMBER TO CHECK IF CASTLE IS LEGAL (MAY BE CUT OFF BY CHECK)
}

void SetDefaultRules(GameRules *rules){
    rules->atomic = false;
    rules->allow_castle = true;
    rules->allow_passant = true;    
    rules->backwards_pawns = false;
    rules->board_height = 8;
    rules->board_width = 8;
    rules->capture_all = false;
    rules->ignore_checks = false;
    rules->capture_own = false;
    rules->check_lives = 0;
    rules->crazyhouse_mode = false;
    rules->giveaway_mode = false;
    rules->has_duck = false;
    rules->promotion_rank = 7;
    rules->sideways_pawns = false;
    rules->torpedo_pawns = false;
    rules->allow_castle = true;
    rules->allow_passant = true;
}

void SetDefaultGame(Game *game){
    Piece wPawn = {.color = White, .is_royal = false, .type = Pawn};
    Piece wKing = {.color = White, .is_royal = true, .type = King};
    Piece wQueen = {.color = White, .is_royal = false, .type = Queen};
    Piece wRook = {.color = White, .is_royal = false, .type = Rook};
    Piece wKnight = {.color = White, .is_royal = false, .type = Knight};
    Piece wBishop = {.color = White, .is_royal = false, .type = Bishop};

    Piece bPawn = {.color = Black, .is_royal = false, .type = Pawn};
    Piece bKing = {.color = Black, .is_royal = true, .type = King};
    Piece bQueen = {.color = Black, .is_royal = false, .type = Queen};
    Piece bRook = {.color = Black, .is_royal = false, .type = Rook};
    Piece bKnight = {.color = Black, .is_royal = false, .type = Knight};
    Piece bBishop = {.color = Black, .is_royal = false, .type = Bishop};
    Piece empty = {.color = White, .is_royal = false, .type = Empty};

    SetDefaultRules(&(game->game_rules));
    for(uint8_t col = 0; col <= 7; col++){
        SetPieceAtPos(&(game->position), 1, col, wPawn);
        SetPieceAtPos(&(game->position), 6, col, bPawn);
        SetPieceAtPos(&(game->position), 2, col, empty);
        SetPieceAtPos(&(game->position), 3, col, empty);
        SetPieceAtPos(&(game->position), 4, col, empty);
        SetPieceAtPos(&(game->position), 5, col, empty);
    }

    SetPieceAtPos(&(game->position), 0, 0, wRook);
    //SetPieceAtPos(&(game->position), 0, 1, wKnight);
    //SetPieceAtPos(&(game->position), 0, 2, wBishop);
    //SetPieceAtPos(&(game->position), 0, 3, wQueen);
    SetPieceAtPos(&(game->position), 0, 4, wKing);
    SetPieceAtPos(&(game->position), 0, 5, wBishop);
    SetPieceAtPos(&(game->position), 0, 6, wKnight);
    SetPieceAtPos(&(game->position), 0, 7, wRook);

    SetPieceAtPos(&(game->position), 7, 0, bRook);
    SetPieceAtPos(&(game->position), 7, 1, bKnight);
    SetPieceAtPos(&(game->position), 7, 2, bBishop);
    SetPieceAtPos(&(game->position), 7, 3, bQueen);
    SetPieceAtPos(&(game->position), 7, 4, bKing);
    SetPieceAtPos(&(game->position), 7, 5, bBishop);
    SetPieceAtPos(&(game->position), 7, 6, bKnight);
    SetPieceAtPos(&(game->position), 7, 7, bRook);

    game->position.num_checks[White] = 0;
    game->position.num_checks[Black] = 0;

    Square none = {.row = -1, .col = -1};
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

void PrintPosition(Game *game){
    for(int8_t row = game->game_rules.board_height - 1; row >= 0; row--){
        for(int8_t col = 0; col < game->game_rules.board_width; col++){
            Square checking = {.row = row, .col = col};
            printf("%c", GetChar(GetPiece(game->position, checking)));
        }
        printf("\n");
    }
}

int main(){
    printf("Piece size: %lu\n", sizeof(Piece));
    printf("Game size: %lu\n", sizeof(Game));
    printf("Square size: %lu\n", sizeof(Square));
    printf("Move size: %lu\n", sizeof(Move));
    
    Game g;
    SetDefaultGame(&g);
    Move move_buf[2048];
    uint16_t num_moves = GetPossibleMoves(g, move_buf);
    printf("Moves: %d\n", num_moves);
    char getCol[8] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'};
    for(uint16_t i = 0; i < num_moves; i++){
        printf("%c%d to %c%d\n", getCol[move_buf[i].from.col], move_buf[i].from.row + 1, getCol[move_buf[i].to.col], move_buf[i].to.row + 1);
    }
    Move castle_move;
    castle_move.from.row = 0;
    castle_move.to.row = 0;

    castle_move.from.col = 4;
    castle_move.to.col = 2;

    MoveSetPieces(&g, castle_move);

    PrintPosition(&g);

    Square check_square = {.row = 0, .col = 3};
    printf("%d\n", GetPiece(g.position, check_square).type);
}