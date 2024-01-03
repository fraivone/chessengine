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


void PrintMoveList(MoveList s){
    Move theMove;
    for(int i = 0; i<s.size; i++){
        theMove = s.list[i].move;
        Square from = ((theMove>>6) & 0x3f);
        Square to = ((theMove) & 0x3f);
        PieceType pt = PieceType(((theMove >> 12) & (0x3))+2);
        MoveType mt = MoveType( theMove & 0xC000);
        int MoveTypeIndex = (mt >> 14)&0x3;
        std::cout<<"Move["<<i<<"] - From "<<+from<<" to "<<+to << "  -  ";
        
        
        if(mt==PROMOTION)
            std::cout<<MoveTypeNames[MoveTypeIndex]<<" to "<<PieceTypeNames[pt]<<std::endl;
        else
            std::cout<<MoveTypeNames[MoveTypeIndex]<<std::endl;
    }
}