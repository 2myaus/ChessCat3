#include <stdint.h>
#include <stdbool.h>

#define CHESSCAT_INCLUDE_MISC_H

#define CHESSCAT_MIN_BITS_REQUIRED(value) ((sizeof(value) * 8) - __builtin_clz(value))

#define CHESSCAT_MAX_BOARD_SIZE 23 //Max board width or height

#define CHESSCAT_NUM_COLORS 4 //Number of colors supported
#define CHESSCAT_NUM_COLOR_BITS CHESSCAT_MIN_BITS_REQUIRED(CHESSCAT_NUM_COLORS - 1) //(Subtract 1 for 0-based numbering)
/*
chesscat_EColor layout:
   B
G     R
   W
*/


#define CHESSCAT_STORE_MAX_MOVES 128 //Max number of moves to store before rewriting earlier moves

typedef enum /* : uint8_t*/{
    Empty, //Colorless
    Pawn,
    King,
    Queen,
    Rook,
    Knight,
    Bishop
    //Duck //Colorless
} chesscat_EPieceType;

typedef enum/* : uint8_t*/{
    White,
    Black,
    Green,
    Red
} chesscat_EColor;

typedef struct {
    bool is_in_game;
    bool has_king_moved : 1;
    bool has_upper_rook_moved : 1;
    bool has_lower_rook_moved : 1;
} _chesscat_ColorData;

typedef struct {
    chesscat_EPieceType type : 5;
    chesscat_EColor color : CHESSCAT_NUM_COLOR_BITS;
    bool is_royal : 1;
} chesscat_Piece;

typedef struct{
    int8_t row;
    int8_t col;
} chesscat_Square;

typedef struct{
    chesscat_Square from;
    chesscat_Square to;
} chesscat_Move;

typedef struct{
    chesscat_Move move;
    chesscat_EPieceType promotion;
} chesscat_MovePromotion;

typedef struct{
    // --chesscat_Game-breaking rules--
    uint8_t board_width;
    uint8_t board_height;

    // --Misc rules--
    bool ignore_checks; //Capture king to win instead of checkmating. Disables check.
    bool capture_own; //Able to capture your own pieces
    //bool sideways_pawns; //Pawns can move sideways
    //bool backwards_pawns; //Pawns can move backwards
    //bool torpedo_pawns; //Pawns can always move 2 spaces forwards
    //uint8_t promotion_rank;
    //uint8_t check_lives; //For n+ Check modes. Set to 0 to disable
    bool capture_all; //Capture all pieces to win
    bool allow_castle;
    bool allow_passant;
    //TODO: Pawn promotion options
    //TODO: Royal chesscat_Piece option

    // --Gamemodes--
    //bool giveaway_mode; //Give away all pieces to win
    //bool crazyhouse_mode; //Place back pieces that you've captured
    //bool has_duck; //🦆
    //bool atomic; //💥

} chesscat_GameRules;

typedef struct{
    chesscat_GameRules game_rules;
    chesscat_EColor to_move : CHESSCAT_NUM_COLOR_BITS;
    chesscat_Square passantable_square; //Should be set to -1 -1 if no square is available
    chesscat_Square passant_target_square; //The pawn to be taken if en passant happens
    _chesscat_ColorData color_data[CHESSCAT_NUM_COLORS]; //Whether the king or rooks have moved
    uint8_t num_checks[CHESSCAT_NUM_COLORS]; //Number of times this color has been checked
    chesscat_Piece board[CHESSCAT_MAX_BOARD_SIZE][CHESSCAT_MAX_BOARD_SIZE]; //0-based array of pieces in [row][col] order
    //chesscat_Piece captured_pieces[CHESSCAT_MAX_BOARD_SIZE * CHESSCAT_MAX_BOARD_SIZE];
} chesscat_Position;

typedef struct{
    chesscat_Position position;
    uint16_t num_moves;
    chesscat_Move moves[CHESSCAT_STORE_MAX_MOVES];
    uint16_t num_promotions;
    chesscat_EPieceType promotions[CHESSCAT_MAX_BOARD_SIZE * CHESSCAT_MAX_BOARD_SIZE]; //Pawn promotions
} chesscat_Game;

typedef enum{
    NotCastle,
    LowerCastle,
    UpperCastle
} _chesscat_EMoveCasleType;

typedef enum{
    Normal,
    Checked,
    Stalemated,
    Checkmated
} chesscat_EPositionState;
