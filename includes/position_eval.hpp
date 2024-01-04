/** @file */
#pragma once
#include "position.hpp"
#include "gen_move.hpp"
#include "types.hpp"

/// Given the move list for color THEM, check if Us is in check
/// returns a bitboard having the bits of checkers set
Bitboard Checkers(MoveList OpponentMoveList, Bitboard OurKingBitboard);
/// Checks if the Color "Us" is in check
/// returns a bitboard having the bits of checkers set
Bitboard Checkers(Color Us);

/// If our king is in check, return a bitboard with the
/// pieces that can actually block the attack
Bitboard PossibleBlockerBitboard(MoveList& opponentMoveList, Color Us);
/// If our king is in check, return a bitboard with the
/// pieces that can actually block the attack
Bitboard PossibleBlockerBitboard(Color Us);