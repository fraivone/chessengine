/** @file */
#pragma once
// #include <cstdint>
#include <string>
#include <iostream>
#include <array>
#include <bitset>
#include <cassert> // for assert
#include <vector> // for assert
const std::string VERSION = "0.1";
const std::string engineName ="ThePaunch";
const std::string author ="Francesco Ivone";
const int defaultDepth = 7;

/// min size of hash table
/// corresponds to guarantee that 
/// the 16 LSB bits of the Zobrist hash
/// are already compared when accessing the table
/// so in a table entry I can store only the 
/// 48 MSB of the ZobristHash
const int MIN_TABLE_SIZE = 65535;

/// Verbose variable to control couts. Ranges from [0,...,5]
extern unsigned VERBOSE;

constexpr int nCols = 8;
constexpr int nRows = 8;
constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY   = 246;
constexpr int ENPSNT_UNAVAILABLE   = 64;



/// 64 bits are enoyugh to store single type single color occupancy on the board
typedef uint64_t Bitboard;
/// Typedef for square. Indicates the bit index of the square
typedef uint8_t Square;
/// Look up table to store 64 bitboards.
/// Helps storing all possible moves, masks, ...
typedef std::array<Bitboard,64> LUT;
/// Hash key for hashtables
typedef uint64_t Hashkey;
typedef std::vector<Hashkey> History;
// Stores  | empty (3) | evalutation sign (1) | evalutation (20) | score tpye (2) | depth (6) | 
typedef uint32_t SearchSummary;
// The evaluation of a node can be Exact, prunedBeta, prunedAlpha
enum ScoreType {
  P_BETA = 1,
  P_ALPHA = 2,
  EXACT = 3
};

/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
/// NOTE: EN-PASSANT bit is set only when a pawn can be captured
///
/// Special cases are MOVE_NONE and MOVE_NULL. We can sneak these in because in
/// any normal move destination square is always different from origin square
/// while MOVE_NONE and MOVE_NULL have the same origin and destination square.

enum Move: uint16_t {
  MOVE_NONE,
  MOVE_NULL = 65
};

/// ok move
constexpr bool is_ok(Move m) { return m != MOVE_NONE && m != MOVE_NULL; }
/// ok square
constexpr bool is_ok(Square sq) { return (sq >= 0 && sq <= 63); }

enum MoveType {
  NORMAL,
  PROMOTION = 1 << 14,
  ENPASSANT = 2 << 14,
  CASTLING  = 3 << 14
};


enum Color {
  WHITE, BLACK, COLOR_NB = 2
};

const int ColorScoreFactor[COLOR_NB] = {1,-1};


/// CastlingRights Bits
// | 0    |  1    |   2   |   3   |
// | W_OO | W_OOO |  B_OO | B_OOO |
/// 

enum CastlingRights {
  NO_CASTLING,
  WHITE_OO,
  WHITE_OOO = WHITE_OO << 1,
  BLACK_OO  = WHITE_OO << 2,
  BLACK_OOO = WHITE_OO << 3,

  KING_SIDE      = WHITE_OO  | BLACK_OO,
  QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
  WHITE_CASTLING = WHITE_OO  | WHITE_OOO,
  BLACK_CASTLING = BLACK_OO  | BLACK_OOO,
  ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

  CASTLING_RIGHT_NB = 16
}; 

// convenient to transform enum into FEN
const std::string castlingString[16] = {"-","K","Q","KQ","k","Kk","Qk","KQk","q","Kq","Qq","KQq","kq","Kkq","Qkq","KQkq"};

//CONVENIENT BIT OPERATIONS
#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_LSB(b) (__builtin_ctzll(b))
#define chars_to_bit(letter,number) ((letter - 'a') + ((number - '0') - 1)*8 )

/*! Counts number of bits set 
    \param uint64_t 
    \return int
*/  
#define countBitsOn(v) (std::bitset<64>(v).count())

inline int pop_LSB(uint64_t &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}
// END CONVENIENT BIT OPERATIONS

