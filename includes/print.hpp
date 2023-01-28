/** @file */
#pragma once
#include <boost/algorithm/string.hpp> // tolower
#include "types.hpp"
#include "position.hpp"
# include "colors.hpp"
# include "pst.hpp"
#include "gen_move.hpp" //movelist
#include "eval.hpp"
#include "hash.hpp"
#include <iomanip> //method setw
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

/// Print a single extmove on the terminal
void PrintExtMove(ExtMove etxmv);
/// Print a single move on the terminal
void PrintMove(Move theMove);

/// Print all the mvoes in the list on the terminal
void PrintMoveList(MoveList s);

/// Print PinMap
void PrintPinMap(Color Us);

/// Print a Piece Square Table to ease the dev
void PrintPST(Color c, PieceType pt);

/// Verbosily print the content of the Transposition Table Entry
void PrintTTEntry(HashTables::TableEntry);

/// Prints the full transposition table
void PrintTranspositionTable();

/// Looks for the current position in the 
/// transposition table. If found, prints the entry 
void PrintThisPositionInTransposition();