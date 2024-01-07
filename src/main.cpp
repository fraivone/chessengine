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
    init_position("7r/8/8/3k1pB1/7p/5P1P/r5PK/8 w - - 9 23");
    RepresentBoard();
    // MoveList start;
    // start = generate_all(start, Position::sideToMove);
    // PrintMoveList(start);
    RepresentBitset(Position::pinnedPieces);
    

    // RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    // MoveList OurMoveList;
    // // KING can't block
    // OurMoveList = generate_all(OurMoveList, Color(Us),PAWN,KNIGHT,BISHOP,ROOK,QUEEN); 
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 1;

}



