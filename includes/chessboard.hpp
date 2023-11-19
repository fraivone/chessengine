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
	explicit ChessBoard(const std::string& fen);
    ~ChessBoard();
    bool debug = true;
    bool debugverbose = false;

    void legalMoves();
    void printBoard();
    void RepresentBitset(uint64_t );
    float evalPosition();
    bool legalStart() { return _islegal;};
    bool isCheck();
    bool isMate();
    bool isStaleMate();
    bool isDrawInsufficientMaterial();
    bool isDrawForRepetition();
    bool isDrawFor50MovesRule();

    int get_enpassant_bit() { return en_passant_bit;}
    Color get_board_turn() { return board_turn;}

    Moves calculate_moves(Color);
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
    int en_passant_bit = -1;
    Color board_turn = BLACK;
    bool _islegal = true;
    bool castle_rights[2][2] = {false,false,false,false};
    
    bool legalPosition();  
    void initializeCommon();
    void ResetLandingSquares();
    void ResetOccupancySquares();
    void load_luts();
    void append_moves(Piece, Moves&, uint64_t, uint64_t);
    void fill_board();
    void update_board_occupancy();
    void update_landing_squares();
    uint64_t get_landing_squares(Piece p, int init_bit, bool attacking_squares );

    Board board;    
    std::unordered_map<Piece, Moves> test;

    /* 
    Current occupancies and landing squares help calculating check, mate, stalemate 
    Divided landing squares in pieces/king and through king or not 
    1. through king -->i.e the rook "checks" the king even through it
    2. pieces king --> stalemate has no available squares --> king can't be moved & other pieces can't be moved
    */

    uint64_t board_occupancy[2]; // idx 0 -> BLACK, idx 1 -> WHITE
    uint64_t board_occupancy_noKing[2]; // idx 0 -> BLACK, idx 1 -> WHITE
    
    uint64_t pieces_landingsquares[2]; // idx 0 -> BLACK, idx 1 -> WHITE
    uint64_t king_landingsquares[2]; // idx 0 -> BLACK, idx 1 -> WHITE
    uint64_t pieces_landingsquares_throughKing[2]; // idx 0 -> BLACK, idx 1 -> WHITE

	uint64_t _white_pawns = 0ULL;
	uint64_t _white_bishops = 0ULL;
	uint64_t _white_rooks = 0ULL;
	uint64_t _white_knights = 0ULL;
	uint64_t _white_king = 0ULL;
	uint64_t _white_queens = 0ULL;
	uint64_t _black_pawns = 0ULL;
	uint64_t _black_bishops = 0ULL;
	uint64_t _black_rooks = 0ULL;
	uint64_t _black_knights = 0ULL;
	uint64_t _black_king = 0ULL;
	uint64_t _black_queens = 0ULL;

    // std::unordered_map<Piece, uint64_t> DEFAULT_STARTING_BOARD{
    // {whitePawn, 65280},{whiteBishop, 36},{whiteRook, 129},{whiteKnight, 66},{whiteKing, 16},{whiteQueen, 8},
    // {blackPawn, 71776119061217280},{blackBishop, 2594073385365405696},{blackRook, 9295429630892703744ULL},{blackKnight, 4755801206503243776},{blackKing, 1152921504606846976},{blackQueen, 576460752303423488}
    // };

    
};

} // #end of chessboard

#endif // CHESSBOARD
