//attempt.hpp
#ifndef ATTEMPT_HPP_
#define ATTEMPT_HPP_
#include <iostream>
#include <string>
#include <cmath>
# include <bitset>
#include <array>
#include <algorithm>
#include "def.hpp"
#include "lut.hpp"

bool BoardisLegal(Board, LUT); //check if  exactly one king each, one king is "seen" by the other, pieces overlap
BoardStatus BoardStatusFromFEN(const std::string& );
void Reset_uint64_2Darray(uint64_t (&t)[2]);
void RepresentBitset(uint64_t number);
void update_board_occupancy(uint64_t (&t)[2], Board theBoard, bool noking = false);
void ResetPseudoMoves(Moves (&array)[2][nPieceTypes]);

// given a piece, calculates all the possible landing squares 
// * based on piece movement + enpassant 
// both proper and opponent pieces can be landed on disregarded
uint64_t get_landing_squares(uint64_t bOcc[2], uint64_t bOcc_noKing[2], Piece p, int init_bit,Color whoMoves, int en_passant_bit, bool attacking_squares );

// function overloaded 
// calc pseudo moves (slow)
void update_landing_squares(uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Moves (&pseudomoves)[2][nPieceTypes],Board current_board,Color whoMoves , int en_passant_bit);
// no calc pseudomoves (fast)
void update_landing_squares(uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Board current_board,Color whoMoves, int en_passant_bit );

bool _isCheck(Board, Color , uint64_t);

bool compareByscore(const Move &a, const Move &b);
void sortBestMoves(Moves moves, Color theColor);
bool bestWhiteByscore(const Move &a, const Move &b);
bool bestBlackByscore(const Move &a, const Move &b);


// overloaded function: without any occupancy:
//      * DontTrack (faster) --> bitboards and pseudomoves are local and locally destroyed
//      * TrackBitBoard (faster) --> 3xoccupancy + 2xlandings are passed by reference
//      * TrackPseudoMoves (slower) --> 1PM_Collector is passed as ref
//      * TrackAll (slower) --> 3xoccupancy + 2xlandings + 1PM_Collector are passed by reference
bool ChainAssertCheck(Board theBoard,uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_squares)[2], uint64_t (&landing_squares_throughKing)[2], uint64_t (&king_landing_squares)[2], Moves (&ps)[2][nPieceTypes],Color theColor,int en_passant_bit);
// function overloaded TrackBitBoard
bool ChainAssertCheck(Board theBoard,uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_squares)[2], uint64_t (&landing_squares_throughKing)[2], uint64_t (&king_landing_squares)[2],Color theColor,int en_passant_bit);
// function overloaded TrackPseudoMoves
bool ChainAssertCheck(Board theBoard,Moves (&ps)[2][nPieceTypes],Color theColor,int en_passant_bit);
// function overloaded DontTrack
bool ChainAssertCheck(Board theBoard,Color theColor,int en_passant_bit);

bool ChainAssertDraw(BoardStatus theBoardStatus);
bool isStaleMate(BoardStatus theBoardStatus, uint64_t* bOcc, uint64_t* king_landingsquares,uint64_t* landing_squares_throughKing, uint64_t* landing_squares);
bool isDrawInsufficientMaterial(Board theBoard);
bool isMate(BoardStatus theBoardStatus, Moves (&ps)[2][nPieceTypes], uint64_t (&bOcc)[2], uint64_t (&king_landing_squares)[2], uint64_t (&landing_squares_throughKing)[2]);
Board MakeMove(Move &m, Board theBoard);
bool ValidateMove(Board, Move& ,uint64_t*, int);
Moves getLegalMoves(Board, Color, Moves (&h)[2][nPieceTypes],uint64_t*, int);
Moves getLegalMoves(BoardStatus theBoardStatus, Moves (&ps)[2][nPieceTypes],uint64_t* bOcc);
ScorenMove minimax(Board theBoard, Color theColor,float alpha, float beta, int depth, int& counter);
float evalBoard(BoardStatus theBoardStatus);
float guessMoveScore(BoardStatus theFutureStatus, Move mv);


#endif // ATTEMPT