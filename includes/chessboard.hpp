// chessboard.hpp
#ifndef CHESSBOARD_HPP_
#define CHESSBOARD_HPP_
#include <iostream>
#include <string>
#include <cmath>
#include "pieces.hpp"
#include "utils.hpp"
#include "lut.hpp"

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

namespace chessboard {

class ChessBoard {
public:
	explicit ChessBoard(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t);
    ~ChessBoard();
    void legalMoves();
    void printBoard();
    float evalPosition();
    bool legalStart() { return _islegal;};
    bool get_board_turn() { return board_turn;}
    Moves calculate_moves();
    Board get_board() { return board;};
    std::array<uint64_t,64> wpawn_fw_lut;
    std::array<uint64_t,64> wpawn_cap_lut;
    std::array<uint64_t,64> bpawn_fw_lut;
    std::array<uint64_t,64> bpawn_cap_lut;
    std::array<uint64_t,64> knight_lut;
    std::array<uint64_t,64> king_lut;
    
private:

    int nRows = 8;
    int nCols = 8;
    bool board_turn = true;
    bool _islegal = true;
    bool legalPosition();  
    void update_luts();
    void append_moves(Board, Piece, Moves&);

	uint64_t _white_pawns;
	uint64_t _white_bishops;
	uint64_t _white_rooks;
	uint64_t _white_knights;
	uint64_t _white_king;
	uint64_t _white_queens;
	uint64_t _black_pawns;
	uint64_t _black_bishops;
	uint64_t _black_rooks;
	uint64_t _black_knights;
	uint64_t _black_king;
	uint64_t _black_queens;

    // std::unordered_map<Piece, uint64_t> DEFAULT_STARTING_BOARD{
    // {whitePawn, 65280},{whiteBishop, 36},{whiteRook, 129},{whiteKnight, 66},{whiteKing, 16},{whiteQueen, 8},
    // {blackPawn, 71776119061217280},{blackBishop, 2594073385365405696},{blackRook, 9295429630892703744ULL},{blackKnight, 4755801206503243776},{blackKing, 1152921504606846976},{blackQueen, 576460752303423488}
    // };

    Board board{
    {whitePawn, _white_pawns},
    {whiteBishop, _white_bishops},
    {whiteRook, _white_rooks},
    {whiteKnight, _white_knights},
    {whiteKing, _white_king},
    {whiteQueen, _white_queens},
    {blackPawn, _black_pawns},
    {blackBishop, _black_bishops},
    {blackRook, _black_rooks},
    {blackKnight, _black_knights},
    {blackKing, _black_king},
    {blackQueen, _black_queens}
    };

    
};

} // #end of chessboard

#endif // CHESSBOARD
