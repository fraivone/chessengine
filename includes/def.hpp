// def.hpp
#ifndef DEF_HPP_
#define DEF_HPP_
#include <iostream>
#include<type_traits>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <utility>
#include <limits>

// MISC CONSTANTS
const float Mate_Score = 1000;
const int16_t  nAllPieces = 12;
const int16_t  nPieceTypes = 6;
const int PLUS_INF = std::numeric_limits<int>::max();
const int MINUS_INF = std::numeric_limits<int>::min();
// END MISC CONSTANTS

// COLOR
enum Color : bool {BLACK, WHITE};
const Color ColorArray[2] = { BLACK, WHITE}; 
const std::string convert_color[] ={"Black", "White"};
// define ! operator for Color
inline Color operator!(Color value){
    if(value==WHITE)
        return BLACK;
    else
        return WHITE;
}
inline int ColorMultiplier(Color c){
    return ((c)*2 -1);
}
//END COLOR

// CASTLE
enum Castle : signed int { king_side, queen_side };
// END CASTE

// PIECE 
enum PieceType : signed int { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING, NOTHING };
const PieceType piecetypes_array[nPieceTypes] = { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING}; 

#define stringify( name ) #name
const std::string convert_enum[] = {
  stringify( PAWN ),
  stringify( BISHOP ),
  stringify( KNIGHT ),
  stringify( ROOK ),
  stringify( QUEEN ),
  stringify( KING ),
  stringify( NOTHING ),
};

struct Piece{
    char name;
    Color color;  // true for white, false for black
    int piece_value;
    PieceType piece_type;
    std::uint16_t index = static_cast<int>(piece_type) << 1 | static_cast<int>(color)  ; // |- - -|-| (3bit piece type, 1 bit oclor)

    // Constructor with member initialization
    Piece(const char c, Color b, int i, PieceType t) : name(c), color(b), piece_value(i), piece_type(t) {}
    
    bool operator ==(const Piece& other) const {
        return (this->color == other.color) &&
            (this->piece_type == other.piece_type);
    }
};
/* required to hash so that a map can have Piece as key */
template<> struct std::hash<Piece> {
    std::size_t operator()(const Piece& p) const {
        return std::hash<char>()(p.name) ^
            std::hash<bool>()(p.color) ^
            std::hash<int>()(p.piece_value) ^ 
            std::hash<int>()(p.piece_type) ;
    }
};
typedef std::array<uint64_t,64> LUT;
const int ColorlessKing = KING << 1;

const Piece whitePawn = {'P', WHITE, 100, PAWN};
const Piece blackPawn = {'p', BLACK, 100, PAWN};
const Piece whiteRook = {'R', WHITE, 500, ROOK};
const Piece blackRook = {'r', BLACK, 500, ROOK};
const Piece whiteBishop = {'B', WHITE, 330, BISHOP};
const Piece blackBishop = {'b', BLACK, 330, BISHOP};
const Piece whiteKnight = {'N', WHITE, 320, KNIGHT};
const Piece blackKnight = {'n', BLACK, 320, KNIGHT};
const Piece whiteQueen = {'Q', WHITE, 900, QUEEN};
const Piece blackQueen = {'q', BLACK, 900, QUEEN};
const Piece whiteKing = {'K', WHITE, 20000, KING};
const Piece blackKing = {'k', BLACK, 20000, KING};
const Piece nullPiece = {'0',BLACK,0,NOTHING};
// pieces ordered by index so that KindOfPieces[piece.index] == piece
const Piece pieces_array[nAllPieces] = { blackPawn,whitePawn,blackBishop,whiteBishop,blackKnight,whiteKnight,blackRook,whiteRook,blackQueen,whiteQueen,blackKing,whiteKing}; 

const std::unordered_map<char, Piece> KindOfPiecesLUT = {
        {'P',whitePawn},
        {'B',whiteBishop},
        {'N',whiteKnight},
        {'R',whiteRook},
        {'Q',whiteQueen},
        {'K',whiteKing},
        {'p',blackPawn},
        {'b',blackBishop},
        {'n',blackKnight},
        {'r',blackRook},
        {'q',blackQueen},
        {'k',blackKing},
    };
// END PIECE

// BOARD NOTATION
typedef std::unordered_map<Piece, uint64_t> Board;
const std::string bit2notation[64] = {
"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"
};

struct BoardStatus{
    Board board;
    Color whoMoves;
    bool castle_rights[2][2];
    uint8_t enpassant_bit;
    uint8_t MoveCounter;
    uint8_t ply;
    int check = -1;
    int draw = -1;
    int mate = -1;
    BoardStatus(){
        castle_rights[0][0] = false;
        castle_rights[0][1] = false;
        castle_rights[1][0] = false;
        castle_rights[1][1] = false;
    }
};

// END BOARD NOTATION


// MOVE
struct Move{
    int initial_bit;
    int final_bit;
    Piece piece;
    Piece captured_piece;
    bool isValid;
    bool isChecked;
    int guessedScore;
    // Constructor with member initialization
    Move(const int ii, int ff, Piece p) : initial_bit(ii), final_bit(ff), piece(p), captured_piece(nullPiece), isValid(false), isChecked(false), guessedScore(0) {}
};
const Move NULL_MOVE{0,0,nullPiece};
typedef std::vector<Move> Moves;
typedef std::pair<float, Move> ScorenMove;


inline void printMove(Move m){
    std::cout << "The " << convert_color[m.piece.color] << " "
          << convert_enum[m.piece.piece_type]
          << " moves from " << bit2notation[m.initial_bit]
          << " to " << bit2notation[m.final_bit]
          << " capturing " 
          << convert_color[m.captured_piece.color] << " " << convert_enum[m.captured_piece.piece_type]
          <<std::endl;
}
// END MOVE


//CONVENIENT BIT
#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_LSB(b) (__builtin_ctzll(b))
#define chars_to_bit(letter,number) ((letter - 'a') + ((number - '0') - 1)*8 )
#define countBitsOn(v) (std::bitset<64>(v).count())

inline int pop_LSB(uint64_t &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}
const uint64_t columnA = 0x101010101010101ULL;
const uint64_t columnH = columnA << 7;
const uint64_t columnAB = columnA | (columnA<<1);
const uint64_t columnGH = columnH | (columnH>>1);
// END CONVENIENT BIT 


#endif // DEF_HPP_