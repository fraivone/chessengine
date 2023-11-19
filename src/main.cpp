// main.cpp
#include <iostream>
#include <unordered_map>
#include <string>
#include <bitset>
#include "chessboard.hpp"
#include "lut.hpp"
#include "utils.hpp"
#include "pieces.hpp"
#include <typeinfo>
#include <type_traits>

int main(int argc, char **argv) {
	// 
    chessboard::ChessBoard* p;
    if (argc == 1)
        p = new chessboard::ChessBoard(0,0,0,578756592880516895,0,pow(2,49),0,0,0,0,0,pow(2,5));
    else if (argc == 2)
	    p = new chessboard::ChessBoard(argv[1]);
    if (p->legalStart()){
        std::cout<<"Eval = " <<p->evalPosition()<<std::endl;
        p->printBoard();
        std::cout<<"is check? "<<p->isCheck()<<std::endl;
        std::cout<<"is mate? "<<p->isMate()<<std::endl;
        //Moves vector_with_moves = p->calculate_moves(p->get_board_turn());
        //std::cout<<"Found "<<vector_with_moves.size()<<" moves"<<std::endl;

        // if (p->debug)
        //     for (auto move: vector_with_moves) {
        //         std::cout << "The " << convert_color[move.piece.color] << " "
        //               << convert_enum[move.piece.piece_type]
        //               << " moves from " << bit2notation(move.initial_bit)
        //               << " to " << bit2notation(move.final_bit)<<std::endl;
        // }

    delete p;
    } // legal starting position

    
    else
        return -1;
};