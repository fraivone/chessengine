# include "print.hpp"

void RepresentBitset(Bitboard number){
    // init the grid
    int nCols = 8;
    int nRows = 8;
    std::string grid[nRows][nCols];
    for(int ri=nRows-1; ri>=0; ri--){
        for(int ci=0; ci<nCols; ci++){
            grid[ri][ci] = "  ";
        }
    }
    // fill the grid
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;
        if (get_bit(number,i) !=0 ){    
            grid[row][col] = " #";
        }
    }
    // print the grid
    for(int ri=nRows-1; ri>=0; ri--){
        std::cout<<"\n";
        std::cout<<KYEL  <<"    --- --- --- --- --- --- --- ---\n";
        std::cout<<ri<<"  ";
        for(int ci=0; ci<nCols; ci++){
            std::cout<<"|"<<KNRM<< grid[ri][ci]<<" "<<KYEL;
            
        }
    std::cout<<"|";
    }
    std::cout<<"\n    --- --- --- --- --- --- --- ---"<<std::endl;
    std::cout<<"     a   b   c   d   e   f   g   h "<<std::endl;
    std::cout<<"\n";
}
