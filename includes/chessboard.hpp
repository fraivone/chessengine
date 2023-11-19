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
    Color board_turn = BLACK;

    void legalMoves();
    void printBoard();
    void printStatusInfo();
    void RepresentBitset(uint64_t );
    float evalPosition();
    bool legalStart() { return _islegal;};

    // GAME STATUSES
    bool isCheck = false;
    bool isMate = false;
    bool isStaleMate = false;
    bool isDraw50Moves = false;
    bool isDrawRepetition = false;
    bool isDrawInsufficientMaterial = false;
    bool GameOver = false;
    bool isDraw = false;

    int get_enpassant_bit() { return en_passant_bit;}
    Color get_board_turn() { return board_turn;}

    Moves calculate_moves(Color);
    Board get_board() { return _board;};

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
    int Mate_Score = 1000;
    bool _islegal = true;
    bool castle_rights[2][2] = {false,false,false,false};
    
    bool _legalPosition();  
    bool _isCheck();
    bool _isMate();
    bool _isStaleMate();
    bool _isDrawInsufficientMaterial();
    bool _isDrawForRepetition();
    bool _isDrawFor50MovesRule();

    void _initializeCommon();
    void _ResetLandingSquares();
    void _ResetOccupancySquares();
    void _load_luts();
    void append_moves(Piece, Moves&, uint64_t, uint64_t);
    void _fill_board();
    void _update_game_status();
    void _update_board_occupancy();
    void _update_landing_squares();

    uint64_t _get_landing_squares(Piece p, int init_bit, bool attacking_squares );

    Board _board;    

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
};

} // #end of chessboard

#endif // CHESSBOARD
