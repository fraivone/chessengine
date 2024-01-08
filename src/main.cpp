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
    
    std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 0)<<std::endl;
    std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 1)<<std::endl;
    std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2)<<std::endl;
    std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 3)<<std::endl;
    std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 4)<<std::endl;  
    // std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5)<<std::endl;
    
    // std::cout<<StupidPerftCount("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R2BK2R b KQkq - 1 1", 1,true)<<std::endl;
    // init_position("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R2BK2R b KQkq - 1 1");
    // auto t = generate_legal(Position::sideToMove);
    // PrintMove(t.list[3].move);
    // MakeMove(t.list[3].move);
    // t = generate_legal(Position::sideToMove);
    // PrintMoveList(t);
    // std::cout<<t.size<<std::endl;


    // return 0;

}



