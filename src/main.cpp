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
#include <type_traits>

int main(int argc, char **argv) {
	std::cout << "\n\n#######\nSize of MyClass: " << sizeof(chessboard::ChessBoard) << " bytes\n#######\n\n" << std::endl;
    Move theMove = {44,4, whiteRook};
    std::unique_ptr<chessboard::ChessBoard> thisInstance;
    if (argc == 1)
        thisInstance = std::make_unique<chessboard::ChessBoard>(0,0,0,578756592880516895,0,pow(2,49),0,0,0,0,0,pow(2,5));
    else if (argc == 2)
        thisInstance = std::make_unique<chessboard::ChessBoard>(argv[1]);
    if (thisInstance->legalStart()){
        thisInstance->printBoard();
        thisInstance->printStatusInfo();
        std::cout<<"Eval = " <<thisInstance->evalPosition()<<std::endl;
        // Moves t = thisInstance->PublicBlocking();
        // std::cout<<"Blocking moves found "<<t.size()<<std::endl;
        
        
    } // legal starting position

    
    else
        return -1;
};