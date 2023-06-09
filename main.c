#include <stdio.h>
#include <stdbool.h>

#include "misc.h"

bool InBounds(Game game, Square square){
    return square.row >= 0 && square.col >= 0
        && square.row < game.game_rules.board_height && square.col < game.game_rules.board_width;
}

void SetPieceAtPos(UniversalPosition position, int8_t row, int8_t col, Piece piece){
    position.board[row][col] = piece;
}

void SetPiece(UniversalPosition position, Square square, Piece piece){
    SetPieceAtPos(position, square.row, square.col, piece);
}

Piece GetPieceAtPos(UniversalPosition position, int8_t row, int8_t col){
    return position.board[row][col];
}

Piece GetPiece(UniversalPosition position, Square square){
    return GetPieceAtPos(position, square.row, square.col);
}

uint16_t GetPossiblePieceMoves(Game game, Square square, Move moves_buf[]){
    bool moves_like_bishop = false;
    bool moves_like_rook = false;
    bool moves_like_knight = false;
    bool moves_like_king = false;
    bool moves_like_pawn = false;

    Piece piece = GetPiece(game.position, square);

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
    }
    
    if(moves_like_king){
        for(int8_t rowto = square.row - 1; rowto <= square.row + 1; rowto++){
            if(rowto < 0 || rowto >= game.game_rules.board_height) {continue;}
            for(int8_t colto = square.col - 1; colto <= square.col + 1; colto++){

                if(colto < 0 || colto >= game.game_rules.board_height) {continue;}
                if(colto == square.col && rowto == square.row) {continue;}
                
                Piece target_piece = GetPieceAtPos(game.position, rowto, colto);
                if(target_piece.color == piece.color) {continue;}

                Square tosquare = {.col = colto, .row = rowto};

                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = tosquare;
                num_moves++;
            }
        }
    }
    if(moves_like_knight){
        //ordered in 2 square_1 square
        Square up_left = {.col = square.col - 1, .row = square.row + 2};
        Square up_right = {.col = square.col + 1, .row = square.row + 2};

        Square right_up = {.col = square.col + 2, .row = square.row + 1};
        Square right_down = {.col = square.col + 2, .row = square.row - 1};

        Square down_right = {.col = square.col + 1, .row = square.row - 2};
        Square down_left = {.col = square.col - 1, .row = square.row - 2};

        Square left_down = {.col = square.col - 2, .row = square.row - 1};
        Square left_up = {.col = square.col - 2, .row = square.row + 1};

        if(InBounds(game, up_left)) {
            Piece p = GetPiece(game.position, up_left);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = up_left;
                num_moves++;
            }
        }
        if(InBounds(game, up_right)) {
            Piece p = GetPiece(game.position, up_right);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = up_right;
                num_moves++;
            }
        }
        if(InBounds(game, right_up)) {
            Piece p = GetPiece(game.position, right_up);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = right_up;
                num_moves++;
            }
        }
        if(InBounds(game, right_down)) {
            Piece p = GetPiece(game.position, right_down);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = right_down;
                num_moves++;
            }
        }
        if(InBounds(game, down_right)) {
            Piece p = GetPiece(game.position, down_right);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = down_right;
                num_moves++;
            }
        }
        if(InBounds(game, down_left)) {
            Piece p = GetPiece(game.position, down_left);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = down_left;
                num_moves++;
            }
        }
        if(InBounds(game, left_down)) {
            Piece p = GetPiece(game.position, left_down);
                if(p.type == Empty || p.color != piece.color){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = left_down;
                num_moves++;
            }
        }
        if(InBounds(game, left_up)) {
            Piece p = GetPiece(game.position, left_up);
                if(p.type == Empty || p.color != piece.color){
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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
            Piece hitPiece = GetPiece(game.position, toSquare);
            if (hitPiece.type != Empty) {
                if (hitPiece.color != piece.color) {
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

            Piece advance_piece = GetPiece(game.position, advance_square);

            if(advance_piece.type == Empty){
                moves_buf[num_moves].from = square;
                moves_buf[num_moves].to = advance_square;
                num_moves++;
            }
            if(InBounds(game, take_left_square)){
                Piece take_left_piece = GetPiece(game.position, take_left_square);
                if(take_left_piece.type != Empty && take_left_piece.color != piece.color){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = take_left_square;
                    num_moves++;
                }
                //TODO: Add en passant
            }
            if(InBounds(game, take_right_square)){
                Piece take_right_piece = GetPiece(game.position, take_right_square);
                if(take_right_piece.type != Empty && take_right_piece.color != piece.color){
                    moves_buf[num_moves].from = square;
                    moves_buf[num_moves].to = take_right_square;
                    num_moves++;
                }
                //TODO: Add en passant
            }
            if(game.game_rules.torpedo_pawns ||
            (piece.color == White && square.row <= 1) ||
            (piece.color == Black && square.row >= game.game_rules.board_height - 2) ||
            (piece.color == Green && square.col >= 1) ||
            (piece.color == Red && square.col >= game.game_rules.board_width - 2)
            ){
                Square double_advance_square = {.row = square.row + row_dist * 2, .col = square.col + col_dist * 2};
                if(InBounds(game, double_advance_square)){
                    Piece double_advance_piece = GetPiece(game.position, double_advance_square);
                    if(double_advance_piece.type == Empty){
                        moves_buf[num_moves].from = square;
                        moves_buf[num_moves].to = double_advance_square;
                        num_moves++;
                    }
                }
            }
            //TODO: Add promotion
        }
    }
    
    return num_moves;
}

int main(){
    printf("Piece size: %lu\n", sizeof(Piece));
    printf("Game size: %lu\n", sizeof(Game));
}