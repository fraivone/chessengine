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
    init_position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    RepresentBoard();
    MoveList start;
    start = generate_all(start, Position::sideToMove);
    PrintMoveList(start);
    

    // RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    // MoveList OurMoveList;
    // // KING can't block
    // OurMoveList = generate_all(OurMoveList, Color(Us),PAWN,KNIGHT,BISHOP,ROOK,QUEEN); 
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 1;

}



