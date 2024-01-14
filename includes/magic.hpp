/** @file */
#pragma once
#include <vector>
#include <cassert> // for assert
#include <stdexcept> // Required to throw errors
#include <algorithm> // find max in array
#include <string.h> //memcopy
#include "types.hpp"
#include "lut.hpp"


/// LUT that given a input bit returns a maks
/// Each mask cleans the occupancy board to get only the relevant bits for the rook movement
const LUT Line = {282578800148862ULL,565157600297596ULL,1130315200595066ULL,2260630401190006ULL,4521260802379886ULL,9042521604759646ULL,18085043209519166ULL,36170086419038334ULL,282578800180736ULL,565157600328704ULL,1130315200625152ULL,2260630401218048ULL,4521260802403840ULL,9042521604775424ULL,18085043209518592ULL,36170086419037696ULL,282578808340736ULL,565157608292864ULL,1130315208328192ULL,2260630408398848ULL,4521260808540160ULL,9042521608822784ULL,18085043209388032ULL,36170086418907136ULL,282580897300736ULL,565159647117824ULL,1130317180306432ULL,2260632246683648ULL,4521262379438080ULL,9042522644946944ULL,18085043175964672ULL,36170086385483776ULL,283115671060736ULL,565681586307584ULL,1130822006735872ULL,2261102847592448ULL,4521664529305600ULL,9042787892731904ULL,18085034619584512ULL,36170077829103616ULL,420017753620736ULL,699298018886144ULL,1260057572672512ULL,2381576680245248ULL,4624614895390720ULL,9110691325681664ULL,18082844186263552ULL,36167887395782656ULL,35466950888980736ULL,34905104758997504ULL,34344362452452352ULL,33222877839362048ULL,30979908613181440ULL,26493970160820224ULL,17522093256097792ULL,35607136465616896ULL,9079539427579068672ULL,8935706818303361536ULL,8792156787827803136ULL,8505056726876686336ULL,7930856604974452736ULL,6782456361169985536ULL,4485655873561051136ULL,9115426935197958144ULL};
/// LUT that given a input bit returns a maks
/// Each mask cleans the occupancy board to get only the relevant bits for the bishop movement
const LUT Diago = {18049651735527936ULL,70506452091904ULL,275415828992ULL,1075975168ULL,38021120ULL,8657588224ULL,2216338399232ULL,567382630219776ULL,9024825867763712ULL,18049651735527424ULL,70506452221952ULL,275449643008ULL,9733406720ULL,2216342585344ULL,567382630203392ULL,1134765260406784ULL,4512412933816832ULL,9024825867633664ULL,18049651768822272ULL,70515108615168ULL,2491752130560ULL,567383701868544ULL,1134765256220672ULL,2269530512441344ULL,2256206450263040ULL,4512412900526080ULL,9024834391117824ULL,18051867805491712ULL,637888545440768ULL,1135039602493440ULL,2269529440784384ULL,4539058881568768ULL,1128098963916800ULL,2256197927833600ULL,4514594912477184ULL,9592139778506752ULL,19184279556981248ULL,2339762086609920ULL,4538784537380864ULL,9077569074761728ULL,562958610993152ULL,1125917221986304ULL,2814792987328512ULL,5629586008178688ULL,11259172008099840ULL,22518341868716544ULL,9007336962655232ULL,18014673925310464ULL,2216338399232ULL,4432676798464ULL,11064376819712ULL,22137335185408ULL,44272556441600ULL,87995357200384ULL,35253226045952ULL,70506452091904ULL,567382630219776ULL,1134765260406784ULL,2832480465846272ULL,5667157807464448ULL,11333774449049600ULL,22526811443298304ULL,9024825867763712ULL,18049651735527936ULL};

/// namespace to encapsulate the variables needed for magic bitboards
namespace MagicVariables{
    void init();
    /// Array of Magic structs for the ROOK
    extern Magic RookMagics[nCols*nRows];
    /// Array of Magic structs for the BISHOP
    extern Magic BishopMagics[nCols*nRows];
    /// LUT with magic bitboard attacks for the rook
    /// first index is the square index, second the magic index 
    /// 4096 is the largest number of blockers
    extern uint64_t Rattacks[nCols*nRows][4096];
    /// LUT with magic bitboard attacks for the bishop
    /// first index is the square index, second the magic index 
    /// 4096 is the largest number of blockers
    extern uint64_t Battacks[nCols*nRows][4096];
}


