/** @file */
#pragma once
#include <position.hpp>
#include <lut.hpp>

// template function to work with different piece types
// it returns a move as defined in types. 
// for pawns I need to know if I en passant is allowed, what is the occupancy of the enemy, what is my occupancy
// for kings and knights I need to know what is the occupancy of the enemy, what is my occupancy
// for queens, bishops and rooks I need to know what is the occupancy of the enemy, what is my occupancy

/// Pawns all legal moves including captures, fw, 2fw and enpassant
/// not checking for checks or mate
Bitboard PawnAnyMove(Color c, Square s); 