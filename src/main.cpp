// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include "print.hpp"
#include "types.hpp"
#include "lut.hpp"
#include "magic.hpp"
#include "position.hpp"
#include "gen_move.hpp"

int main(){
    init_lut();
    init_magics();
    init_position("r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2");
    RepresentBoard();
    MoveList legalMoves;
    legalMoves = generate_legal(Position::sideToMove);
    PrintMoveList(legalMoves);
    // RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    
    
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 0;

}



