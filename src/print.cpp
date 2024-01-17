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

void PrintMove(Move theMove){
    auto string_first = mvhuman(theMove);
    MoveType mt = MoveType( theMove & 0xC000);
    int MoveTypeIndex = (mt >> 14)&0x3;

    std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<std::endl;
}

void PrintExtMove(ExtMove extmv){
    Color c = extmv.value > 0? WHITE: BLACK;
    if((abs(extmv.value) == VALUE_MATE_0))
        std::cout<<"Game over "<<ColorNames[c]<<" won"<<std::endl;
    else{
        auto string_first = mvhuman(extmv.move);
        MoveType mt = MoveType( extmv.move & 0xC000);
        int MoveTypeIndex = (mt >> 14)&0x3;


        if((abs(extmv.value) >= VALUE_MATE_30) & (abs(extmv.value) < VALUE_MATE_0)){
            int mateInPlys = (VALUE_MATE_0 - extmv.value) >> 16;
            std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<"\tEvaluation: "<<ColorNames[c]<<" MATE in "<<mateInPlys<<std::endl;
        }

        else if((extmv.value == VALUE_NONE))
            std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<"\tEvaluation: NONE"<<std::endl;
        else if((abs(extmv.value) == VALUE_INFINITE))
            std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<"\tEvaluation: INFINITE"<<std::endl;
        else
            std::cout<<string_first<< " "<<MoveTypeNames[MoveTypeIndex]<<"\tEvaluation: "<<extmv.value<<std::endl;
    }

}


void PrintMoveList(MoveList s){
    Move theMove;
    int padding;
    std::string out;
    for(int i = 0; i<s.size; i++){
        out = "Move["+std::to_string(i)+"]";
        padding = 9 - out.length();
        std::cout << std::setw(padding + (out).length()) << std::left << out << "-  ";
        PrintMove(s.list[i].move);
    }
}


void PrintPinMap(Color Us){
    for (int pinned = 0; pinned < nCols*nRows; pinned++){
        Square attacker = Position::PinMap[Us][pinned];
        if(attacker != ENPSNT_UNAVAILABLE)
            std::cout<<"Square "<<+attacker<<" is pinning square "<<+pinned<<std::endl;
    }
}


void PrintTranspositionTable(){
    for(int i = 0; i<TABLE_SIZE; i++){
        // don't print empty entries
        if(HashTables::table[i].move()==MOVE_NONE)
            continue;
        std::cout<<"Zobrist 48MSB "<<std::hex<<HashTables::table[i].ms48b_zobrist()<<std::dec<<std::endl;
        std::cout<<"Best move "<<mvhuman(HashTables::table[i].move())<<std::endl;
        std::cout<<"Depth "<<+HashTables::table[i].depth()<<std::endl;
        std::cout<<"Eval "<<HashTables::table[i].eval()<<std::endl;
        std::cout<<"ScoreType "<<HashTables::table[i].scoretype()<<std::endl;
        std::cout<<"-----------------------"<<std::dec<<std::endl;
    }
}