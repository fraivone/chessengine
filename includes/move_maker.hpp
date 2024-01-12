/** @file */
#pragma once
#include <types.hpp>
#include <position.hpp>
#include <pst.hpp>

/// Given a move(internally gnerated) makes the move and updates
/// the position and the stateinfo
void MakeMove(Move mv);
/// Given a move previously made, updates
/// the position and the stateinfo undoing the move
void UndoMove(Move mv);

/// count legal moves only at depth, non cumulative. Mimicks stockfish
uint64_t StupidPerftCount(MoveList mvList, uint8_t depth,uint8_t MaxDepth, bool verbose = false, uint64_t count = 0);
/// count legal moves only at depth, non cumulative. Mimicks stockfish
uint64_t StupidPerftCount(std::string FEN, uint8_t MAxDepth, bool verbose = false);
/// Given a string in stockfish style (4 or 5 char with <sq_from><sq_to><promotion_type optional>)
/// generates a move. 
/// If the move is legal, it returns it.
/// else return NULL_MOVE
Move parse_move(std::string inputstring);