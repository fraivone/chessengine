/** @file */
#pragma once
#include <position.hpp>
#include <lut.hpp>
#include <magic.hpp>

/// structure to hold a move and its score(?)
struct ExtMove {
    Move move;
    int  value;

    operator Move() const { return move; }
    void operator=(Move m) { move = m; }

    // Inhibit unwanted implicit conversions to Move
    // with an ambiguity that yields to a compile error.
    operator float() const = delete;
};

/// Structure to store the ExtMove. Contains a the list 
/// itself as an array and the size. The size is different from MAX SIZE which is set to MAX MOVES
struct MoveList{
    ExtMove list[MAX_MOVES];
    int size = 0;
    // Append a move to this move list
    void Add(Move s) {list[size] = s; size++;}
    // Append a ExtMove to this move list
    void Add(ExtMove s) {list[size] = s; size++;}
    void Append(MoveList s) {for(int i=0; i<s.size; i++){list[size] = s.list[i];size++;}}
    // pop element from this move list
    void Pop(int index) {if(index<size){std::move(list + index + 1, list + size, list + index );size--;}}
    void Clear() {size = 0;}
};


/// Method to help promoting a pawn a given square to another to all possible pieces
/// taken from stockfish, simplified 
MoveList make_promotions(MoveList& moveList,Square from, Square to);

// template function to work with different piece types
// it returns a move as defined in types. 
// for pawns I need to know if I en passant is allowed, what is the occupancy of the enemy, what is my occupancy
// for kings and knights I need to know what is the occupancy of the enemy, what is my occupancy
// for queens, bishops and rooks I need to know what is the occupancy of the enemy, what is my occupancy

/// Pawns all pseudomoves including captures, fw, 2fw, enpassant and promotions
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contains the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
MoveList PawnAnyMoves(MoveList& moveList, Color c, Square s);

/// Pawns all pseudomoves including captures, fw, 2fw and enpassant
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contain the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
MoveList PawnAnyMoves(MoveList& moveList, Square s);

/// Convenient method to OR all attacked squares in a movelist 
/// to a bitboard so that it's possible to easily test the move generator.
/// Used in boostTester
Bitboard Attacked2Bitboard(const MoveList moveList);
/// Convenient method to OR all starting squares currently attacking
Bitboard Attackers2Bitboard(const MoveList moveList);
/// Convenient method to OR all starting squares currently attacking target
Bitboard Attackers2Bitboard(const MoveList moveList, Bitboard target);

/// given a pieceType generate pseudomoves
/// requires magic bitboards to be init
MoveList generate_pseudomoves(MoveList&,Color, Square, const PieceType);

/// generate all pseudomoves in this postion
MoveList generate_all(MoveList&,Color);

/// generate all pseudomoves in this postion for PieceTyep
MoveList generate_all(MoveList&,Color, const PieceType);

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers arbitrary number of piece types
template<typename... PieceTypes>
inline MoveList generate_all(MoveList& moveList,Color Us, PieceType pt, PieceTypes... pts) {
    moveList = generate_all(moveList, Us, pt);
    moveList = generate_all(moveList, Us, pts...);
    return moveList;
}