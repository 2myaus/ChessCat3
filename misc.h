#include <stdint.h>
#include <stdbool.h>

 typedef enum /* : uint8_t*/{
    Pawn,
    King,
    Queen,
    Rook,
    Knight,
    Bishop
} PieceType;

typedef enum/* : uint8_t*/{
    White,
    Black
} Color;

typedef struct {
    PieceType type : 5;
    Color color : 2;
    bool is_royal : 1;
} Piece;

typedef struct{
    int8_t row;
    int8_t col;
} Square;

typedef struct{
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

} GameRules;

typedef struct{
    Piece board[24][24];
} Position;