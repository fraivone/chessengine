// main.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include "print.hpp"
#include "types.hpp"
#include "magic.hpp"
#include "position.hpp"
#include "gen_move.hpp"

int main(){

    init_position("rnbqkbnr/ppp1pppp/3P4/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq b6 0 3");
    RepresentBoard();
    // MoveList start;    
    // start = PawnAnyMoves(start, WHITE, 50);
    // PrintMoveList(start);
    // RepresentBitset(Moves2Bitboard(start));
    


    /*
    // Contains the attacks for each squares / each occupancy combination
    // firse index is the square index, second the magic index 
    uint64_t attacks[64][4096];
    uint64_t Rreference[4096];
    uint64_t Rmagics[64];
    uint64_t Rshifts[64];
    uint64_t magic;
    // size of the mask, current magic attempt, number of unique indeces filled in this attempt, highest counter so far 
    int bits,attempt,counter,max,collisions[4096] ; 
    
    
    // int square = 41;
    for(int square = 0; square<64; square++){
        // use the right seed (based the current row number) to converge faster
        PRNG rng(seeds[square/nRows]);
        auto allBlockers = generateAllBlockers(Line[square]);
        bits = countBitsOn(Line[square]); // number of set bits
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
            // std::cout<<"Magic generated = "<<magic<<"\n";

            counter = 0;
            // loop over all blockers
            for(auto blocker:allBlockers){

                // here I am using a legacy function to calculate landings
                // it works alright as long as I pretend that all blockers are opponent
                auto theseBlockersAttack = rook_landings(square,blocker,0);
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
                        // std::cout<<"Unharmful collision\n";
                        collisions[thisIndex]++;
                    }
                    // bad collision, need a new magic number
                    // index is already used with a different value --> break, magic number didn't work
                    else{
                    // std::cout<<"Current Magic isn't good.\tSquare = "<<square<<" Attempt = "<<attempt<<"    Best Magic Number Scored = "<<max<<" /   "<<allBlockers.size()<<"\n";
                    break;
                    }
                }
                if(counter > max) max = counter;
            }// end loop over all blockers        
        }while(counter < allBlockers.size());

        Rmagics[square] = magic;
        memcpy(attacks[square], Rreference, sizeof(Rreference));
        Rshifts[square] = 64 - bits;

        std::cout<<"For the square "<<square<<" success after "<<attempt<<" attempts.\nFound\n";
        std::cout<<"\tMagic =  "<<Rmagics[square]<<"\t Shift = "<<Rshifts[square]<<std::endl;;
        auto MaxNumberOfCollisions = *std::max_element(collisions , collisions + 4096);
        if (MaxNumberOfCollisions == 0)
            std::cout<<"\tNo collisions at all\n";
        else{
            int x = std::distance(collisions, std::find(collisions, collisions + 4096, MaxNumberOfCollisions));
            std::cout << "\tThe index "<<x<<" is the one with the most collisions ("<<MaxNumberOfCollisions<<")"<< std::endl;
        }
    }
    
    
    // TESTS
    // for(auto idx=0; idx<allBlockers.size(); idx++){
    //     auto index1 = (allBlockers[idx] * Rmagics[square]) >> Rshifts[square];
    //     if(index1==2){
    //         RepresentBitset(allBlockers[idx]);
    //         std::cout<<"The above blockers config has index = "<<index1<<std::endl;
    //         std::cout<<"Below the possible attacks"<<std::endl;
    //         RepresentBitset(attacks[square][index1]);
    //         std::cout<<"------------------------------------------"<<std::endl;
    //         std::cout<<"------------------------------------------"<<std::endl;
    //         std::cin.get();
    //     }

    // }

    */

    return 1;

}



