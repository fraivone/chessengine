/*
8   56 # # # # # # # # 63
7   48 # # # # # # # # 55
6   40 # # # # # # # # 47
5   32 # # # # # # # # 39
4   24 # # # # # # # # 31
3   16 # # # # # # # # 23
2   8  # # # # # # # # 15
1   0  # # # # # # # # 7 
    A       ...        H
*/
#include "lut.hpp"


Bitboard BetweenBB[nCols*nRows][nCols*nRows];

void init_lut(){
    std::cout<<"Initializing inBetween bitboards"<<std::endl;
    generate_inBetweenLUT();
}

void generate_inBetweenLUT(){
    for(Square sq1=0; sq1<nCols*nRows; sq1++)
        for(Square sq2=0; sq2<nCols*nRows; sq2++)
            BetweenBB[sq1][sq2] = calculate_inBetween(sq1,sq2);
}

Bitboard calculate_inBetween(Square sq1, Square sq2){
    int ARow = sq1/nRows;
    int ACol = sq1%nCols;
    int BRow = sq2/nRows;
    int BCol = sq2%nCols;
    int deltaRows = BRow - ARow;
    int deltaCols = BCol - ACol;

    int dirXA = 0;
    int dirYA = 0;
    Bitboard inBetweenSquares = 0ULL;
    // sq1 and sq2 touch each other or are the same square, no in between
    if(   ( (abs(deltaRows) == 1)  & (abs(deltaCols) == 0) )
        ||( (abs(deltaRows) == 0)  & (abs(deltaCols) == 1) )
        ||( (abs(deltaRows) == 0)  & (abs(deltaCols) == 0) )
        ||( (abs(deltaRows) == 1)  & (abs(deltaCols) == 1) ))
        return inBetweenSquares;
    
    else{
        // same file
        if(deltaCols == 0 ){
            dirYA = ARow < BRow ? 1 : -1;
            while((abs(ARow - BRow) !=1)){
                ARow += dirYA;
                inBetweenSquares |= make_bitboard(make_square(ACol,ARow));
            } 
        }
        // same rank
        else if(deltaRows == 0 ){
            dirXA = ACol < BCol ? 1 : -1;
            while((abs(ACol - BCol) !=1)){
                ACol += dirXA;
                inBetweenSquares |= make_bitboard(make_square(ACol,ARow));
            } 
        }
        // diago
        else if (abs(deltaCols) == abs(deltaRows)){
            dirXA = ACol < BCol ? 1 : -1;
            dirYA = ARow < BRow ? 1 : -1;
            while( (abs(ACol - BCol) !=1) & (abs(ARow - BRow) !=1) ){
                ACol += dirXA;
                ARow += dirYA;
                inBetweenSquares |= make_bitboard(make_square(ACol,ARow));
            }
        }
        else{
            // std::cout<<"Squares "<<+sq1<<","<<+sq2<<" are incompatible\n";
            ;
        }
        
    }
    return inBetweenSquares;

}






std::array<uint64_t,64> wpawn_straight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_wpawnstraight_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   (initial_pos << 8);
        map_wpawnstraight_landing[init_bit] = landing;
    }
    return map_wpawnstraight_landing;
}
std::array<uint64_t,64> wpawn_doublestraight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_wpawndoublestraight_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        landing = 0ULL;
        set_bit(initial_pos,init_bit);
        if (init_bit >=8 & init_bit<= 15 ) // paw is on its starting position
            landing =   (initial_pos << 16);
        map_wpawndoublestraight_landing[init_bit] = landing;
    }
    return map_wpawndoublestraight_landing;
}

std::array<uint64_t,64> bpawn_straight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_bpawnstraight_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   (initial_pos >> 8);
        map_bpawnstraight_landing[init_bit] = landing;
    }
    return map_bpawnstraight_landing;
}
std::array<uint64_t,64> bpawn_doublestraight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_bpawndoublestraight_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        landing = 0ULL;
        set_bit(initial_pos,init_bit);
        if (init_bit >=48 & init_bit<= 56 ) // paw is on its starting position
            landing =   (initial_pos >> 16);
        map_bpawndoublestraight_landing[init_bit] = landing;
    }
    return map_bpawndoublestraight_landing;
}

std::array<uint64_t,64> wpawn_diagcapture_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_wpawndiagcapture_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   ((initial_pos << 7)  & ~columnH) |
                    ((initial_pos << 9)  & ~columnA) ;
        map_wpawndiagcapture_landing[init_bit] = landing;
    }
    return map_wpawndiagcapture_landing;
}

