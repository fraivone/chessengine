// main.cpp
#include <iostream>
#include <unordered_map>
#include <string>
#include <bitset>
#include "chessboard.hpp"
#include "lut.hpp"
#include "def.hpp"
#include <typeinfo>
#include <memory>
// #include "movesearcher.cpp"
#include <type_traits>

int main(int argc, char **argv) {
	// std::cout << "\n\n#######\nSize of MyClass: " << sizeof(chessboard::ChessBoard) << " bytes\n#######\n\n" << std::endl;
    chessboard::ChessBoard* thisInstance;
    if (argc == 1)
        thisInstance = new chessboard::ChessBoard(0,0,0,578756592880516895,0,pow(2,49),0,0,0,0,0,pow(2,5));
    else if (argc == 2)
        thisInstance = new chessboard::ChessBoard(argv[1]);
    if (thisInstance->legalStart()){
        // thisInstance->printBoard();
        // thisInstance->printStatusInfo();
        std::cout<<searchBestMove(thisInstance, 1)<<std::endl;
        // Moves legals = thisInstance->tempFunc();
        // Color turn = thisInstance->get_board_turn();
        // thisInstance->searchBestMove(turn, legals, thisInstance->get_board(), 1);
        // Moves t = thisInstance->PublicBlocking();
        // std::cout<<"Blocking moves found "<<t.size()<<std::endl;
        
        
    } // legal starting position

    
    else
        return -1;
};