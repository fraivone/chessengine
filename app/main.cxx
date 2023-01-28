// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdlib> // convert argv to int with atoi
#include <algorithm> // sort algorithm
#include "print.hpp"
#include "types.hpp"
#include "lut.hpp"
#include "magic.hpp"
#include "position.hpp"
#include "gen_move.hpp"
#include "move_maker.hpp"
#include "eval.hpp"
#include "hash.hpp"
unsigned VERBOSE = 1;
int main(int argc, char **argv){
    HashTables::init();
    init_lut();
    init_magics();
    
    
    // no argument parsed, interface to pyhton execution
    if (argc == 1){
        
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
        init_position("r1b5/p3R3/2pk4/1p6/4P1pP/1BP1P3/P1P3Pq/6K1 b - h3 0 25");
        MoveList legal = generate_legal(Position::sideToMove);


        
        

        // std::cout<<"eval() "<< t[0].eval()<<std::endl;
        // std::cout<<"Value(EvalPosition()) "<< Value(EvalPosition())<<std::endl;
        // std::cout<<"Size of SearchSummary "<< sizeof(ss)<<std::endl;

        // std::cout<<"Size of uint64_t"<< sizeof(s)<<std::endl;
        // std::cout<<"Size of bool"<< sizeof(b)<<std::endl;
        
        
    }
        
    return 0;

}

