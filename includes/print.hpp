/** @file */
#pragma once
#include <boost/algorithm/string.hpp> // tolower
#include "types.hpp"
#include "position.hpp"
# include "colors.hpp"
#include <gen_move.hpp> //movelist
/*! Prints the given bitboard with # in place of the set bits.
    \param Bitboard 
    \return void
*/  
void RepresentBitset(Bitboard);

/*! Prints the board with Piece Types and colors
    \param Piece
    \return void
*/  
void RepresentBoard();

/// Print a single move on the terminal
void PrintMove(Move theMove);

/// Print all the mvoes in the list on the terminal
void PrintMoveList(MoveList s);