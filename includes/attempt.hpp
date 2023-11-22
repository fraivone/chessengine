//attempt.hpp
#ifndef ATTEMPT_HPP_
#define ATTEMPT_HPP_
#include <iostream>
#include <string>
#include <cmath>
# include <bitset>
#include <array>
#include "def.hpp"
#include "lut.hpp"

bool BoardisLegal(Board, LUT); //check if  exactly one king each, one king is "seen" by the other, pieces overlap
BoardStatus BoardStatusFromFEN(const std::string& );
void Reset_uint64_2Darray(uint64_t (&t)[2]);
void update_board_occupancy(uint64_t (&t)[2], Board theBoard, bool noking = false);
void ResetPseudoMoves(Moves (&array)[2][nPieceTypes]);
uint64_t get_landing_squares(uint64_t bOcc[2], uint64_t bOcc_noKing[2], Piece p, int init_bit, bool attacking_squares );
void update_landing_squares(uint64_t bOcc[2], 
                            uint64_t bOcc_noKing[2], 
                            uint64_t (&landing_sq)[2],
                            uint64_t (&landing_sq_throughKing)[2],
                            uint64_t (&king_landing_sq)[2],
                            Moves (&pseudomoves)[2][nPieceTypes],
                            Board current_board,
                            LUT king_lut);



#endif // ATTEMPT