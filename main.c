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
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = up_left;
            num_moves++;
        }
        if(InBounds(game, up_right)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = up_right;
            num_moves++;
        }
        if(InBounds(game, right_up)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = right_up;
            num_moves++;
        }
        if(InBounds(game, right_down)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = right_down;
            num_moves++;
        }
        if(InBounds(game, down_right)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = down_right;
            num_moves++;
        }
        if(InBounds(game, down_left)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = down_left;
            num_moves++;
        }
        if(InBounds(game, left_down)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = left_down;
            num_moves++;
        }
        if(InBounds(game, left_up)) {
            moves_buf[num_moves].from = square;
            moves_buf[num_moves].to = left_up;
            num_moves++;
        }
    }
    

    return num_moves;
}

int main(){
    printf("Piece size: %lu\n", sizeof(Piece));
    printf("Game size: %lu\n", sizeof(Game));
}