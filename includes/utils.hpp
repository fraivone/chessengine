#ifndef UTILS_HPP_
#define UTILS_HPP_
#include <iostream>
#include <unordered_map>
#include <vector>
#include "move.hpp"
#include "pieces.hpp"

typedef std::unordered_map<Piece, uint64_t> Board;
typedef std::vector<Move> Moves;

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_LSB(b) (__builtin_ctzll(b))
#define chars_to_bit(letter,number) ((letter - 'a') + ((int(number - '0')) - 1)*8 )

inline int pop_LSB(uint64_t &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}
const uint64_t columnA = 0x101010101010101ULL;
const uint64_t columnH = columnA << 7;
const uint64_t columnAB = columnA | (columnA<<1);
const uint64_t columnGH = columnH | (columnH>>1);

std::string bit2notation(int bit);

#endif