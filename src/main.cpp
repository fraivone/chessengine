// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include "print.hpp"
#include "types.hpp"
#include "magic.hpp"
#include "position.hpp"
#include "gen_move.hpp"
#include "position_eval.hpp"

int main(){
    init_magics();
    
    init_position("3Rk3/5prp/p5b1/6B1/r6P/2P5/PP3P2/2K3R1 b - - 2 23");
    RepresentBoard();
    MoveList start;    
    // start = PawnAnyMoves(start, BLACK, 50);
    
    RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    // MoveList OurMoveList;
    // // KING can't block
    // OurMoveList = generate_all(OurMoveList, Color(Us),PAWN,KNIGHT,BISHOP,ROOK,QUEEN); 
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 1;

}



