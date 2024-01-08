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
#include "move_maker.hpp"

int main(){
    init_lut();
    init_magics();
    init_position("r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2");
    RepresentBoard();
    MoveList legalMoves;
    legalMoves = generate_legal(Position::sideToMove);
    Bitboard before = pieces();
    Bitboard after;
    for(int i= 0; i<legalMoves.size; i++ ){
        PrintMove(legalMoves.list[i].move);
        MakeMove(legalMoves.list[i].move);    
        UndoMove(legalMoves.list[i].move);
        after = pieces();
        if(after != before){
            std::cout<<"Dude they are different "<<std::hex<<(before - (before & after))<<std::endl;
            RepresentBoard();
        }
            
    }
    RepresentBoard();
    
    
    // RepresentBitset(BlockerPossibleBitboard(Position::sideToMove));
    
    
    // return Attackers2Bitboard(OurMoveList,possible_blockers_bb);
    
    

    return 0;

}



