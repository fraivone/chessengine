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