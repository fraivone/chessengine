/** @file */
#pragma once
#include <iostream>
#include <types.hpp>
#include <sstream> // stringstream for FEN
#include <cstring> // mmset for initializing stateinfo
// #include <lut.hpp>
#include <gen_move.hpp>

 /*!  \todo 
    * - Include method to generate FEN given the bitboards and the status of the current position
 */

/// namespace to encapsulate the variables needed for position
namespace Position{
    void init();
    /// holds occupancy for all pieces on the board, one per square. 64 piece array
    extern Piece board[nCols*nRows];
    /// holds occupancy for each piece type, 8 bitboards
    extern Bitboard BitboardsByType[PIECE_TYPE_NB];
    /// holds occupancy for each color, 2 bitboards
    extern Bitboard BitboardsByColor[COLOR_NB];
    /// who moves    
    extern Color sideToMove;
    /// state
    extern StateInfo st;
    /// move ply count
    extern int gamePly;
}

/// init position
void init_position(std::string FEN);

/// Update castling rights in the state info
void set_castling_right(Color c, char FEN_char);


/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers one piece type
inline Bitboard pieces(PieceType pt)  { return Position::BitboardsByType[pt]; }

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers arbitrary number of piece types
template<typename... PieceTypes>
inline Bitboard pieces(PieceType pt, PieceTypes... pts) {
    return pieces(pt) | pieces(pts...);
}

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers one color
inline Bitboard pieces(Color c)  { return Position::BitboardsByColor[c]; }

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers considers a specific piece type / color combo
template<typename... PieceTypes>
inline Bitboard pieces(Color c, PieceTypes... pts)  {
    return pieces(c) & pieces(pts...);
}

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers considers all pieces
inline Bitboard pieces(){return (Position::BitboardsByColor[WHITE] | Position::BitboardsByColor[BLACK]);}

/// adds a piece on the board
void put_piece(Square square, Piece PP);
/// overloaded method to remove a piece from the board. 
/// Takes a square / piece
void remove_piece(Square square, Piece pp);
/// overloaded method to remove a piece from the board. 
/// Takes a square only
void remove_piece(Square square);
/// overloaded method to move a piece on the board
void move_piece(Square startsquare,Square endsquare, Piece PP);
/// overloaded method to move a piece on the board
void move_piece(Square startsquare,Square endsquare);

/// Read FEN string and update the Bitboards
void setBBFromFEN(std::string FEN);
/// Read FEN string and update status
void setStatusFromFEN(std::string FEN);