#include "magic.hpp"

namespace MagicVariables {
    uint64_t Rattacks[nCols*nRows][4096] = {ALL_SQUARES};
    uint64_t Battacks[nCols*nRows][4096] = {ALL_SQUARES};
    Magic BishopMagics[nCols*nRows];
    Magic RookMagics[nCols*nRows];
    void init(){
        for(int i = 0; i<nRows*nCols; i++){
            BishopMagics[i].mask = Diago[i];
            BishopMagics[i].magicNumber = 0;
            BishopMagics[i].shift = 0;
            
            RookMagics[i].mask = Line[i];
            RookMagics[i].magicNumber = 0;
            RookMagics[i].shift = 0;
        }
    }
}

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
        if(VERBOSE>2) std::cout<<thisMask<<",";
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
        if(VERBOSE>2) std::cout<<thisMask<<",";       
    }
}

std::vector<Bitboard> generateAllBlockers(Bitboard Mask){
    // will contain all blockers configs for this Mask/square
    std::vector<Bitboard> blockers_occupancies;
    // will contain the index of the set bits in Mask
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
    // include zero, it is the no blockers config
    for(uint64_t thisCombinationIndex = 0; thisCombinationIndex <= total_combinations; thisCombinationIndex++ ){
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


void generate_magics(PieceType pt, Bitboard (&table)[nCols*nRows][4096], Magic (&MagicArray)[nRows*nCols]){
    LUT LandingMask;
    if (pt == BISHOP)
        LandingMask = Diago;
    else if (pt == ROOK)
        LandingMask = Line;
    else
        throw std::invalid_argument("In function generate_magics PieceType must be either BISHOP or ROOK.");

    // Contains the attacks for each index combination
    uint64_t Rreference[4096];
    uint64_t magic;
    // size of the mask, current magic attempt, number of unique indeces filled in this attempt, highest counter so far, collisions log
    int bits,attempt,counter,max,collisions[4096] ; 

    
    for(Square square = 0; square<nRows*nCols; square++){
        // use the right seed (based the current row number) to converge faster
        PRNG rng(seeds[square/nRows]);
        auto allBlockers = generateAllBlockers(LandingMask[square]);
        bits = countBitsOn(LandingMask[square]); // number of set bits
        attempt = 0;
        max = 0;

        do{
            attempt ++;
            // Initialize Rreference to "attack all squares"
            std::fill_n(Rreference, 4096, ALL_SQUARES);
            // Initialize Collisions to 0
            std::fill_n(collisions, 4096, 0);
            // get new magic
            magic = rng.sparse_rand<Bitboard>(); 
            if(VERBOSE>2) std::cout<<"Magic generated = "<<magic<<"\n";

            counter = 0;
            // loop over all blockers
            for(auto blocker:allBlockers){

                // here I am using a legacy function to calculate landings
                // it works alright as long as I pretend that all blockers are opponents
                auto theseBlockersAttack = sliding_peices_landings(pt,square,0,blocker);
                // generate index for this blocker
                auto thisIndex = ( (blocker * magic ) >> (64-bits));

                // index is not yet used --> fill
                if ( Rreference[thisIndex] == ALL_SQUARES){
                    Rreference[thisIndex] = theseBlockersAttack;
                    counter++;
                }

                // index is already used, we have a collision
                else{
                    // unharmful collision
                    // index is already used but the value is the same --> don't change the attacks array but count this blocker as filled
                    if(theseBlockersAttack == Rreference[thisIndex]){
                        counter++;
                        if(VERBOSE>3) std::cout<<"Unharmful collision\n";
                        collisions[thisIndex]++;
                    }
                    // bad collision, need a new magic number
                    // index is already used with a different value --> break, magic number didn't work
                    else{
                        if(VERBOSE>2) std::cout<<"Current Magic isn't good.\tSquare = "<<square<<" Attempt = "<<attempt<<"    Best Magic Number Scored = "<<max<<" /   "<<allBlockers.size()<<"\n";
                    break;
                    }
                }
                if(counter > max) max = counter;
            }// end loop over all blockers        
        }while(counter < allBlockers.size());

        MagicArray[square].magicNumber = magic;
        MagicArray[square].shift = 64-bits;
        memcpy(table[square], Rreference, sizeof(Rreference));

        if(VERBOSE>1) std::cout<<"For the square "<<square<<" success after "<<attempt<<" attempts.\nFor "<<PieceNames[pt]<<" found\n";
        if(VERBOSE>1) std::cout<<"\tMagic =  "<<MagicArray[square].magicNumber<<"\t Shift = "<<MagicArray[square].shift<<std::endl;;
        auto MaxNumberOfCollisions = *std::max_element(collisions , collisions + 4096);
        if (MaxNumberOfCollisions == 0)
            if(VERBOSE>1) std::cout<<"\tNo collisions at all\n";
        else{
            int x = std::distance(collisions, std::find(collisions, collisions + 4096, MaxNumberOfCollisions));
            if(VERBOSE>1) std::cout << "\tThe index "<<x<<" is the one with the most collisions ("<<MaxNumberOfCollisions<<")"<< std::endl;
        }
    }
}


void init_magics(){
    if(VERBOSE>1) std::cout<<"Defaulting magic variables"<<std::endl;
    MagicVariables::init();
    if(VERBOSE>1) std::cout<<"Initializing rook magic bitboards"<<std::endl;
    generate_magics(ROOK, MagicVariables::Rattacks, MagicVariables::RookMagics);
    if(VERBOSE>1) std::cout<<"Initializing bishop magic bitboards"<<std::endl;
    generate_magics(BISHOP, MagicVariables::Battacks, MagicVariables::BishopMagics);
}
//void import_magics(Magic RookMagics[nCols*nRows], uint64_t Rattacks[nCols*nRows][4096],Magic BishopMagics[nCols*nRows], uint64_t Battacks[nCols*nRows][4096]);
//     if(VERBOSE>0) std::cout<<"Defaulting magic variables"<<std::endl;
//     MagicVariables::init();
//     if(VERBOSE>0) std::cout<<"Initializing rook magic bitboards"<<std::endl;
//     generate_magics(ROOK, MagicVariables::Rattacks, MagicVariables::RookMagics);
//     if(VERBOSE>0) std::cout<<"Initializing bishop magic bitboards"<<std::endl;
//     generate_magics(BISHOP, MagicVariables::Battacks, MagicVariables::BishopMagics);
// }

Bitboard get_rook_landings(Square square, Bitboard all_pieces_occupancy){
    Bitboard blockers = all_pieces_occupancy & MagicVariables::RookMagics[square].mask;
    uint64_t magicIndex = ( (blockers * MagicVariables::RookMagics[square].magicNumber ) >> MagicVariables::RookMagics[square].shift );
    Bitboard attacks = MagicVariables::Rattacks[square][magicIndex];
    return attacks;   
}
Bitboard get_bishop_landings(Square square, Bitboard all_pieces_occupancy){
    Bitboard blockers = all_pieces_occupancy & MagicVariables::BishopMagics[square].mask;
    uint64_t magicIndex = ( (blockers * MagicVariables::BishopMagics[square].magicNumber ) >> MagicVariables::BishopMagics[square].shift );
    Bitboard attacks = MagicVariables::Battacks[square][magicIndex];    
    return attacks;   
}

Bitboard get_sliding_landings(PieceType pt, Square square, Bitboard all_pieces_occupancy){
    uint64_t magicIndex;
    Bitboard blockers,attacks = 0;
    if(pt == ROOK)
        attacks |= get_rook_landings(square, all_pieces_occupancy);
    else if(pt == BISHOP)
        attacks |= get_bishop_landings(square, all_pieces_occupancy);
    else if(pt == QUEEN)
        attacks |= get_bishop_landings(square, all_pieces_occupancy) | get_rook_landings(square, all_pieces_occupancy);
    else
        throw std::invalid_argument("In function get_sliding_landings PieceType must be either BISHOP, ROOK or QUEEN.");
        
    return attacks;

}