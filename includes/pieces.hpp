// pieces.hpp
#ifndef PIECES_HPP_
#define PIECES_HPP_
#include <iostream>
#include <string>

#define stringify( name ) #name

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
    bool color;  // true for white, false for black
    int piece_value;
    PieceType piece_type;

    // Constructor with member initialization
    Piece(const std::string& str, bool b, int i, PieceType t) : name(str), color(b), piece_value(i), piece_type(t) {}

    
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

const Piece whitePawn = {"pW", true, 1, PAWN};
const Piece blackPawn = {"pB", false, 1, PAWN};
const Piece whiteRook = {"rW", true, 5, ROOK};
const Piece blackRook = {"rB", false, 5, ROOK};
const Piece whiteBishop = {"bW", true, 3, BISHOP};
const Piece blackBishop = {"bB", false, 3, BISHOP};
const Piece whiteKnight = {"nW", true, 3, KNIGHT};
const Piece blackKnight = {"nB", false, 3, KNIGHT};
const Piece whiteQueen = {"qW", true, 9, QUEEN};
const Piece blackQueen = {"qB", false, 9, QUEEN};
const Piece whiteKing = {"kW", true, 200, KING};
const Piece blackKing = {"kB", false, 200, KING};


#endif // PIECES_HPP_