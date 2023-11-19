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

/*
Functions and LUT generators to determine the possible landing
squares of the pieces. Landing on my own or my opponents pieces
is allowed because:
1. Landing on my own piece serves as Piece is defended
2. Landing on my oppoenents pieces serves as Piece is captured
*/

std::array<uint64_t,64> wpawn_straight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_wpawnstraight_landing;
    for(int init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   (initial_pos << 8);
        if (init_bit >=8 & init_bit<= 15 ) // paw is on its starting position
            landing |=   (initial_pos << 16);
        map_wpawnstraight_landing[init_bit] = landing;
    }
    return map_wpawnstraight_landing;
}

std::array<uint64_t,64> bpawn_straight_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_bpawnstraight_landing;
    for(int init_bit =0; init_bit<64; init_bit++){
        initial_pos = 0ULL;
        set_bit(initial_pos,init_bit);
        landing =   (initial_pos >> 8);
        if (init_bit >=48 & init_bit<= 56 ) // paw is on its starting position
            landing |=   (initial_pos >> 16);
        map_bpawnstraight_landing[init_bit] = landing;
    }
    return map_bpawnstraight_landing;
}

std::array<uint64_t,64> wpawn_diagcapture_lut(){
    uint64_t initial_pos,landing;
    std::array<uint64_t,64> map_wpawndiagcapture_landing;
    for(int init_bit =0; init_bit<64; init_bit++){
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
    for(int init_bit =0; init_bit<64; init_bit++){
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

    for(int init_bit =0; init_bit<64; init_bit++){
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

    for(int init_bit =0; init_bit<64; init_bit++){
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
uint64_t straight_lines(int init_bit, uint64_t my_status, uint64_t opponent_status){
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

uint64_t diago_lines(int init_bit, uint64_t my_status, uint64_t opponent_status){
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

uint64_t rook_landings(int init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return straight_lines(init_bit, own_occupancy, opponent_occupancy);
}
uint64_t bishop_landings(int init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return diago_lines(init_bit, own_occupancy, opponent_occupancy);
}
uint64_t queen_landings(int init_bit, uint64_t own_occupancy, uint64_t opponent_occupancy){
    return diago_lines(init_bit, own_occupancy, opponent_occupancy) | straight_lines(init_bit, own_occupancy, opponent_occupancy);
}