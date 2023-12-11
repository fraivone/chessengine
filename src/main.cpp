// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include "lut.hpp"
#include "print.hpp"
#include "types.hpp"

int main(){

    Bitboard whitePawn = 1<<7;
    std::cout<< bpawn_cap_lut[whitePawn] + bpawn_fw_lut[whitePawn] + bpawn_doublefw_lut[whitePawn]<<std::endl;
    RepresentBitset(whitePawn);
    return 1;

}


