#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

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

Piece GetPieceFromChar(char c){
    Piece making = {.type = Empty, .is_royal = false, .color = White};
    switch (tolower(c)) {
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
    if(!isupper(c)){
        making.color = Black;
    }
    return making;
}

Square GetSquareFromString(char* str){
    Square square = {.row = -1, .col = -1};

    int len = strlen(str);
    if(len == 2){
        int rownum = str[1] - '0';
        if(rownum < 1 || rownum > 9){
            return square;
        }
        int colnum = str[0] - 'a';
        if(colnum < 0 || colnum > 25){
            return square;
        }

        square.row = rownum - 1;
        square.col = colnum;
    }
    else if(len == 3){
        int colnum = str[0] - 'a';
        if(colnum < 0 || colnum > 25){
            return square;
        }

        int rownum = atoi(str + 1);
        if(rownum < 1){
            return square;
        }

        square.row = rownum - 1;
        square.col = colnum;
    }

    return square;
}

bool SameSquare(Square s1, Square s2){
    return s1.row == s2.row && s1.col == s2.col;
}

bool SameMove(Move m1, Move m2){
    return SameSquare(m1.from, m2.from) && SameSquare(m1.to, m2.to);
}

bool InBounds(UniversalPosition *position, Square square){
    return square.row >= 0 && square.col >= 0
        && square.row < position->game_rules.board_height && square.col < position->game_rules.board_width;
}

bool IsValidSquare(Square square){
    return square.col > -1 && square.row > -1;
}

bool IsOnPromotionRank(UniversalPosition *position, Square square, Color color){
    switch (color) {
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

void SetPieceAtPos(UniversalPosition *position, int8_t row, int8_t col, Piece piece){
    position->board[row][col] = piece;
}

void SetPiece(UniversalPosition *position, Square square, Piece piece){
    SetPieceAtPos(position, square.row, square.col, piece);
}

Piece GetPieceAtPos(UniversalPosition *position, int8_t row, int8_t col){
    return position->board[row][col];
}

Piece GetPiece(UniversalPosition *position, Square square){
    return GetPieceAtPos(position, square.row, square.col);
}

Square FindKing(UniversalPosition *position, Color color){
    for(uint16_t row = 0; row < position->game_rules.board_height; row++){
        for(uint16_t col = 0; col < position->game_rules.board_width; col++){
            Square square = {.row = row, .col = col};
            Piece piece = GetPiece(position, square);
            if(piece.type == King && piece.color == color){
                return square;
            }
        }
    }
    Square none = {.row = -1, .col = -1};
    return none;
}

Square FindLowerRook(UniversalPosition *position, Color color){ //Finds lower-coordinate rook on the specified color's king's rank
    Square none = {.row = -1, .col = -1};
    if(color == White || color == Black){
        Square king_square = FindKing(position, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(int8_t i = king_square.col - 1; i >= 0; i--){
            Square square = {.row = king_square.row, .col = i};
            Piece piece = GetPiece(position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    if(color == Green || color == Red){
        Square king_square = FindKing(position, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(int8_t i = king_square.row - 1; i >= 0; i--){
            Square square = {.row = i, .col = king_square.col};
            Piece piece = GetPiece(position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    return none;
}

Square FindUpperRook(UniversalPosition *position, Color color){
    Square none = {.row = -1, .col = -1};
    if(color == White || color == Black){
        Square king_square = FindKing(position, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(uint16_t i = king_square.col + 1; i < position->game_rules.board_width; i++){
            Square square = {.row = king_square.row, .col = i};
            Piece piece = GetPiece(position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    if(color == Green || color == Red){
        Square king_square = FindKing(position, color);
        if(!IsValidSquare(king_square)) { return none; }
        for(uint16_t i = king_square.row + 1; i < position->game_rules.board_height; i++){
            Square square = {.row = i, .col = king_square.col};
            Piece piece = GetPiece(position, square);
            if(piece.type == Rook && piece.color == color){
                return square;
            }
        }
    }
    return none;
}

bool ColorCanCapture(UniversalPosition *position, Color color, Piece piece){
    return (piece.type == Empty || piece.color != color);
}

uint16_t GetPossiblePieceMoves(UniversalPosition *position, Square square, Move moves_buf[]){
    bool moves_like_bishop = false;
    bool moves_like_rook = false;
    bool moves_like_knight = false;
    bool moves_like_king = false;
    bool moves_like_pawn = false;

    Piece piece = GetPiece(position, square);

    if(piece.color != position->to_move || piece.type == Empty){
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
            if(rowto < 0 || rowto >= position->game_rules.board_height) {continue;}
            for(int8_t colto = square.col - 1; colto <= square.col + 1; colto++){

                if(colto < 0 || colto >= position->game_rules.board_height) {continue;}
                if(colto == square.col && rowto == square.row) {continue;}
                
                Piece target_piece = GetPieceAtPos(position, rowto, colto);
                if(!ColorCanCapture(position, piece.color, target_piece)) {continue;}

                Square tosquare = {.col = colto, .row = rowto};
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = tosquare;
                }
                num_moves++;
            }
        }
        if(position->game_rules.allow_castle && piece.type == King && piece.is_royal && !position->color_data[piece.color].has_king_moved){
            if(!position->color_data[piece.color].has_lower_rook_moved){
                Square lower_rook = FindLowerRook(position, piece.color);
                int8_t coldist = abs(square.col - lower_rook.col);
                int8_t rowdist = abs(square.row - lower_rook.row);
                if(IsValidSquare(lower_rook) && (coldist > 1 || rowdist > 1)){
                    if(piece.color == White || piece.color == Black){
                        bool is_blocked = false;
                        for(int8_t col = lower_rook.col + 1; col < square.col; col++){
                            Square checking = {.col = col, .row = square.row};
                            if(GetPiece(position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col - 2, .row = square.row};

                            if(moves_buf != NULL){
                                moves_buf[num_moves].from = square;
                                moves_buf[num_moves].to = tosquare;
                            }
                            num_moves++;     
                        }                   
                    }
                    else{
                        bool is_blocked = false;
                        for(int8_t row = lower_rook.row + 1; row < square.col; row++){
                            Square checking = {.col = square.col, .row = row};
                            if(GetPiece(position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col, .row = square.row - 2};

                            if(moves_buf != NULL){
                                moves_buf[num_moves].from = square;
                                moves_buf[num_moves].to = tosquare;
                            }
                            num_moves++;    
                        }
                    }
                }
            }
            if(!position->color_data[piece.color].has_upper_rook_moved){
                Square upper_rook = FindUpperRook(position, piece.color);
                int8_t coldist = abs(square.col - upper_rook.col);
                int8_t rowdist = abs(square.row - upper_rook.row);
                if(IsValidSquare(upper_rook) && (coldist > 1 || rowdist > 1)){
                    if(piece.color == White || piece.color == Black){
                        bool is_blocked = false;
                        for(int8_t col = upper_rook.col - 1; col > square.col; col--){
                            Square checking = {.col = col, .row = square.row};
                            if(GetPiece(position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col - 2, .row = square.row};

                            if(moves_buf != NULL){
                                moves_buf[num_moves].from = square;
                                moves_buf[num_moves].to = tosquare;
                            }
                            num_moves++;     
                        }                   
                    }
                    else{
                        bool is_blocked = false;
                        for(int8_t row = upper_rook.row - 1; row > square.row; row--){
                            Square checking = {.col = square.col, .row = row};
                            if(GetPiece(position, checking).type != Empty){
                                is_blocked = true;
                                break;
                            }
                        }
                        if(!is_blocked){
                            Square tosquare = {.col = square.col, .row = square.row - 2};

                            if(moves_buf != NULL){
                                moves_buf[num_moves].from = square;
                                moves_buf[num_moves].to = tosquare;
                            }
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

        if(InBounds(position, up_left)) {
            Piece p = GetPiece(position, up_left);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = up_left;
                }
                num_moves++;
            }
        }
        if(InBounds(position, up_right)) {
            Piece p = GetPiece(position, up_right);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = up_right;
                }
                num_moves++;
            }
        }
        if(InBounds(position, right_up)) {
            Piece p = GetPiece(position, right_up);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = right_up;
                }
                num_moves++;
            }
        }
        if(InBounds(position, right_down)) {
            Piece p = GetPiece(position, right_down);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = right_down;
                }
                num_moves++;
            }
        }
        if(InBounds(position, down_right)) {
            Piece p = GetPiece(position, down_right);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = down_right;
                }
                num_moves++;
            }
        }
        if(InBounds(position, down_left)) {
            Piece p = GetPiece(position, down_left);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = down_left;
                }
                num_moves++;
            }
        }
        if(InBounds(position, left_down)) {
            Piece p = GetPiece(position, left_down);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = left_down;
                }
                num_moves++;
            }
        }
        if(InBounds(position, left_up)) {
            Piece p = GetPiece(position, left_up);
            if(ColorCanCapture(position, piece.color, p)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = left_up;
                }
                num_moves++;
            }
        }
    }
    if(moves_like_bishop){
        Square toSquare = square;
        while (true) { //Up-Right
            toSquare.row++;
            toSquare.col++;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Up-Left
            toSquare.row++;
            toSquare.col--;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down-Right
            toSquare.row--;
            toSquare.col++;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down-Left
            toSquare.row--;
            toSquare.col--;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }
    }
    if(moves_like_rook){
        Square toSquare = square;
        while (true) { //Up
            toSquare.row++;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Left
            toSquare.col--;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Down
            toSquare.row--;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
            num_moves++;
        }

        toSquare = square;
        while (true) { //Right
            toSquare.col++;
            if (!InBounds(position, toSquare)) {
                break;
            }
            Piece hitPiece = GetPiece(position, toSquare);
            if (hitPiece.type != Empty) {
                if (ColorCanCapture(position, piece.color, hitPiece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = toSquare;
                    }
                    num_moves++;
                }
                break;
            }
            if(moves_buf != NULL){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = toSquare;
            }
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

        if(InBounds(position, advance_square)){
            Square take_left_square = {.row = square.row + row_dist - col_dist, .col = square.col + col_dist - row_dist};
            Square take_right_square = {.row = square.row + row_dist + col_dist, .col = square.col + col_dist + row_dist};

            Piece advance_piece = GetPiece(position, advance_square);

            if(advance_piece.type == Empty){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = advance_square;
                }
                num_moves++;
            }
            if(InBounds(position, take_left_square)){
                Piece take_left_piece = GetPiece(position, take_left_square);
                if(take_left_piece.type != Empty && ColorCanCapture(position, piece.color, take_left_piece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_left_square;
                    }
                    num_moves++;
                }
                if(SameSquare(take_left_square, position->passantable_square)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_left_square;
                    }
                    num_moves++;
                }
            }
            if(InBounds(position, take_right_square)){
                Piece take_right_piece = GetPiece(position, take_right_square);
                if(take_right_piece.type != Empty && ColorCanCapture(position, piece.color, take_right_piece)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_right_square;
                    }
                    num_moves++;
                }
                if(SameSquare(take_right_square, position->passantable_square)){
                    if(moves_buf != NULL){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = take_right_square;
                    }
                    num_moves++;
                }
            }
            /*
            if(position->game_rules.torpedo_pawns ||
            (piece.color == White && square.row <= 1) ||
            (piece.color == Black && square.row >= position->game_rules.board_height - 2) ||
            (piece.color == Green && square.col >= 1) ||
            (piece.color == Red && square.col >= position->game_rules.board_width - 2)
            ){
                Square double_advance_square = {.row = square.row + row_dist * 2, .col = square.col + col_dist * 2};
                if(InBounds(position, double_advance_square)){
                    Piece double_advance_piece = GetPiece(position, double_advance_square);
                    if(double_advance_piece.type == Empty){
                        if(moves_buf != NULL){
                            moves_buf[num_moves].from = square;
                            moves_buf[num_moves].to = double_advance_square;
                        }
                        num_moves++;
                    }
                }
            }
            */
            //TODO: Add promotion
        }
        /*
        if(position->game_rules.sideways_pawns){
            Square left_square = {.row = square.row - col_dist, .col = square.col - row_dist};
            Square right_square = {.row = square.row + col_dist, .col = square.col + row_dist};

            if(InBounds(position, left_square)){
                if(moves_buf != NULL){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = left_square;
                }
                num_moves++;
            }
            if(InBounds(position, right_square)){
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

uint16_t GetPossibleMoves(UniversalPosition *position, Move moves_buf[]){
    uint16_t move_count = 0;
    for(uint8_t row = 0; row < position->game_rules.board_height; row++){
        for(uint8_t col = 0; col < position->game_rules.board_width; col++){
            Square square = {.row = row, .col = col};
            Move *buf_pos = NULL;
            if(moves_buf != NULL){
                buf_pos = &(moves_buf[move_count]);
            }
            move_count += GetPossiblePieceMoves(position, square, buf_pos);
        }
    }
    return move_count;
}

bool ShouldIgnoreChecks(UniversalPosition *position){
    if(position->game_rules.ignore_checks){
        return true;
    }
    uint8_t num_colors = 0;
    if(position->color_data[White].is_in_game) { num_colors++; }
    if(position->color_data[Black].is_in_game) { num_colors++; }
    if(position->color_data[Red].is_in_game) { num_colors++; }
    if(position->color_data[Green].is_in_game) { num_colors++; }

    if(num_colors > 2){
        return true;
    }
    return false;
}

void MoveSetPieces(UniversalPosition *position, Move move) { // Sets piece positions only
    Piece empty = {.color = White, .is_royal = false, .type = Empty};
    Piece moving = GetPiece(position, move.from);
    
    if (moving.type == King && moving.is_royal && position->game_rules.allow_castle &&
        !position->color_data[moving.color].has_king_moved) { //Castling logic
        if(moving.color == White || moving.color == Black){
            int8_t move_dist = move.to.col - move.from.col;
            if(move_dist > 1 && !position->color_data[moving.color].has_upper_rook_moved){
                Square rookSquare = FindUpperRook(position, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col - 1};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(position, rookMove);
                }
            }
            if(move_dist < -1 && !position->color_data[moving.color].has_lower_rook_moved){
                Square rookSquare = FindLowerRook(position, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = rookSquare.row, .col = move.to.col + 1};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(position, rookMove);
                }
            }
        }
        else{
            int8_t move_dist = move.from.row - move.to.row;
            if(move_dist > 1 && !position->color_data[moving.color].has_upper_rook_moved){
                Square rookSquare = FindUpperRook(position, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = move.to.col - 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(position, rookMove);
                }
            }
            if(move_dist < -1 && position->color_data[moving.color].has_lower_rook_moved){
                Square rookSquare = FindUpperRook(position, moving.color);
                Move rookMove;
                rookMove.from = rookSquare;
                Square rookSquare2 = {.row = move.to.col + 1, .col = rookSquare.col};
                rookMove.to = rookSquare2;
                if(IsValidSquare(rookSquare)){
                    MoveSetPieces(position, rookMove);
                }
            }
        }
    }
    if (moving.type == Pawn){
        if(SameSquare(move.to, position->passantable_square)){
            SetPiece(position, position->passant_target_square, empty);
        }     
    }
    SetPiece(position, move.to, GetPiece(position, move.from));
    SetPiece(position, move.from, empty);
}

void SetNextToPlay(UniversalPosition *position){
    if(position->color_data[White].is_in_game == false &&
    position->color_data[Black].is_in_game == false &&
    position->color_data[Green].is_in_game == false &&
    position->color_data[Red].is_in_game == false){
        return;
    }
    switch (position->to_move) {
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
    while(position->color_data[position->to_move].is_in_game == false || !IsValidSquare(FindKing(position, position->to_move))){
        switch (position->to_move) {
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

MoveCastleType GetCastleType(UniversalPosition *position, Move move){
    int8_t coldist = move.to.col - move.from.col;
    int8_t rowdist = move.to.row - move.from.row;

    Piece piece = GetPiece(position, move.from);
    if(piece.type == King && piece.is_royal && (abs(rowdist) > 1 || abs(coldist > 1))){
        if(rowdist > 0 || coldist > 0){
            return UpperCastle;
        }
        if(rowdist < 0 || coldist < 0){
            return LowerCastle;
        }        
    }
    return NotCastle;
}

bool CanCaptureRoyal(UniversalPosition *position){ // Whether a royal can be captured in the given position
    Move moves[GetPossibleMoves(position, NULL)];
    uint16_t num_moves = GetPossibleMoves(position, moves); //TODO: Replace this with something more efficient

    for(uint16_t i = 0; i < num_moves; i++){
        Move move = moves[i];
        if(GetPiece(position, move.to).is_royal){
            return true;
        }
    }
    return false;
}

void MakeMove(UniversalPosition *position, Move move, PieceType pawn_promotion){
    Piece piece = GetPiece(position, move.from);
    Square none = {.row = -1, .col = -1};
    bool pawn_promotes = false;
    position->passant_target_square = none;
    position->passantable_square = none;
    if(piece.type == King){
        MoveCastleType castle_type = GetCastleType(position, move);
        position->color_data[position->to_move].has_king_moved = true;
        if(castle_type == UpperCastle){
            position->color_data[position->to_move].has_upper_rook_moved = true;
        }
        else if(castle_type == LowerCastle){
            position->color_data[position->to_move].has_lower_rook_moved = true;
        }
    }
    else if(piece.type == Rook){
        Square lower_rook = FindLowerRook(position, position->to_move);
        Square upper_rook = FindUpperRook(position, position->to_move);
        if(SameSquare(lower_rook, move.from)){
            position->color_data[position->to_move].has_lower_rook_moved = true;
        }
        else if(SameSquare(upper_rook, move.from)){
            position->color_data[position->to_move].has_upper_rook_moved = true;
        }
    }
    else if(piece.type == Pawn){
        int8_t col_dist = move.to.col - move.from.col;
        int8_t row_dist = move.to.row - move.from.row;
        if(col_dist > 1){
            Square passant = {.row = move.from.row, .col = move.from.col + 1};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        else if(col_dist < -1){
            Square passant = {.row = move.from.row, .col = move.from.col - 1};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        if(row_dist > 1){
            Square passant = {.row = move.from.row + 1, .col = move.from.col};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        else if(row_dist < -1){
            Square passant = {.row = move.from.row - 1, .col = move.from.col};
            position->passantable_square = passant;
            position->passant_target_square = move.to;
        }
        if(IsOnPromotionRank(position, move.to, piece.color)){
            pawn_promotes = true;
        }
    }    
    //TODO: Do Captured_Pieces here
    //TODO: Do Num_Checks here
    MoveSetPieces(position, move);
    if(pawn_promotes){
        Piece promotion = {.color = piece.color, .is_royal = false, .type = pawn_promotion};
        SetPiece(position, move.to, promotion);
    }
    SetNextToPlay(position);
}



MovePromotion GetMoveFromString(UniversalPosition *position, char* str){
    Square none = {.row = -1, .col = -1};
    MovePromotion move_p;
    move_p.move.to = none;
    move_p.move.from = none;
    move_p.promotion = Empty;

    MovePromotion move_p_final; //Copy to this before returning. If there is an error in parsing, return without copying

    int len = strlen(str);

    if(len < 2 || len > 6) {return move_p;}

    Square to_square;

    uint8_t read_pos = len;

    if(strchr(str, '=') != NULL){
        Piece promote_to = GetPieceFromChar(*(str + len - 1));
        if(promote_to.type != Empty){
            move_p.promotion = promote_to.type;
        }
        else{
            return move_p_final;
        }
        read_pos -= 2;
    }
    read_pos -= 2;
    to_square = GetSquareFromString(str + read_pos);
    if(!IsValidSquare(to_square)){
        return move_p_final;
    }
    //TODO: Finish
}

void GameMakeMove(Game *game, Move move, PieceType pawn_promotion){
    MakeMove(&(game->position), move, pawn_promotion);
    //TODO: Move Logging (including pawn promotion log)
}

bool MovesIntoCheck(UniversalPosition *position, Move move){
    if(ShouldIgnoreChecks(position)){
        return false;
    }

    UniversalPosition position_copy = *position;

    MakeMove(&position_copy, move, Pawn);

    return CanCaptureRoyal(&position_copy);
}

bool IsMoveLegal(UniversalPosition *position, Move move){
    if(MovesIntoCheck(position, move)){
        return false;
    }
    Move moves[GetPossiblePieceMoves(position, move.from, NULL)];
    uint16_t num_moves = GetPossiblePieceMoves(position, move.from, NULL); //TODO: Replace this with something more efficient

    for(uint16_t i = 0; i < num_moves; i++){
        if(SameMove(move, moves[i])){
            return true;
        }
    }

    //TODO: REMEMBER TO CHECK IF CASTLE IS LEGAL (MAY BE CUT OFF BY CHECK)
    return false;
}

void SetDefaultRules(GameRules *rules){
    rules->allow_castle = true;
    rules->allow_passant = true;
    rules->board_height = 8;
    rules->board_width = 8;
    rules->ignore_checks = false;
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

    SetDefaultRules(&(game->position.game_rules));
    for(uint8_t col = 0; col <= 7; col++){
        SetPieceAtPos(&(game->position), 1, col, wPawn);
        SetPieceAtPos(&(game->position), 6, col, bPawn);
        SetPieceAtPos(&(game->position), 2, col, empty);
        SetPieceAtPos(&(game->position), 3, col, empty);
        SetPieceAtPos(&(game->position), 4, col, empty);
        SetPieceAtPos(&(game->position), 5, col, empty);
    }

    SetPieceAtPos(&(game->position), 0, 0, wRook);
    SetPieceAtPos(&(game->position), 0, 1, wKnight);
    SetPieceAtPos(&(game->position), 0, 2, wBishop);
    SetPieceAtPos(&(game->position), 0, 3, wQueen);
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
    for(int8_t row = game->position.game_rules.board_height - 1; row >= 0; row--){
        for(int8_t col = 0; col < game->position.game_rules.board_width; col++){
            Square checking = {.row = row, .col = col};
            printf("%c", GetChar(GetPiece(&(game->position), checking)));
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
    uint16_t num_moves = GetPossibleMoves(&g.position, move_buf);
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

    //MoveSetPieces(&g, castle_move);
    //MakeMove(&g, castle_move);

    PrintPosition(&g);

    if(IsValidSquare(GetSquareFromString(""))){
        printf("Val\n");
    }
}