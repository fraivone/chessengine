// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdlib> // convert argv to int with atoi
#include "print.hpp"
#include "types.hpp"
#include "lut.hpp"
#include "magic.hpp"
#include "position.hpp"
#include "gen_move.hpp"
#include "move_maker.hpp"

int main(int argc, char **argv){
    init_lut();
    init_magics();
    // fen + depth provided, single run
    if (argc == 3)
        std::cout<<StupidPerftCount(argv[1], atoi(argv[2]),true)<<std::endl;  
    // no argument parsed
    else if (argc == 1){
        std::string line,depthString;
        int depth;
        while(1){
            std::cout<<"Enter fen"<<std::endl;
            std::getline(std::cin, line);
        
            std::cout<<"Enter depth"<<std::endl;
            std::getline(std::cin, depthString);
            depth = atoi(depthString.c_str());
            std::cout<<"Got "<<line<<"\tdepth "<<depth<<std::endl;
            
            
            std::cout<<StupidPerftCount(line, depth,true)<<std::endl;  
            std::cout<<"DONE"<<std::endl;
            line.clear();
            depthString.clear();
        }
    }
    else{
        init_position("r2k4/p1ppqNb1/bn2pQp1/3P4/1p2P2r/2NB3p/PPPB1PPP/R3K2R b KQ - 0 3");
        MoveList t = generate_legal(Position::sideToMove);
        PrintMoveList(t);
        RepresentBoard();
    }
       
    
    // init_position("r4k1r/p1ppq1b1/bn1Np1pB/3n4/1p2P3/2N2Q1p/PPP1BPPP/R3K2R b KQ - 1 3");
    // auto t = generate_legal(Position::sideToMove);
    // PrintMoveList(t);
    // MakeMove(t.list[38].move);
    // t = generate_legal(Position::sideToMove);
    // // PrintMoveList(t);
    // std::cout<<t.size<<std::endl;


    return 0;

}



