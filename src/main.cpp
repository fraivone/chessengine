// main.cpp
#include <iostream>
#include <unordered_map>
#include <string>
#include "chessboard.hpp"
#include "lut.hpp"
#include <bitset>
#include "utils.hpp"

int main(int argc, char **argv) {
	chessboard::ChessBoard* p = new chessboard::ChessBoard(4,pow(2,8),0,0,0,pow(2,49),0,0,0,0,0,pow(2,44));
    if (p->legalStart()){
        std::cout<<"Eval = " <<p->evalPosition()<<std::endl;
        p->printBoard();
        Moves vector_with_moves = p->calculate_moves();

        for (auto move: vector_with_moves) {
            std::cout << "The " << convert_color[move.piece.color] << " "
                      << convert_enum[move.piece.piece_type]
                      << " moves from " << bit2notation(move.initial_bit)
                      << " to " << bit2notation(move.final_bit)<<std::endl;

        }

    } // legal starting position

    
    else
        return -1;
};