const uint64_t columnA = 0x101010101010101ULL;
const uint64_t columnH = columnA << 7;
const uint64_t row1 = 255ULL;
const uint64_t columnAB = columnA | (columnA<<1);
const uint64_t columnGH = columnH | (columnH>>1);
const uint64_t chessboard_border = 18411139144890810879ULL;
const uint64_t chessboard_inner = ~chessboard_border;
const uint64_t vertical_edges = 0x8181818181818181;
const uint64_t horizontal_edges = 0xFF000000000000FF;
const uint64_t ALL_SQUARES = 0xFFFFFFFFFFFFFFFF;


constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;
constexpr Bitboard Files[] = {FileABB,FileBBB,FileCBB,FileDBB,FileEBB,FileFBB,FileGBB,FileHBB};

constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);
constexpr Bitboard Ranks[] = {Rank1BB,Rank2BB,Rank3BB,Rank4BB,Rank5BB,Rank6BB,Rank7BB,Rank8BB};

const Square White_King_Start = 4;
const Square White_Rook_KingSide = 7;
const Square White_Rook_QueenSide = 0;
const Square Black_King_Start = 60;
const Square Black_Rook_KingSide = 63;
const Square Black_Rook_QueenSide = 56;

// Number of squares the king moves when castling
const int Castle_KingSide_KingDelta = 2;
const int Castle_QueenSide_KingDelta = -Castle_KingSide_KingDelta;
const int Castle_KingSide_RookDelta = -Castle_KingSide_KingDelta;
const int Castle_QueenSide_RookDelta = +3;

// convenient square
const int square_fw[COLOR_NB] = {8,-8};
const int square_bw[COLOR_NB] = {-8,+8};

enum Direction : int {
    NORTH = 8,
    EAST  = 1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

// Moves a bitboard one or two steps as specified by the direction D
template<Direction D>
constexpr Bitboard shift(Bitboard b) {
    return D == NORTH         ? b << 8
         : D == SOUTH         ? b >> 8
         : D == NORTH + NORTH ? b << 16
         : D == SOUTH + SOUTH ? b >> 16
         : D == EAST          ? (b & ~FileHBB) << 1
         : D == WEST          ? (b & ~FileABB) >> 1
         : D == NORTH_EAST    ? (b & ~FileHBB) << 9
         : D == NORTH_WEST    ? (b & ~FileABB) << 7
         : D == SOUTH_EAST    ? (b & ~FileHBB) >> 7
         : D == SOUTH_WEST    ? (b & ~FileABB) >> 9
                              : 0;
}

/// Magic bitboards require a mask, a magic number and a magic shift to be used. Each Magic is square specific.
struct Magic {
  Bitboard mask;
  Bitboard magicNumber;
  unsigned shift;
};


/// Struct to hold piece types
enum PieceType {
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 8
};

enum Piece {
    NO_PIECE,
    W_PAWN = PAWN,     W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_NB = 16
};

enum Value : int {
    VALUE_ZERO     = 0,
    VALUE_DRAW     = 0,
    VALUE_NONE     = 31999,
    VALUE_INFINITE = 2147483647,
    // the evalution has a data format 
    // [20-16]Mate in | [15-0] Evaluation
    // The 5 bits [16,20] to store how many plys till mate (mate in [0:30] pyls)
    // The 12 bits [0,11] store the eval value (max 65535)
    // For "MATE IN" the lower the number of plys the higher the value 
    // so that it gets sorted properly as the best move
    // 0 -> 11111(remaining15bits)
    // 1 -> 11110(remaining15bits)
    // 2 -> 11101(remaining15bits)
    // the solution is (VALUE_MATE_0 - mate in plys) << 15
    VALUE_MATE_0 = 31 << 16,
    VALUE_MATE_1 = VALUE_MATE_0 - (1ULL << 16),
    VALUE_MATE_2 = VALUE_MATE_0 - (2ULL << 16),
    // ..
    VALUE_MATE_30 = VALUE_MATE_0 - (30ULL << 16), // max number of plys storable is 30 otherwise we get 0


