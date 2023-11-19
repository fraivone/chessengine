//lookuptable 
#ifndef LUT_HPP_
#define LUT_HPP_
#include <iostream>
#include "utils.hpp"
#include "pieces.hpp"
#include <cstdint>
#include <array>

/*
8   56 # # # # # # # # 63
7   48 # # # # # # # # 55
6   40 # # # # # # # # 47
5   32 # # # # # # # # 39
4   24 # # # # # # # # 31
3   16 # # # # # # # # 23
2   8  # # # # # # # # 15
1   0  # # # # # # # # 7 
    A       ...        H
*/

// 2 types of pieces
// sliding: rook, bishop, queens
// can go to a certain square if
// 1. It's on their sliding direction
// 2. There are no obstructing pieces along that direct
// leaping: king, pawn, knight
// can go to a certain square if
// 1. It's on their leaping direction
// for each piece, calculate the landing squares
// ignoring all the other chess rules
// TODO PAWN movement depends on the opponents pieces

std::array<uint64_t,64> wpawn_straight_lut();
std::array<uint64_t,64> bpawn_straight_lut();
std::array<uint64_t,64> wpawn_diagcapture_lut();
std::array<uint64_t,64> bpawn_diagcapture_lut();
std::array<uint64_t,64> knight_position_lut();
std::array<uint64_t,64> king_position_lut();


uint64_t straight_lines(int, uint64_t, uint64_t);
uint64_t diago_lines(int, uint64_t, uint64_t);

uint64_t rook_landings(int, uint64_t, uint64_t);
uint64_t bishop_landings(int, uint64_t, uint64_t);
uint64_t queen_landings(int, uint64_t, uint64_t);

#endif // LUT_HPP_