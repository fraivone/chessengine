/** @file */
#pragma once
#include <types.hpp>
#include <position.hpp>

/// Given a move(internally gnerated) makes the move and updates
/// the position and the stateinfo
void MakeMove(Move mv);
/// Given a move previously made, updates
/// the position and the stateinfo undoing the move
void UndoMove(Move mv);

// count legal moves
int StupidPerftCount(MoveList mvList, uint8_t depth,uint8_t MaxDepth, bool verbose = false, int count = 0);
// count legal moves
int StupidPerftCount(std::string FEN, uint8_t MAxDepth, bool verbose = false);