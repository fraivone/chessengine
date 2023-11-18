// move.hpp
#ifndef MOVE_HPP_
#define MOVE_HPP_
#include <iostream>
#include <string>
#include "pieces.hpp"

// struct Move{
//     int initial_bit;
//     int final_bit;
//     Piece piece;
//     // Constructor with member initialization
//     Move(const int ii, int ff, Piece p) : initial_bit(ii), final_bit(ff), piece(p) {}
// };

struct Move{
    int initial_bit;
    int final_bit;
    Piece piece;
    bool isCheck;
    // Constructor with member initialization
    Move(const int ii, int ff, Piece p, bool cc) : initial_bit(ii), final_bit(ff), piece(p), isCheck(cc) {}
};

#endif // MOVE_HPP_