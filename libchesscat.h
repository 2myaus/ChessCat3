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
    // Game-breaking rules--
    uint8_t board_width;
    uint8_t board_height;

    // --Misc rules--
    bool ignore_checks; //Capture king to win instead of checkmating. Disables check.
    bool capture_own; //Able to capture your own pieces
    bool sideways_pawns; //Pawns can move sideways
    bool kangaroo_pawns; //Pawns can jump over the piece in front of them
    //bool backwards_pawns; //Pawns can move backwards
    bool torpedo_pawns; //Pawns can always move 2 spaces forwards
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

/* main.c */
bool _chesscat_same_squares(chesscat_Square s1, chesscat_Square s2);
bool _chesscat_same_move(chesscat_Move m1, chesscat_Move m2);
bool chesscat_is_valid_square(chesscat_Square square);
bool chesscat_is_valid_move(chesscat_Move m);
bool chesscat_square_in_bounds(chesscat_Position *position, chesscat_Square square);
bool _chesscat_square_on_promotion_rank(chesscat_Position *position, chesscat_Square square, chesscat_EColor color);
bool _chesscat_position_ignores_checks(chesscat_Position *position);
void _chesscat_set_piece(chesscat_Position *position, int8_t row, int8_t col, chesscat_Piece piece);
void chesscat_set_piece_at_square(chesscat_Position *position, chesscat_Square square, chesscat_Piece piece);
chesscat_Piece _chesscat_get_piece(chesscat_Position *position, int8_t row, int8_t col);
chesscat_Piece chesscat_get_piece_at_square(chesscat_Position *position, chesscat_Square square);
chesscat_Square _chesscat_find_king(chesscat_Position *position, chesscat_EColor color);
chesscat_Square _chesscat_find_lower_rook(chesscat_Position *position, chesscat_EColor color);
chesscat_Square _chesscat_find_upper_rook(chesscat_Position *position, chesscat_EColor color);
void _chesscat_set_next_to_play(chesscat_Position *position);
_chesscat_EMoveCasleType _chesscat_move_castles(chesscat_Position *position, chesscat_Move move);
bool _chesscat_color_can_capture_piece(chesscat_Position *position, chesscat_EColor color, chesscat_Piece piece);
void _chesscat_add_move_to_buf(chesscat_Move move, chesscat_Move *moves_buf[], uint16_t *num_moves);
uint16_t chesscat_get_possible_moves_from(chesscat_Position *position, chesscat_Square square, chesscat_Move moves_buf[]);
uint16_t chesscat_get_all_possible_moves(chesscat_Position *position, chesscat_Move moves_buf[]);
uint16_t chesscat_get_all_legal_moves(chesscat_Position *position, chesscat_Move moves_buf[]);
uint16_t chesscat_get_legal_moves_from(chesscat_Position *position, chesscat_Square square, chesscat_Move moves_buf[]);
bool _chesscat_can_royal_be_captured(chesscat_Position *position);
void chesscat_move_pieces(chesscat_Position *position, chesscat_Move move);
void chesscat_make_move(chesscat_Position *position, chesscat_Move move, chesscat_EPieceType pawn_promotion);
bool chesscat_moves_into_check(chesscat_Position *position, chesscat_Move move);
bool chesscat_is_move_legal(chesscat_Position *position, chesscat_Move move, chesscat_EPieceType promotion);
bool chesscat_is_move_possible(chesscat_Position *position, chesscat_Move move);
chesscat_EPositionState chesscat_get_current_state(chesscat_Position *position);
void chesscat_game_make_move(chesscat_Game *game, chesscat_Move move, chesscat_EPieceType pawn_promotion);
void _chesscat_set_default_rules(chesscat_GameRules *rules);
void chesscat_set_default_game(chesscat_Game *game);
int chesscat_set_game_to_FEN(chesscat_Game *game, char* FEN);
chesscat_Piece chesscat_get_piece_from_char(char c);
chesscat_Square chesscat_get_square_from_string(char *str);
chesscat_MovePromotion chesscat_get_move_from_string(chesscat_Position *position, char *str);
