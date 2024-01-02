# include "print.hpp"

void RepresentBitset(Bitboard number){
    std::string grid[nRows][nCols];
    // fill the grid
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;
        if (get_bit(number,i) !=0 ){    
            grid[row][col] = " #";
        }
        else  
            grid[row][col] = "  ";
    }
    // print the grid
    for(int ri=nRows-1; ri>=0; ri--){
        std::cout<<"\n";
        std::cout<<KYEL  <<"    --- --- --- --- --- --- --- ---\n";
        std::cout<<ri+1<<"  ";
        for(int ci=0; ci<nCols; ci++){
            std::cout<<"|"<<KNRM<< grid[ri][ci]<<" "<<KYEL;
            
        }
    std::cout<<"|";
    }
    std::cout<<"\n    --- --- --- --- --- --- --- ---"<<std::endl;
    std::cout<<"     a   b   c   d   e   f   g   h "<<std::endl;
    std::cout<<KNRM<<"\n";
}
void RepresentBoard(){
    // init the grid
    std::string grid[nRows][nCols];
    // fill the grid
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;
        switch(type_of(Position::board[i])) {
            case PAWN:
                grid[row][col] = " P";
                break;
            case KNIGHT:
                grid[row][col] = " N";
                break;
            case BISHOP:
                grid[row][col] = " B";
                break;
            case ROOK:
                grid[row][col] = " R";
                break;
            case QUEEN:
                grid[row][col] = " Q";
                break;
            case KING:
                grid[row][col] = " K";
                break;
            default: 
                grid[row][col] = "  ";
        }
        if( (Position::board[i] != NO_PIECE) && (color_of(Position::board[i]) == BLACK) )
            boost::algorithm::to_lower(grid[row][col]);
    }
    // print the grid
    for(int ri=nRows-1; ri>=0; ri--){
        std::cout<<"\n";
        std::cout<<KYEL  <<"    --- --- --- --- --- --- --- ---\n";
        std::cout<<ri+1<<"  ";
        for(int ci=0; ci<nCols; ci++){
            std::cout<<"|"<<KNRM<< grid[ri][ci]<<" "<<KYEL;
            
        }
    std::cout<<"|";
    }
    std::cout<<"\n    --- --- --- --- --- --- --- ---"<<std::endl;
    std::cout<<"     a   b   c   d   e   f   g   h "<<std::endl;
    std::cout<<KNRM<<"\n";
}
