#include <iostream>
#include "chessboard.hpp"
# include <bitset>

using chessboard::ChessBoard;

ChessBoard::ChessBoard(uint64_t wps,uint64_t wbs,uint64_t wns,uint64_t wrs,uint64_t wqs,uint64_t wk,uint64_t bps,uint64_t bbs,uint64_t bns,uint64_t brs,uint64_t bqs,uint64_t bk): 
    _white_pawns(wps), 
    _white_bishops(wbs),
    _white_knights(wns),
    _white_rooks(wrs),
    _white_queens(wqs),
    _white_king(wk),
    _black_pawns(bps),
    _black_bishops(bbs),
    _black_knights(bns),
    _black_rooks(brs),
    _black_queens(bqs),
    _black_king(bk) 
    {
    update_luts();   // load the look up tables
    legalPosition(); // check if starting position is valid    
}

ChessBoard::~ChessBoard() = default; 

// void generate_all_moves(bool board_turn){

//     if (board_turn)
//         generate_white_moves();
//     else
//         generate_black_moves();
// }


void ChessBoard::update_luts(){
    knight_lut = knight_position_lut();
    wpawn_fw_lut = wpawn_straight_lut();
    wpawn_cap_lut = wpawn_diagcapture_lut();
    bpawn_fw_lut = bpawn_straight_lut();
    bpawn_cap_lut = bpawn_diagcapture_lut();
    king_lut =  king_position_lut();
}

float ChessBoard::evalPosition(){
    float white_score = 0.;
    float black_score = 0.;
    for (const auto& pair : board) {
        Piece this_piece = pair.first;
        uint64_t value = pair.second;
        int number_of_pieces = std::bitset<64>(value).count();
        
        // piece is white
        if (this_piece.color)     
            white_score += number_of_pieces*this_piece.piece_value;
        // piece is black
        else  
            black_score += number_of_pieces*this_piece.piece_value;
    }

    return white_score - black_score;
}

void ChessBoard::printBoard(){
    // init the grid
    std::string grid[nRows][nCols];
    for(int ri=nRows-1; ri>=0; ri--){
        for(int ci=0; ci<nCols; ci++){
            grid[ri][ci] = "...";
        }
    }
    // fill the grid
    for (const auto& pair : board) {
        Piece key = pair.first;
        uint64_t value = pair.second;
        for (int i=0; i<nCols*nRows; i++){
            int row = i/nRows;
            int col = i%nCols;
                if (get_bit(value,i) !=0 ){    
                    grid[row][col] = key.name +" ";
                }
        }
    }
    // print the grid
    for(int ri=nRows-1; ri>=0; ri--){
        std::cout<<"\n";
        for(int ci=0; ci<nCols; ci++){
            std::cout<< grid[ri][ci]<<" ";
        }
    std::cout<<"\n"<<std::endl;

    }
    std::cout<<"\n";
}


bool ChessBoard::legalPosition(){
    _islegal = true;

    // exactly one king each
    if (std::bitset<64>(board[blackKing]).count() != 1 or std::bitset<64>(board[whiteKing]).count() != 1){
        _islegal = false;
        std::cout<<"Too many/few kings on the board.Illegal position\nExiting..."<<std::endl;
    }
    int bking_bit = get_LSB(board[blackKing]);
    int wking_bit = get_LSB(board[whiteKing]);
    auto _x = king_lut[bking_bit] & board[whiteKing]; //white king is "seen" by the black king
    auto _y = king_lut[wking_bit] & board[blackKing]; //black king is "seen" by the white king
    if( (_x !=0) | (_y !=0)  ){
        _islegal = false;
        std::cout<<"Kings are checking each other.Illegal position.\nExiting..."<<std::endl;
    }

    // pieces don't overlap
    for (const auto& first_pair : board) {
        if (_islegal == false) break;
        Piece firstkey = first_pair.first;
        uint64_t firstvalue = first_pair.second;
        for (const auto& second_pair : board) {
            Piece secondkey = second_pair.first;
            uint64_t secondvalue = second_pair.second;
            if (secondkey == firstkey) continue;
            if ((secondvalue & firstvalue) != 0){
                std::cout<<firstkey.name<< " and "<< secondkey.name<< "  overlap. Illegal position.\nExiting..."<<std::endl;
                _islegal = false;
                break;
            }
            
        }
    }
    
    return _islegal;
}


void ChessBoard::append_moves(Board b, Piece p, Moves& move_collector){
    uint64_t all_moves;
    while(board[p]){
        auto piece_init_bit = pop_LSB(board[p]);
        switch (p.piece_type) {
            case QUEEN:
                all_moves = queen_moves(piece_init_bit, b, p.color) & ~ownboard_status(b, p.color);
                break;
            case KING:
                all_moves = king_lut[piece_init_bit] & ~ownboard_status(b, p.color);
                break;
            case KNIGHT:
                all_moves = king_lut[piece_init_bit] & ~ownboard_status(b, p.color);
                break;
            case PAWN:
                all_moves = p.color ? wpawn_fw_lut[piece_init_bit] & ~ownboard_status(b, p.color) : bpawn_fw_lut[piece_init_bit] & ~ownboard_status(b, p.color);
                break;
            case BISHOP:
                all_moves = bishop_moves(piece_init_bit, b, p.color) & ~ownboard_status(b, p.color);
                break;
            case ROOK:
                all_moves = rook_moves(piece_init_bit, b, p.color) & ~ownboard_status(b, p.color);
                break;
            default:
                break;
        }
        while(all_moves){
            auto piece_end_bit = pop_LSB(all_moves);
            move_collector.push_back(Move(piece_init_bit, piece_end_bit, p));
        }
    }
}

Moves ChessBoard::calculate_moves(){
    Moves move_collector;
    if (this->get_board_turn()){
        append_moves(this->get_board(), whitePawn, move_collector);
        append_moves(this->get_board(), whiteKnight, move_collector);
        append_moves(this->get_board(), whiteBishop, move_collector);
        append_moves(this->get_board(), whiteRook, move_collector);
        append_moves(this->get_board(), whiteQueen, move_collector);
        append_moves(this->get_board(), whiteKing, move_collector);
        }
    else{
        append_moves(this->get_board(), blackPawn, move_collector);
        append_moves(this->get_board(), blackKnight, move_collector);
        append_moves(this->get_board(), blackBishop, move_collector);
        append_moves(this->get_board(), blackRook, move_collector);
        append_moves(this->get_board(), blackQueen, move_collector);
        append_moves(this->get_board(), blackKing, move_collector);
    }
    return move_collector;
}
