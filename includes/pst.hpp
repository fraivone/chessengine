/** @file */
#pragma once
#include <types.hpp>

// these tables are built based on pawn value
// of 100, which might differ from PawnValue 
// defined in types.hpp 
const int PST_PAWN_VALUE = 100;

const std::array<int,nRows*nCols> null_pst = {
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0,
         0,  0,   0,   0,   0,   0,  0,  0
};

const std::array<int,nRows*nCols> Pawn_pst = {
         0,  0,   0,   0,   0,   0,  0,  0,
        50, 50,  50,  50,  50,  50, 50, 50,
        10, 10,  20,  30,  30,  20, 10, 10,
         5,  5,  10,  25,  25,  10,  5,  5,
         0,  0,   0,  20,  20,   0,  0,  0,
         5, -5, -10,   0,   0, -10, -5,  5,
         5, 10,  10, -20, -20,  10, 10,  5,
         0,  0,   0,   0,   0,   0,  0,  0
};


const std::array<int,nRows*nCols> Knight_pst = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20,   0,   0,   0,   0, -20, -40,
        -30,   0,  10,  15,  15,  10,   0, -30,
        -30,   5,  15,  20,  20,  15,   5, -30,
        -30,   0,  15,  20,  20,  15,   0, -30,
        -30,   5,  10,  15,  15,  10,   5, -30,
        -40, -20,   0,   5,   5,   0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
};

const std::array<int,nRows*nCols> Bishop_pst = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10,   0,   0,   0,   0,   0,   0, -10,
        -10,   0,   5,  10,  10,   5,   0, -10,
        -10,   5,   5,  10,  10,   5,   5, -10,
        -10,   0,  10,  10,  10,  10,   0, -10,
        -10,  10,  10,  10,  10,  10,  10, -10,
        -10,   5,   0,   0,   0,   0,   5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
};

const std::array<int,nRows*nCols> Rook_pst = {
         0,  0,  0,  0,  0,  0,  0,  0,
         5, 10, 10, 10, 10, 10, 10,  5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
        -5,  0,  0,  0,  0,  0,  0, -5,
         0,  0,  0,  5,  5,  0,  0,  0
};

const std::array<int,nRows*nCols> Queen_pst = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10,   0,   0,  0,  0,   0,   0, -10,
        -10,   0,   5,  5,  5,   5,   0, -10,
         -5,   0,   5,  5,  5,   5,   0,  -5,
          0,   0,   5,  5,  5,   5,   0,  -5,
        -10,   5,   5,  5,  5,   5,   0, -10,
        -10,   0,   5,  0,  0,   0,   0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
};

// king middle game
const std::array<int,nRows*nCols> King_pst = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
         20,  20,   0,   0,   0,   0,  20,  20,
         20,  30,  10,   0,   0,  10,  30,  20
};

const std::array<std::array<int,nRows*nCols>,7> PiecesPST = {{null_pst,Pawn_pst,Knight_pst,Bishop_pst,Rook_pst,Queen_pst,King_pst}};

// returns the right value from the right table
// A Word on the symmetry
// PST are currently symm along 2 axes, pretty convenient
// but I have anticipated it won't be always like that
// Indeed the mapping currently only mirrors one direction (up down)
// leaving the other unchanged
// black (row,col) --> WHITE  (8-row,col)
// XOR(sq,56) does the trick
// If you want to have mirroring on both direction (up-down, left-right)
// black (row,col) --> WHITE (8-row,8-col)
// use XOR(sq,63)
inline int getPieceSquareTableValue(Color Us, Square sq, PieceType pt) { 
    return Us==BLACK? PiecesPST[pt][sq] : PiecesPST[pt][(sq^56)]; }