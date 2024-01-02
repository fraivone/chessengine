/** @file */
#pragma once
#include <boost/algorithm/string.hpp> // tolower
#include "types.hpp"
#include "position.hpp"
# include "colors.hpp"
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