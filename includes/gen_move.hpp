/** @file */
#pragma once
#include <position.hpp>
#include <lut.hpp>

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
/// itself as an array and the current_idx, containing the next index to be filled
struct MoveList{
    ExtMove list[MAX_MOVES];
    int current_idx = 0;
    void Add(Move s) {list[current_idx] = s; current_idx++;}
    void Add(ExtMove s) {list[current_idx] = s; current_idx++;}
    void Clear() {current_idx = 0;}
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

/// Convenient method to OR all moves in a movelist 
/// to a bitboard so that it's possible to easily test the move generator.
/// Used in boostTester
Bitboard Moves2Bitboard(const MoveList moveList);