/*! Mirrors a byte (i.e. 00100011 --> 11000100 )
    \param uint8_t 
    \return unsigned char
*/  
unsigned char reverseByte(uint8_t);

/*! Mirrors a Bitboard horizontally.
    \param Bitboard 
    \return Bitboard
*/  
Bitboard MirrorBitboardHoriz(Bitboard);

/*! Prints on screen 64 bitboards, one for each square and in order. Each bitboard represents the possible landings from the starting position in case of linear movements.\n Used to generate the LUT LineMask.
    \return void
*/  
void generateLineMagicMask();


/*! Prints on screen 64 bitboards, one for each square and in order. Each bitboard represents the possible landings from the starting position in case of diagonal movements.\n Used to generate the LUT DiagoMask.
    \return void
*/  
void generateDiagoMagicMask();


/* Generate all blockers positions for the input move mask. This is a service method to prepare the magic bitboard. Since the blockers will be computed by doing the *AND* with the mask, I just use the mask to generate all possbile blockers
    \param Bitboard Mask
    \return std::vector<Bitboard>
*/
std::vector<Bitboard> generateAllBlockers(Bitboard Mask);



/* Initializes magic bitboard. Including attacks, magic number and magic bitshifts
    \param PieceType pt which piece's magic? Either ROOK or BISHOP
    \param Bitboard table[nCols*nRows][] the LUT itself, passed by reference
    \param Magic magics[nCols*nRows] array of structs to store magic numbers, bitshifts
*/
void generate_magics(PieceType pt, Bitboard (&table)[nCols*nRows][4096], Magic (&MagicArray)[nRows*nCols]);

/// initialize magic bitboard arrays and magic struct arrays
void init_magics();
/// import magic bitboard arrays and magic struct arrays
/// Not yet implemented as it is not needed
void import_magics(Magic RookMagics[nCols*nRows], uint64_t Rattacks[nCols*nRows][4096],Magic BishopMagics[nCols*nRows], uint64_t Battacks[nCols*nRows][4096]);

/* Get moves for the bishop using magic bitboards
    \param \param Square to be considered (0-63) square the starting square [0,64)
    \param Bitboard all_pieces_occupancy configuration of all pieces on the board
    \return Bitboard with all landings INCLUDING capturing own pieces
*/
Bitboard get_bishop_landings(Square square, Bitboard all_pieces_occupancy);

/* Get moves for the rook using magic bitboards
    \param Square square the starting square [0,64)
    \param Bitboard all_pieces_occupancy configuration of all pieces on the board
    \return Bitboard with all landings INCLUDING capturing own pieces
*/
Bitboard get_rook_landings(Square square, Bitboard all_pieces_occupancy);

/* Get moves for a given sliding piece using magic bitboards
    \param PieceType pt which piece is movign? ROOK, BISHOP or QUEEN
    \param Square Square the starting square [0,64)
    \param Bitboard all_pieces_occupancy configuration of all pieces on the board
    \return Bitboard with all landings INCLUDING capturing own pieces
*/
Bitboard get_sliding_landings(PieceType pt, Square square, Bitboard all_pieces_occupancy);



// Optimal PRNG seeds to pick the correct magics in the shortest time
//https://github.com/official-stockfish/Stockfish/blob/fbdf5d94a9a42acb92720a5896b16c92931ec3de/src/bitboard.cpp#L143
const int seeds[] = {8977, 44560, 54343, 38998, 5731, 95205, 104912, 17020};

// https://github.com/official-stockfish/Stockfish/blob/fbdf5d94a9a42acb92720a5896b16c92931ec3de/src/misc.h#L127
/// Pseudo random number generator, convenient for magics
class PRNG {

    uint64_t s;

    uint64_t rand64() {

        s ^= s >> 12, s ^= s << 25, s ^= s >> 27;
        return s * 2685821657736338717LL;
    }

   public:
    PRNG(uint64_t seed) :
        s(seed) {
        assert(seed);
    }

    template<typename T>
    T rand() {
        return T(rand64());
    }

    // Special generator used to fast init magic numbers.
    // Output values only have 1/8th of their bits set on average.
    template<typename T>
    T sparse_rand() {
        return T(rand64() & rand64() & rand64());
    }
};