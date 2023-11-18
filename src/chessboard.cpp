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
    fill_board();
    update_luts();   // load the look up tables
    legalPosition(); // check if starting position is valid    
}

ChessBoard::ChessBoard(const std::string& fen){
    const size_t size = fen.size();
    size_t iter = 0;
    int index = 56;
    std::unordered_map<char, uint64_t*> map_fen_chars ={
        {'P',&_white_pawns},
        {'B',&_white_bishops},
        {'N',&_white_knights},
        {'R',&_white_rooks},
        {'Q',&_white_queens},
        {'K',&_white_king},
        {'p',&_black_pawns},
        {'b',&_black_bishops},
        {'n',&_black_knights},
        {'r',&_black_rooks},
        {'q',&_black_queens},
        {'k',&_black_king},
    };

    // pieces on the board
    for (; (iter < size) and (fen[iter] != ' '); iter++)
    {
        if (fen[iter] == '/'){
            index = index -  16;
            continue;
        }
        if (isdigit(fen[iter]))
            index += (fen[iter] - '0'); // converts char digit to int. `5` to 5
        else{
            if (debugverbose)
                std::cout<<"board["<<index<<"] = "<<fen[iter]<<std::endl;
            *map_fen_chars[fen[iter]]  |=  uint64_t(pow(2,index));
            
            ++index;
        }
    }

    fill_board();
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


void ChessBoard::fill_board(){
    board ={
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


uint64_t ChessBoard::opponentboard_status(Color color ){
    uint64_t opponent_status;
    if (color == WHITE)
        opponent_status = board[blackPawn] | board[blackBishop] | board[blackKnight] | board[blackRook] | board[blackQueen] | board[blackKing];
    else if (color == BLACK)
        opponent_status = board[whitePawn] | board[whiteBishop] | board[whiteKnight] | board[whiteRook] | board[whiteQueen] | board[whiteKing];
    return opponent_status;
}
uint64_t ChessBoard::ownboard_status(Color color ){
    uint64_t my_status;
    if (color == WHITE)
        my_status = board[whitePawn] | board[whiteBishop] | board[whiteKnight] | board[whiteRook] | board[whiteQueen] | board[whiteKing];
    else if (color == BLACK)
        my_status = board[blackPawn] | board[blackBishop] | board[blackKnight] | board[blackRook] | board[blackQueen] | board[blackKing];
    std::cout<<"My board is "<< my_status<<std::endl;
    return my_status;
}

void ChessBoard::append_moves(Piece p, Moves& move_collector, uint64_t own_status, uint64_t opponent_status){
    uint64_t all_moves;
    auto piece_positions = board[p];
    // loop through all pieces of type p
    while(piece_positions){
        auto piece_init_bit = pop_LSB(piece_positions);
        switch (p.piece_type) {
            case QUEEN:
                all_moves = queen_moves(piece_init_bit, board, p.color, own_status, opponent_status) & ~own_status;
                break;
            case KING:
                all_moves = king_lut[piece_init_bit] & ~own_status;
                break;
            case KNIGHT:
                all_moves = knight_lut[piece_init_bit] & ~own_status;
                break;
            case PAWN:
                all_moves = p.color ? wpawn_fw_lut[piece_init_bit] & ~own_status : bpawn_fw_lut[piece_init_bit] & ~own_status;
                break;
            case BISHOP:
                all_moves = bishop_moves(piece_init_bit, board, p.color, own_status, opponent_status) & ~own_status;
                break;
            case ROOK:
                all_moves = rook_moves(piece_init_bit, board, p.color, own_status, opponent_status) & ~own_status;
                break;
            default:
                break;
        }
        while(all_moves){
            auto piece_end_bit = pop_LSB(all_moves);
            
            if (p.color == WHITE){
                if ( bool(get_bit(board[blackKing],piece_end_bit)) ){ // white piece checks black king
                    move_collector.push_back(Move(piece_init_bit, piece_end_bit, p, true));                
                    if (debug)
                        std::cout << "The " << convert_color[p.color] << " "
                        << convert_enum[p.piece_type]
                        << " is checking the king in " << bit2notation(piece_end_bit)<<std::endl;
                }

                else
                    move_collector.push_back(Move(piece_init_bit, piece_end_bit, p, false));
            }
            else if ( p.color == BLACK){
                if( bool(get_bit(board[whiteKing],piece_end_bit)) ){ // white piece checks black king
                move_collector.push_back(Move(piece_init_bit, piece_end_bit, p, true));                
                if (debug)
                    std::cout << "The " << convert_color[p.color] << " "
                      << convert_enum[p.piece_type]
                      << " is checking the king in " << bit2notation(piece_end_bit)<<std::endl;
            }
            else
                move_collector.push_back(Move(piece_init_bit, piece_end_bit, p, false));
            }
        }
    }
}

Moves ChessBoard::calculate_moves(Color color){
    Moves move_collector;
    uint64_t ownstatus = ownboard_status(color);
    uint64_t opponent_status = opponentboard_status(color);
    if (color == WHITE){
        append_moves(whitePawn, move_collector, ownstatus, opponent_status);
        append_moves(whiteKnight, move_collector, ownstatus, opponent_status);
        append_moves(whiteBishop, move_collector, ownstatus, opponent_status);
        append_moves(whiteRook, move_collector, ownstatus, opponent_status);
        append_moves(whiteQueen, move_collector, ownstatus, opponent_status);
        append_moves(whiteKing, move_collector, ownstatus, opponent_status);
        }
    else if (color == BLACK){
        append_moves(blackPawn, move_collector, ownstatus, opponent_status);
        append_moves(blackKnight, move_collector, ownstatus, opponent_status);
        append_moves(blackBishop, move_collector, ownstatus, opponent_status);
        append_moves(blackRook, move_collector, ownstatus, opponent_status);
        append_moves(blackQueen, move_collector, ownstatus, opponent_status);
        append_moves(blackKing, move_collector, ownstatus, opponent_status);
    }
    return move_collector;
}
