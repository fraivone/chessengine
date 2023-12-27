#include "magic.hpp"
#include <cmath>


unsigned char reverseByte(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

Bitboard MirrorBitboardHoriz(Bitboard t){
        Bitboard kk = 0;
        // first row
        kk |= static_cast<int>(reverseByte( (t>>0) & 0xff )) << 0;
        // second row
        kk |= static_cast<Bitboard>(reverseByte( (t>>8) & 0xff )) << 8;
        // third row
        kk |= static_cast<Bitboard>(reverseByte( (t>>16) & 0xff )) << 16;
        // fourth row
        kk |= static_cast<Bitboard>(reverseByte( (t>>24) & 0xff )) << 24;
        // fifth row
        kk |= static_cast<Bitboard>(reverseByte( (t>>32) & 0xff )) << 32;
        // sixth row
        kk |= static_cast<Bitboard>(reverseByte( (t>>40) & 0xff )) << 40;
        // seventh row
        kk |= static_cast<Bitboard>(reverseByte( (t>>48) & 0xff )) << 48;
        // eighth row
        kk |= static_cast<Bitboard>(reverseByte( (t>>56) & 0xff )) << 56;
        return kk;
}

void generateLineMagicMask(){
    uint64_t thisRow;
    uint64_t thisColumn;
    uint64_t thisMask;
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;

        thisRow = (row1<<(row*8)) & ~vertical_edges;
        thisColumn = (columnA << col) & ~horizontal_edges;
        thisMask = thisRow | thisColumn;
        clear_bit(thisMask,i);
        std::cout<<thisMask<<",";
    }
}

void generateDiagoMagicMask(){
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;
        
        Bitboard thisMask = 0;
        auto row_idx = 0;
        // cartesian view
        // y is the row coordinate
        // x is the column coordinate
        // x = 0 --> y = start
        for(auto col_idx = 0; col_idx<8; col_idx++){
            row_idx = col_idx + (row-col);
            if ( row_idx >= 0)
                thisMask |= 1ULL<<(8*row_idx + col_idx);

            row_idx = -col_idx + (row+col);
            if(row_idx>=0)
                thisMask |= 1ULL<<(8*row_idx + col_idx);
        }
        clear_bit(thisMask,i);
        thisMask &= chessboard_inner;
        std::cout<<thisMask<<",";       
    }
}

std::vector<Bitboard> generateAllBlockers(Bitboard Mask){
    /// will contain all blockers configs for this Mask/square
    std::vector<Bitboard> blockers_occupancies;
    /// will contain the index of the set bits in Mask
    std::vector<int> index_setbits;
    
    Bitboard copy_mask = Mask;
    int setBits = countBitsOn(Mask);    
    uint64_t total_combinations = (1ULL<<(setBits)) -1 ;
    
    // filling index_setbits
    while(copy_mask)
        index_setbits.push_back(pop_LSB(copy_mask));

    // temp variables
    uint64_t copy_index;
    Bitboard thisBlockerBitboard;
    uint64_t thisBitIsSet;
    // skipping zero as it would mean no blockers
    for(uint64_t thisCombinationIndex = 1; thisCombinationIndex <= total_combinations; thisCombinationIndex++ ){
        thisBlockerBitboard = 0;
        copy_index = uint64_t(thisCombinationIndex);
        while(copy_index){
            thisBitIsSet = pop_LSB(copy_index); 
            thisBlockerBitboard |= 1ULL<<index_setbits[thisBitIsSet];
        }

        blockers_occupancies.push_back(thisBlockerBitboard);
    }

    return blockers_occupancies;
}