std::array<uint64_t,64> bpawn_diagcapture_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_bpawndiagcapture_landing;
    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   ((initial_pos >> 9) & ~columnH) |
                    ((initial_pos >> 7) & ~columnA) ;
        map_bpawndiagcapture_landing[init_bit] = landing;
    }
    return map_bpawndiagcapture_landing;
}

std::array<uint64_t,64> king_position_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_king_landing;

    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   (((initial_pos << 1) | (initial_pos << 9)  | (initial_pos >> 7)) & ~columnA) |  // move  to the right. If ends on column A it means it crossed the board. Illegal
                    (((initial_pos >> 1) | (initial_pos >> 9 ) | (initial_pos << 7 )) & ~columnH)|  // move to the  left. If ends on column A it means it crossed the board. Illegal
                    ((initial_pos >> 8)) |                                                          // move 1 square down. Not checks needed.
                    ((initial_pos << 8));                                                           // move 1 square up. Not checks needed.
        map_king_landing[init_bit] = landing;
    }

    return map_king_landing;
}

std::array<uint64_t,64> knight_position_lut(){

    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_knight_landing;

    for(Square init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);

        landing = (((initial_pos << 15) | (initial_pos >> 17)) & ~columnH)  |          // move L shape toward left. If ends on columnH it means that it crossed the board. Illegal
                (((initial_pos << 17) | (initial_pos >> 15)) & ~columnA)    |          // move L shape toward right. If ends on columnA it means that it crossed the board. Illegal 
                (((initial_pos << 6) | (initial_pos >> 10)) & ~columnGH)    |          // move L shape toward 2xleft. If ends on columnGH it means that it crossed the board. Illegal 
                (((initial_pos << 10) | (initial_pos >> 6)) & ~columnAB);              // move L shape toward 2xright. If ends on columnGH it means that it crossed the board. Illegal   

        map_knight_landing[init_bit] = landing;

    }
    return map_knight_landing;
}
uint64_t straight_lines(Square init_bit, uint64_t my_status, uint64_t opponent_status){
    uint64_t initial_pos = 0ULL;
    set_bit(initial_pos,init_bit);
    
    uint64_t current_pos = initial_pos;
    uint64_t landing = 0;
    
    while (current_pos){  // move up
        current_pos = current_pos << 8;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }    
    current_pos = initial_pos;
    while (current_pos){  // move down
        current_pos = current_pos >> 8;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    current_pos = initial_pos;
    while (current_pos){  // move left
        current_pos = (current_pos >> 1) & ~columnH;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    current_pos = initial_pos;
    while (current_pos){ // move right
        current_pos = (current_pos << 1) & ~columnA;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    return landing;

}

uint64_t diago_lines(Square init_bit, uint64_t my_status, uint64_t opponent_status){
    uint64_t initial_pos = 0ULL;
    set_bit(initial_pos,init_bit);
    uint64_t current_pos = initial_pos;
    uint64_t landing = 0;
    
    while (current_pos){  //  up right
        current_pos = (current_pos << 9) & ~columnA;;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }
    
    current_pos = initial_pos;
    while (current_pos){  // move down right
        current_pos = (current_pos >> 7) & ~columnA;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    current_pos = initial_pos;
    while (current_pos){ // move up left
        current_pos = (current_pos << 7) & ~columnH;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    current_pos = initial_pos;
    while (current_pos){  // move down left
        current_pos = (current_pos >> 9) & ~columnH;
        landing = landing | current_pos;
        if ((current_pos & opponent_status) | (current_pos & my_status)) //current pos hits on mine or my opponents pieces
            break;
    }

    return landing;
}

uint64_t rook_landings(Square init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return straight_lines(init_bit, own_occupancy, opponent_occupancy);
}
uint64_t bishop_landings(Square init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return diago_lines(init_bit, own_occupancy, opponent_occupancy);
}
uint64_t queen_landings(Square init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return diago_lines(init_bit, own_occupancy, opponent_occupancy) | straight_lines(init_bit, own_occupancy, opponent_occupancy);
}


Bitboard sliding_peices_landings(PieceType pt, Square init_square, uint64_t own_occupancy, uint64_t opponent_occupancy){
    if (pt==ROOK)
        return rook_landings(init_square, own_occupancy, opponent_occupancy);
    else if (pt==BISHOP)
        return bishop_landings(init_square, own_occupancy, opponent_occupancy);
    else{
        throw std::invalid_argument("In function init_magics PieceType must be either BISHOP or ROOK.");
        return 0;
    }
}
