/** @file */
#pragma once
#include <eval.hpp>
#include <position.hpp>
#include <move_maker.hpp>
#include <pst.hpp>
#include <algorithm> // std::sort 


/// Given a extmove it adds the move score and returns it
/// It realies on the scores already present in Position::st
/// and updates them
ExtMove EvalExtMove(ExtMove& extmove);
/// Eval the board for what it is now
/// only taking into account the material
int EvalPosition();

/// Evaluate each move in the move list
/// returns the same move list where all
/// moves have now been evaluated
MoveList EvalMoveList(MoveList& mvList);

/// test evaluation function
ExtMove minmax(Color Us, int alpha, int beta, int depth, int maxdepth, uint64_t & counter, bool verbose = false);

/// helps sorting best move for black
bool black_sorter(ExtMove const& lhs, ExtMove const& rhs);
/// helps sorting best move for white
bool white_sorter(ExtMove const& lhs, ExtMove const& rhs);