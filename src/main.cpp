// main.cpp
#include <iostream>
#include <unordered_map>
#include <string>
#include <bitset>
#include "chessboard.hpp"
#include "lut.hpp"
#include "def.hpp"
#include <typeinfo>
#include <chrono>
#include <memory>
// #include "movesearcher.cpp"
#include <type_traits>

int main(int argc, char **argv) {
	std::cout << "\n\n#######\nSize of MyClass: " << sizeof(chessboard::ChessBoard) << " bytes\n#######\n\n" << std::endl;
    chessboard::ChessBoard* thisInstance;
    if (argc == 1)
        thisInstance = new chessboard::ChessBoard(0,0,0,578756592880516895,0,pow(2,49),0,0,0,0,0,pow(2,5));
    else if (argc == 2)
        thisInstance = new chessboard::ChessBoard(argv[1]);
    if (thisInstance->legalStart()){
        // thisInstance->printBoard();
        thisInstance->printStatusInfo();
        std::chrono::steady_clock::time_point begin,end;
        for(int iter=0;iter<5; iter++ ){
            begin = std::chrono::steady_clock::now();
            ScorenMove best = searchBestMove2(thisInstance->get_board(), thisInstance->board_turn, MINUS_INF, PLUS_INF, iter);
            std::cout<<best.first<<std::endl;
            printMove(best.second);
            end = std::chrono::steady_clock::now();
            std::cout<<"v2 took "<<std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()<<std::endl;
            std::cout<<"\n\n\n";
        }
              
    } // legal starting position

    
    else
        return -1;
};