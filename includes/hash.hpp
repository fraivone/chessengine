/** @file */
#pragma once
#include "magic.hpp"

/// handle methods for PRN to work with the Zobrist hashing
/// Zobrist hash has to (almost) uniquely identify a position
/// So we need all pieces positions + enpassant + castling rights + whomoves
/// basically a FEN without the clock numbers
namespace HashTables{
    /// init function to be called at startup 
    /// to initialize the hash variables with PRNs
    void init();
    /// pseudo random numbers for each piece (color+type), each square
    extern Hashkey PRN_pieces[PIECE_NB][nRows*nCols];
    /// pseudo random numbers for each column 
    extern Hashkey PRN_enpassant[nCols];
    /// pseudo random numbers for each of the
    ///  2^4 = 16 combinations of castling right statuses
    extern Hashkey PRN_castling[CASTLING_RIGHT_NB];
    /// pseudo random number for side to play  
    extern Hashkey whomoves;
    // Stockfish uses another PRN for the pawns
    // in order to tell what is the pawn strucutre
    // For now I don't need that
}