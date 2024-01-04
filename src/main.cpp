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
#include "position_eval.hpp"

int main(){
    init_lut();
    init_magics();
    init_position("r1bqk2r/p1p2ppp/1p1p1n2/4p3/3P4/NnP1PN2/PP1BBPPb/R3K2R w KQkq - 1 8");
    RepresentBoard();
    MoveList start;    
    start = generate_all(start, Position::sideToMove,KING);
    PrintMoveList(start);
    

    // RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    // MoveList OurMoveList;
    // // KING can't block
    // OurMoveList = generate_all(OurMoveList, Color(Us),PAWN,KNIGHT,BISHOP,ROOK,QUEEN); 
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 1;

}



