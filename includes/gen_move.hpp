/** @file */
#pragma once
#include <position.hpp>
#include <lut.hpp>

// template function to work with different piece types
// it returns a move as defined in types. 
// for pawns I need to know if I en passant is allowed, what is the occupancy of the enemy, what is my occupancy
// for kings and knights I need to know what is the occupancy of the enemy, what is my occupancy
// for queens, bishops and rooks I need to know what is the occupancy of the enemy, what is my occupancy

/// Pawns all pseudomoves including captures, fw, 2fw and enpassant
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contain the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
Bitboard PawnAnyMoves(Color c, Square s); 

/// Pawns all pseudomoves including captures, fw, 2fw and enpassant
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contain the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
Bitboard PawnAnyMoves(Square s);