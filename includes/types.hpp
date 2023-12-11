#pragma once
// #include <cstdint>
#include <string>
#include <iostream>
#include <array>

typedef uint64_t Bitboard;
// Look up table containing the possible moves
typedef std::array<uint64_t,64> LUT;

/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
/// NOTE: EN-PASSANT bit is set only when a pawn can be captured
///
/// Special cases are MOVE_NONE and MOVE_NULL. We can sneak these in because in
/// any normal move destination square is always different from origin square
/// while MOVE_NONE and MOVE_NULL have the same origin and destination square.

enum Move {
  MOVE_NONE,
  MOVE_NULL = 65
};

enum MoveType {
  NORMAL,
  PROMOTION = 1 << 14,
  ENPASSANT = 2 << 14,
  CASTLING  = 3 << 14
};


enum Color {
  WHITE, BLACK, COLOR_NB = 2
};

enum CastlingRights {
  NO_CASTLING,
  WHITE_OO,
  WHITE_OOO = WHITE_OO << 1,
  BLACK_OO  = WHITE_OO << 2,
  BLACK_OOO = WHITE_OO << 3,

  KING_SIDE      = WHITE_OO  | BLACK_OO,
  QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
  WHITE_CASTLING = WHITE_OO  | WHITE_OOO,
  BLACK_CASTLING = BLACK_OO  | BLACK_OOO,
  ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

  CASTLING_RIGHT_NB = 16
}; 


//CONVENIENT BIT
#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_LSB(b) (__builtin_ctzll(b))
#define chars_to_bit(letter,number) ((letter - 'a') + ((number - '0') - 1)*8 )
#define countBitsOn(v) (std::bitset<64>(v).count())

inline int pop_LSB(uint64_t &b) {
    int i = get_LSB(b);
    b &= b - 1;
    return i;
}
const uint64_t columnA = 0x101010101010101ULL;
const uint64_t columnH = columnA << 7;
const uint64_t columnAB = columnA | (columnA<<1);
const uint64_t columnGH = columnH | (columnH>>1);
// END CONVENIENT BIT 