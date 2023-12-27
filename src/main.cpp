// main.cpp
#include <iostream>
#include <algorithm> // find in vector
#include <vector>
#include <chrono>
#include <random> //random number
#include "lut.hpp"
#include "print.hpp"
#include "types.hpp"
#include "magic.hpp"

int main(){
    // random numbers machinery
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;

    auto allBlockers = generateAllBlockers(Line[48]);
    std::vector<Bitboard> attacks;
    // TODO
    // Fill the vector attacks with bitboards. For each element in allBlockers, attacks contains the allowed attacks at the same index
    // first fix the old function that calculates the landing based on the new logic
    // * no borders
    // * can land on friendly pieces




    std::vector<uint64_t> all_magic_indeces;
    int bits = countBitsOn(Line[48]); // number of set bits

    while(1){
        uint64_t magic = dis(gen);
        std::cout<<"Magic generated = "<<magic<<"\n";
        int counter = 0;
        for(auto blocker:allBlockers){
            auto thisIndex = ( (blocker & Line[48]) * magic ) >> (64-bits);
            if ( std::find(all_magic_indeces.begin(), all_magic_indeces.end(), thisIndex) != all_magic_indeces.end() ){
                std::cout<<"Magic index is repeated after "<<counter<<". Aborting\n";
                all_magic_indeces.clear();
                break;
            }
            else{
                all_magic_indeces.push_back(thisIndex);
                counter++;
            }
        }
        if(counter == allBlockers.size()-1){
            std::cout<<"Success\n";
            break;
        }
        
    }
    return 1;

}