    // In the code, we make the assumption that these values
    // are such that non_pawn_material() can be used to uniquely
    // identify the material on the board.
    PawnValue   = 208,
    KnightValue = 781,
    BishopValue = 825,
    RookValue   = 1276,
    QueenValue  = 2538,
};

constexpr Value PieceValue[PIECE_NB] = {
  VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO,
  VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO};

// Swap color of piece B_KNIGHT <-> W_KNIGHT
constexpr Piece operator~(Piece pc) { return Piece(pc ^ 8); }

/// row and column in [1,...,8] 
constexpr Square make_square(int column, int row) { return Square(8*(row) + column); }
constexpr Bitboard make_bitboard(Square sq) { is_ok(sq); return 1ULL << sq; }

constexpr Piece make_piece(Color c, PieceType pt) { return Piece((c << 3) + pt); }

constexpr Move make_move(Square from, Square to) { return Move((from << 6) + to); }

template<MoveType T>
constexpr Move make(Square from, Square to, PieceType pt = KNIGHT) {
    return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
}


constexpr PieceType type_of(Piece pc) { return PieceType(pc & 7); }

inline Color color_of(Piece pc) {
    assert(pc != NO_PIECE);
    return Color(pc >> 3);
}

constexpr Square mv_to(Move mv) {return ((mv) & 0x3f); }
constexpr Square mv_from(Move mv) {return ((mv>>6) & 0x3f); }

constexpr bool same_row(Square sq1, Square sq2){  return (sq1/nRows == sq2/nRows)? true: false;}
constexpr bool same_col(Square sq1, Square sq2){  return (sq1%nCols == sq2%nCols)? true: false;}
constexpr bool same_diago(Square sq1, Square sq2){ return abs(sq1%nCols - sq2%nCols) == abs(sq1/nRows - sq2/nRows)? true: false;}


/// convenient string to be match a char to its piece. Used while converting FEN
constexpr std::string_view PieceToChar(" PNBRQK  pnbrqk");
/// convenient array to convert a position to FEN
constexpr char PieceCharMap[COLOR_NB][7] = { {'.','P','N','B','R','Q','K'},{'.','p','n','b','r','q','k'}};

/// Create an array of string representations for piece types
static const char* PieceTypeNames[] = {"NO_PIECE_TYPE", "PAWN", "KNIGHT", "BISHOP", "ROOK", "QUEEN", "KING","ALL_PIECES", "PIECE_TYPE_NB"};
static const char* MoveTypeNames[] = {"NORMAL", "PROMOTION", "ENPASSANT", "CASTLING"};
/// Create an array of string representations for colors
static const char* ColorNames[] = { "WHITE", "BLACK", "COLOR_NB"};
/// String representations for castling rights
inline std::string CastlingNames(int cc) {
    std::string out = "";
    if (cc == 0)
      out += "None";
    else{
      if (get_bit(cc,0))
        out += "WHITE_OO ";
      if (get_bit(cc,1))
        out += "WHITE_OOO ";
      if (get_bit(cc,2))
        out += "BLACK_OO ";
      if (get_bit(cc,3))
        out += "BLACK_OOO ";
    }
    return out;
}

/// Create an array of string representations for pieces
static const char* PieceNames[] = { "NULL", "WHITE PAWN", "WHITE KNIGHT", "WHITE BISHOP","WHITE ROOK","WHITE QUUEN","WHITE KING",
                                    "NULL", "BLACK PAWN", "BLACK KNIGHT", "BLACK BISHOP","BLACK ROOK","BLACK QUUEN","BLACK KING"};


// StateInfo struct stores information needed to restore a Position object to
// its previous state when we retract a move. Whenever a move is made on the
// board (by calling Position::do_move), a StateInfo object must be passed.

struct StateInfo {

    // Copied when making a move
    /// To be understood
    uint64_t    materialKey;
    /// To be understood
    uint64_t    pawnKey;
    int  nonPawnMaterial[COLOR_NB];
    int  PSTScore[COLOR_NB];
    int    castlingRights;
    // number of plys since last pawn move or last capture
    int    rule50;
    int    pliesFromNull;
    /// en passant square
    Square epSquare;

    // Not copied when making a move (will be recomputed anyhow)
    /// Hash key of this positions
    Hashkey        ZobristHash;
    /// To be understood
    Bitboard   checkersBB;
    StateInfo* previous;
    Bitboard   blockersForKing[COLOR_NB];
    Bitboard   pinners[COLOR_NB];
    Bitboard   checkSquares[PIECE_TYPE_NB];
    Piece      capturedPiece;
    int        repetition;
};

// Convenient FEN
const std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";