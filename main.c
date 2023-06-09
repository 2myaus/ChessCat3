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

void GetPossiblePieceMoves(Game game, Square square, Move *moves_buf[]){
    bool moves_like_bishop = false;
    bool moves_like_rook = false;
    bool moves_like_knight = false;
    bool moves_like_king = false;
    bool moves_like_pawn = false;

    Piece piece = GetPiece(game.position, square);

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


}

int main(){
    printf("Piece size: %lu\n", sizeof(Piece));
    printf("Game size: %lu\n", sizeof(Game));
}