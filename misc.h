#include <stdint.h>
#include <stdbool.h>

#define MIN_BITS_REQUIRED(value) ((sizeof(value) * 8) - __builtin_clz(value))

#define MAX_BOARD_SIZE 24 //Max board width or height
#define NUM_COLORS 4 //Number of colors supported
#define NUM_COLOR_BITS MIN_BITS_REQUIRED(NUM_COLORS - 1) //(Subtract 1 for 0-based numbering)

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
    Black,
    Green,
    Red
} Color;

typedef struct {
    PieceType type : 5;
    Color color : NUM_COLOR_BITS;
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
    Color to_move : NUM_COLOR_BITS;
    Square passantable_square; //Should be set to -1 -1 if no square is available
    Piece board[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    Piece captured_pieces[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
} Position;

typedef struct{
    GameRules game_rules;
    Position position;
} Game;