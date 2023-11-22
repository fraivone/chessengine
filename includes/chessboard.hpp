// chessboard.hpp
#ifndef CHESSBOARD_HPP_
#define CHESSBOARD_HPP_
#include <iostream>
#include <string>
#include <cmath>
#include <array>
#include "def.hpp"
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
	explicit ChessBoard(uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,uint64_t,Color);
	explicit ChessBoard(const std::string& fen);
    ~ChessBoard();
    bool debug = true;
    bool debugverbose = false;
    Color board_turn = BLACK;

    // UTILS
    void legalMoves();
    void printBoard(Board);
    void printStatusInfo();
    void RepresentBitset(uint64_t );
    Moves getLegalMoves(Color color, Moves (&ps)[2][nPieceTypes]);
    Moves tempFunc(){    return getLegalMoves(board_turn, PM_collector); }
    bool legalStart() { return _islegal;};
    float evalBoard(Board, Color);
    int get_enpassant_bit() { return en_passant_bit;}
    Color get_board_turn() { return board_turn;}
    Board get_board() { return _board;};
    Board PublicMakeMove(Move&, Board);

    // GAME STATUSES
    bool isCheck = false; // current position is in check
    bool nextMove_isCheck=false;
    bool isMate = false;
    bool isStaleMate = false;
    bool isDraw50Moves = false;
    bool isDrawRepetition = false;
    bool isDrawInsufficientMaterial = false;
    bool GameOver = false;
    bool isDraw = false;
    float score=0;

    std::array<uint64_t,64> wpawn_fw_lut;
    std::array<uint64_t,64> wpawn_doublefw_lut;
    std::array<uint64_t,64> wpawn_cap_lut;
    std::array<uint64_t,64> bpawn_fw_lut;
    std::array<uint64_t,64> bpawn_doublefw_lut;
    std::array<uint64_t,64> bpawn_cap_lut;
    std::array<uint64_t,64> knight_lut;
    std::array<uint64_t,64> king_lut;
    
private:
    int nRows = 8;
    int nCols = 8;
    int en_passant_bit = -1;
    bool _islegal = true;
    bool castle_rights[2][2] = {false,false,false,false};
    
    bool _legalPosition();  
    bool _isCheck(Board, Color , uint64_t);
    bool _isMate();
    bool _isStaleMate();
    bool _isDrawInsufficientMaterial();
    bool _isDrawForRepetition();
    bool _isDrawFor50MovesRule();
    bool _ChainAssertCheck(Board theBoard, 
                           uint64_t (&bOcc)[2], 
                           uint64_t (&bOcc_noKing)[2], 
                           uint64_t (&landing_squares)[2],
                           uint64_t (&landing_squares_throughKing)[2], 
                           uint64_t (&king_landing_squares)[2], 
                           Moves (&ps)[2][nPieceTypes],
                           Color theColor);


    void _initializeCommon();
    // void _updateAfterMove();
    void _ResetPseudoMoves(Moves (&array)[2][nPieceTypes]);
    void _ResetLandingSquares(uint64_t (&t)[2]);
    // resets the occupancy array by reference
    void _ResetOccupancySquares(uint64_t (&t)[2]);
    void _load_luts();
    void append_moves(Piece, Moves&, uint64_t, uint64_t);
    void _init_board();
    void _update_game_status();
    void _update_board_occupancy(uint64_t (&t)[2], Board theBoard, bool noking );
    void _update_landing_squares(uint64_t bOcc[2], uint64_t bOcc_noKing[2], uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Moves (&pseudomoves)[2][nPieceTypes],Board current_board);
    
    // Move methods
    bool _ValidateMove(Move&);
    Board _MakeMove(Move&, Board);
    Board _UndoMove(Move&, Board);

    uint64_t _get_landing_squares(uint64_t bOcc[2], uint64_t bOcc_noKing[2], Piece p, int init_bit, bool attacking_squares );
    uint64_t piece_landing_squares[12][64];

    Board _board;    

    /* 
    Current occupancies and landing squares help calculating check, mate, stalemate 
    Divided landing squares in pieces/king and through king or not 
    1. through king -->i.e the rook "checks" the king even through it
    2. pieces king --> stalemate has no available squares --> king can't be moved & other pieces can't be moved
    */
    // idx 0 -> BLACK, idx 1 -> WHITE
    uint64_t board_occupancy[2]; 
    uint64_t board_occupancy_noKing[2]; 
    
    uint64_t pieces_landingsquares[2]; 
    uint64_t pieces_landingsquares_throughKing[2]; 
    uint64_t king_landingsquares[2]; 
    /*
    PseudoMove Collectors of Pseudo Moves
    pseudo_move_collector[p.index].size()
    _ValidateMove will then check if a move is valid
    */
    Moves PM_collector[2][nPieceTypes];
    // Moves PM_collector_throughKing[2][nPieceTypes]; ;

    Board nextMove_board;
    uint64_t nextMove_board_occupancy[2]; 
    uint64_t nextMove_board_occupancy_noKing[2]; 
    uint64_t nextMove_pieces_landingsquares[2]; 
    uint64_t nextMove_pieces_landingsquares_throughKing[2]; 
    uint64_t nextMove_king_landingsquares[2]; 
    Moves nextMove_PM_collector[2][nPieceTypes];

    // idx 0 -> BLACK, idx 1 -> WHITE
    Moves LegalMoves[2]; 


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


float searchBestMove(chessboard::ChessBoard* b, int depth, int ncalls = 0 );
ScorenMove searchBestMove2(Board theBoard, Color theColor,float alpha, float beta, int depth);

#endif // CHESSBOARD
