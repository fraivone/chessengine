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

std::array<uint64_t,64> wpawn_straight_lut();
std::array<uint64_t,64> bpawn_straight_lut();
std::array<uint64_t,64> wpawn_diagcapture_lut();
std::array<uint64_t,64> bpawn_diagcapture_lut();
std::array<uint64_t,64> knight_position_lut();
std::array<uint64_t,64> king_position_lut();

uint64_t opponentboard_status(Board, bool );
uint64_t ownboard_status(Board, bool );

uint64_t straight_moves(int, Board, bool);
uint64_t diago_moves(int, Board, bool);

uint64_t rook_moves(int, Board, bool);
uint64_t bishop_moves(int, Board, bool);
uint64_t queen_moves(int, Board, bool);

#endif // LUT_HPP_