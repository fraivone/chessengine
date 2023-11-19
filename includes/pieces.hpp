// pieces.hpp
#ifndef PIECES_HPP_
#define PIECES_HPP_
#include <iostream>
#include<type_traits>
#include <string>

#define stringify( name ) #name

enum Color : bool {BLACK, WHITE};
enum Castle : signed int { king_side, queen_side };
enum PieceType : signed int { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING };
const std::string convert_enum[] = {
  stringify( PAWN ),
  stringify( BISHOP ),
  stringify( KNIGHT ),
  stringify( ROOK ),
  stringify( QUEEN ),
  stringify( KING )
};

const std::string convert_color[] ={
    "Black", "White"
};

struct Piece{
    std::string name;
    Color color;  // true for white, false for black
    int piece_value;
    PieceType piece_type;

    // Constructor with member initialization
    Piece(const std::string& str, Color b, int i, PieceType t) : name(str), color(b), piece_value(i), piece_type(t) {}

    
    bool operator ==(const Piece& other) const {
        return (this->name == other.name) &&
            (this->color == other.color) &&
            (this->piece_value == other.piece_value) &&
            (this->piece_type == other.piece_type);
    }
};
/* required to hash so that a map can have Piece as key */
template<> struct std::hash<Piece> {
    std::size_t operator()(const Piece& p) const {
        return std::hash<std::string>()(p.name) ^
            std::hash<bool>()(p.color) ^
            std::hash<int>()(p.piece_value) ^ 
            std::hash<int>()(p.piece_type) ;
    }
};
const Piece whitePawn = {"pW", WHITE, 100, PAWN};
const Piece blackPawn = {"pB", BLACK, 100, PAWN};
const Piece whiteRook = {"rW", WHITE, 500, ROOK};
const Piece blackRook = {"rB", BLACK, 500, ROOK};
const Piece whiteBishop = {"bW", WHITE, 330, BISHOP};
const Piece blackBishop = {"bB", BLACK, 330, BISHOP};
const Piece whiteKnight = {"nW", WHITE, 320, KNIGHT};
const Piece blackKnight = {"nB", BLACK, 320, KNIGHT};
const Piece whiteQueen = {"qW", WHITE, 900, QUEEN};
const Piece blackQueen = {"qB", BLACK, 900, QUEEN};
const Piece whiteKing = {"kW", WHITE, 20000, KING};
const Piece blackKing = {"kB", BLACK, 20000, KING};

const Piece AllKindOfPieces[12] = { whitePawn,blackPawn,whiteRook,blackRook,whiteBishop,blackBishop,whiteKnight,blackKnight,whiteQueen,blackQueen,whiteKing,blackKing};
#endif // PIECES_HPP_