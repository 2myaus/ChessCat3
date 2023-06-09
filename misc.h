#include <stdint.h>
#include <stdbool.h>

#define MIN_BITS_REQUIRED(value) ((sizeof(value) * 8) - __builtin_clz(value))

#define MAX_BOARD_SIZE 24 //Max board width or height

#define NUM_COLORS 4 //Number of colors supported
#define NUM_COLOR_BITS MIN_BITS_REQUIRED(NUM_COLORS - 1) //(Subtract 1 for 0-based numbering)
/*
Color layout:
   B
G     R
   W
*/


#define STORE_MAX_MOVES 128 //Max number of moves to store before rewriting earlier moves

 typedef enum /* : uint8_t*/{
    Empty, //Colorless
    Pawn,
    King,
    Queen,
    Rook,
    Knight,
    Bishop,
    Duck //Colorless
} PieceType;

typedef enum/* : uint8_t*/{
    White,
    Black,
    Green,
    Red
} Color;

typedef struct {
    bool has_king_moved : 1;
    bool has_king_rook_moved : 1;
    bool has_queen_rook_moved : 1;
} ColorData;

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
    Square from;
    Square to;
} Move;

typedef struct{
    // --Game-breaking rules--
    uint8_t board_width;
    uint8_t board_height;

    // --Misc rules--
    bool capture_king; //Capture king to win instead of checkmating. Disables check.
    bool capture_own; //Able to capture your own pieces
    bool sideways_pawns; //Pawns can move sideways
    bool backwards_pawns; //Pawns can move backwards
    bool torpedo_pawns; //Pawns can always move 2 spaces forwards
    uint8_t promotion_rank;
    uint8_t check_lives; //For n+ Check modes. Set to 0 to disable
    bool capture_all; //Capture all pieces to win
    //TODO: Pawn promotion options
    //TODO: Royal Piece option

    // --Gamemodes--
    bool giveaway_mode; //Give away all pieces to win
    bool crazyhouse_mode; //Place back pieces that you've captured
    bool has_duck;//🦆
    bool atomic; //💥

} GameRules;

typedef struct{
    Color to_move : NUM_COLOR_BITS;
    Square passantable_square; //Should be set to -1 -1 if no square is available
    ColorData color_data[NUM_COLORS]; //Whether the king or rooks have moved
    Square duck_position;
    uint8_t num_checks[NUM_COLORS]; //Number of times this color has been checked
    Piece board[MAX_BOARD_SIZE][MAX_BOARD_SIZE]; //0-based array of pieces in [row][col] order
    Piece captured_pieces[MAX_BOARD_SIZE * MAX_BOARD_SIZE];
} UniversalPosition;

typedef struct{
    GameRules game_rules;
    UniversalPosition position;
    Move moves[STORE_MAX_MOVES];
} Game;