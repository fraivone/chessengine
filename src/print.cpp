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

void PrintPST(Color c, PieceType pt){
    // the goal is to pring each value of the table in a fixed 
    // width fashion. The value should be centered in the printing box
    int boxWidth = 4;
    int prepadding = 0;
    int postpadding = 0;
    for(int ri=nRows-1; ri>=0; ri--){
        std::cout<<"\n";
        std::cout<<KYEL  <<"    --- --- --- --- --- --- --- ---\n";
        std::cout<<ri+1<<"  ";
        for(int ci=0; ci<nCols; ci++){
            Square s = make_square(ci,ri);
            int number = getPieceSquareTableValue(c,s,pt);
            if (std::to_string(number).length()==1){
                prepadding = 1; postpadding = 1;
            }
            else if (std::to_string(number).length()==2){
                prepadding = 1; postpadding = 0;
            }
            else if (std::to_string(number).length()==3){
                prepadding = 0; postpadding = 0;
            }
            else{
            prepadding = 0; postpadding = 0;
            }
            std::cout<<"|";
            std::cout <<KNRM<< std::setw(prepadding + std::to_string(number).length()) << std::right << number << std::setw(postpadding) << ""<<KYEL ;
        }
    std::cout<<"|";
    }
    std::cout<<"\n    --- --- --- --- --- --- --- ---"<<std::endl;
    std::cout<<"     a   b   c   d   e   f   g   h "<<std::endl;
    std::cout<<KNRM<<"\n";

}

// prints the move as stockfish does when perft
std::string mvhuman(Move theMove){
    Square from = mv_from(theMove);
    Square to = mv_to(theMove);
    std::string out;
    out += char('a'+from%nCols);
    out += char('1'+from/nRows);
    out += char('a'+to%nCols);
    out += char('1'+to/nRows);

    MoveType mt = MoveType( theMove & 0xC000);
    PieceType pt = PieceType(((theMove >> 12) & (0x3))+2);
    if(mt==PROMOTION)
        out += PieceCharMap[BLACK][pt];
    return out;
}


void PrintMove(Move theMove, int value){
    auto string_first = mvhuman(theMove);
    MoveType mt = MoveType( theMove & 0xC000);
    int MoveTypeIndex = (mt >> 14)&0x3;

    std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<"\tEvaluated: "<<float(value)<<std::endl;
}


void PrintMoveList(MoveList s){
    Move theMove;
    int padding;
    std::string out;
    for(int i = 0; i<s.size; i++){
        out = "Move["+std::to_string(i)+"]";
        padding = 9 - out.length();
        std::cout << std::setw(padding + (out).length()) << std::left << out << "-  ";
        PrintMove(s.list[i].move,s.list[i].value);
    }
}


void PrintPinMap(Color Us){
    for (int pinned = 0; pinned < nCols*nRows; pinned++){
        Square attacker = Position::PinMap[Us][pinned];
        if(attacker != ENPSNT_UNAVAILABLE)
            std::cout<<"Square "<<+attacker<<" is pinning square "<<+pinned<<std::endl;
    }
}