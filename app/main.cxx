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

int main(int argc, char **argv){
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
    else if(argc ==2){
        std::string line;
        MoveList legal;
        bool moveIsLegal;
        int i;
        ExtMove theBest;
        std::cout<<"Let's play"<<std::endl;
        init_position("r3kb1r/pbp1p1pp/2p2p2/6B1/2P5/5B2/PP3PPP/3RK2R w Kkq - 0 12");
        while(1){
            moveIsLegal = false;
            uint64_t move_counter = 0;
            legal.Clear();
            legal = generate_legal(Position::sideToMove);
            RepresentBoard();

            while(!moveIsLegal){        
                std::cout<<"Enter move"<<std::endl;
                std::getline(std::cin, line);
                const char* charArray = line.c_str();
                int colFrom = charArray[0] -'a';
                int rowFrom = charArray[1] -'1';
                int colTo = charArray[2] -'a';
                int rowTo = charArray[3] -'1';
                std::cout<<"You entered the move "<<charArray[0]<<charArray[1]<<charArray[2]<<charArray[3]<<".";

                Square from = make_square(colFrom,rowFrom);
                Square to = make_square(colTo,rowTo);
                Move m = make_move(from,to);
                for(i=0; i < legal.size; i++){
                    if( (legal.list[i].move & 0xFFF) == m){
                        moveIsLegal = true;
                        break;
                    }
                }
                if(!moveIsLegal)
                    std::cout<<" Your move is illegal."<<std::endl;
                else
                    std::cout<<std::endl;
            }
            // make the move
            std::cout<<"You play ";
            PrintMove(legal.list[i].move,legal.list[i].value);
            MakeMove(legal.list[i].move);

            move_counter = 0;
            legal.Clear();
            legal = generate_legal(Position::sideToMove);
            theBest = minmax(Position::sideToMove,-10000000, +10000000, 7,7,move_counter, false);
            std::cout<<"I searched "<<move_counter<<" moves. I play the best move ";
            PrintMove(theBest.move,theBest.value);
            MakeMove(theBest);

        }

            // make_move(Square from, Square to)
            // std::cout<<"Position is evaluated "<<float(EvalPosition())/PawnValue<<std::endl;
            // auto theBest = minmax(Position::sideToMove,-10000000, +10000000, 7,7);
            // PrintMove(theBest.move, theBest.value);
            // ExtMove test;
            // test = Move(1);
            // std::cout<<test.value<<std::endl;
            // RepresentBoard();
    }
    else{

        init_position("2r3k1/2r2pp1/4p2p/1B1p4/3P2P1/Pn3N2/5PP1/1R3RK1 b - - 2 26");
        MoveList legal = generate_legal(Position::sideToMove);
        legal = EvalMoveList(legal);
        if(Position::sideToMove == WHITE)
            std::sort(&legal.list[0], &legal.list[legal.size], &white_sorter);
        else
            std::sort(&legal.list[0], &legal.list[legal.size], &black_sorter);
        PrintMoveList(legal);
    }
        
    return 0;

}



