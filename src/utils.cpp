#include "utils.hpp"
#include <string>
#include <iostream>

std::string bit2notation(int bit){
    int row = bit/8 + 1;
    int col = bit%8;
    char currentLetter = 'A';
    //std::cout<<"bit = "<<bit<<"\trow,col = "<<row<<" , "<<col<<std::endl;
    std::string output = char(currentLetter+col) + std::to_string(row);
    return output;
}

// PieceType kind_of_piece = static_cast<PieceType>(idx);