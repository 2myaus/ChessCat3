#include <stdint.h>
#include <stdbool.h>

enum PieceType : uint8_t{
    Pawn,
    King,
    Queen,
    Rook,
    Knight,
    Bishop
} typedef PieceType;

enum Color : uint8_t{
    White,
    Black
} typedef Color;

struct Piece{
    PieceType type : 5;
    Color color : 2;
    bool is_royal : 1;
} typedef Piece;

struct Square{
    int8_t row;
    int8_t col;
} typedef Square;

struct GameRules{
    // --Game-breaking rules--
    uint8_t board_width;
    uint8_t board_height;

    // --Misc rules--
    bool capture_king; //Capture king to win instead of checkmating
    bool capture_own; //Able to capture your own pieces
    bool sideways_pawns; //Pawns can move sideways
    bool backwards_pawns;
    bool torpedo_pawns; //Pawns can always move 2 spaces forwards
    uint8_t promotion_rank;
    uint8_t check_lives; //For n+ Check modes. Set to 0 to disable
    //TODO: Pawn promotion options
    //TODO: Royal Piece option

    // --Gamemodes--
    bool giveaway_mode; //Give away all pieces to win
    bool crazyhouse_mode; //Place back pieces that you've captured
    bool has_duck; //🦆
    bool atomic; //💥

} typedef GameRules;

struct Position{
    Piece board[24][24];
} typedef Position;