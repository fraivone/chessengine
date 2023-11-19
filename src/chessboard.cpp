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
        initializeCommon();
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
    board_turn = fen[iter + 1] == 'w' ? Color::WHITE : Color::BLACK;

    for (iter += 3; (iter < size )and (fen[iter] != ' '); iter++)
    {

        if (fen[iter] == 'k')
            castle_rights[Color::BLACK][Castle::king_side] = true;

        else if (fen[iter] == 'K')
            castle_rights[Color::WHITE][Castle::king_side] = true;

        else if (fen[iter] == 'q')
            castle_rights[Color::BLACK][Castle::queen_side] = true;

        else if (fen[iter] == 'Q')
            castle_rights[Color::WHITE][Castle::queen_side] = true;
    }

    if (fen[iter+1] == '-') // no enpassant
        en_passant_bit = -1;
    else
        en_passant_bit = chars_to_bit(fen[iter + 1], fen[iter + 2]);
    //TODO Implemnt half move and full move for draw for repetition

    initializeCommon();
    
}

ChessBoard::~ChessBoard() = default; 


void ChessBoard::initializeCommon(){
    fill_board();
    legalPosition(); // check if starting position is valid, 2 kings, Kings aren't in mutual check, pieces don't overlap
    load_luts();   // load the look up tables for landing squares of leaping pieces

    update_board_occupancy(); // all positions occupied by black/white
    update_landing_squares(); // all possible landing squares for black/white
}

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

void ChessBoard::load_luts(){
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

    return (white_score - black_score)/100.;
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


void ChessBoard::PrintNumber(uint64_t number){
    // init the grid
    std::string grid[nRows][nCols];
    for(int ri=nRows-1; ri>=0; ri--){
        for(int ci=0; ci<nCols; ci++){
            grid[ri][ci] = ".... ";
        }
    }
    // fill the grid
    for (int i=0; i<nCols*nRows; i++){
        int row = i/nRows;
        int col = i%nCols;
        if (get_bit(number,i) !=0 ){    
            grid[row][col] = "#### ";
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
        // std::cout<<"Too many/few kings on the board.Illegal position\nExiting..."<<std::endl;
        return false;
    }
    int bking_bit = get_LSB(board[blackKing]);
    int wking_bit = get_LSB(board[whiteKing]);
    auto _x = king_lut[bking_bit] & board[whiteKing]; //white king is "seen" by the black king
    auto _y = king_lut[wking_bit] & board[blackKing]; //black king is "seen" by the white king
    if( (_x !=0) | (_y !=0)  ){
        _islegal = false;
        // std::cout<<"Kings are checking each other.Illegal position.\nExiting..."<<std::endl;
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
                // std::cout<<firstkey.name<< " and "<< secondkey.name<< "  overlap. Illegal position.\nExiting..."<<std::endl;
                _islegal = false;
                break;
            }
            
        }
    }
    
    return _islegal;
}

void ChessBoard::update_board_occupancy(){
    board_occupancy[BLACK] = board[blackPawn] | board[blackBishop] | board[blackKnight] | board[blackRook] | board[blackQueen] | board[blackKing];
    board_occupancy[WHITE] = board[whitePawn] | board[whiteBishop] | board[whiteKnight] | board[whiteRook] | board[whiteQueen] | board[whiteKing];
    
    board_occupancy_noKing[BLACK] = board[blackPawn] | board[blackBishop] | board[blackKnight] | board[blackRook] | board[blackQueen];
    board_occupancy_noKing[WHITE] = board[whitePawn] | board[whiteBishop] | board[whiteKnight] | board[whiteRook] | board[whiteQueen];
}

uint64_t ChessBoard::get_landing_squares(Piece p, int piece_init_bit, bool through_king = false){
    uint64_t own_status,opponent_status;
    own_status = p.color == WHITE ? board_occupancy[WHITE] : board_occupancy[BLACK];
    if (!through_king){
        opponent_status = p.color == WHITE ? board_occupancy[BLACK] : board_occupancy[WHITE];
    }
    else{
        opponent_status = p.color == WHITE ? board_occupancy_noKing[BLACK] : board_occupancy_noKing[WHITE];
    }
    uint64_t landing_squares = 0ULL;
    switch (p.piece_type) {
            case QUEEN:
                landing_squares |= queen_landings(piece_init_bit, own_status, opponent_status);
                break;
            case KING:
                landing_squares |= king_lut[piece_init_bit];
                break;
            case KNIGHT:
                landing_squares |= knight_lut[piece_init_bit];
                break;
            case PAWN:
                // include forward movement if not opposed
                // diagonal captures if opponent exists
                landing_squares |= p.color ? (wpawn_fw_lut[piece_init_bit] & ~opponent_status) | (wpawn_cap_lut[piece_init_bit] & opponent_status) : (bpawn_fw_lut[piece_init_bit] & ~opponent_status) | (bpawn_cap_lut[piece_init_bit] & opponent_status);
                break;
            case BISHOP:
                landing_squares |= bishop_landings(piece_init_bit, own_status, opponent_status);
                break;
            case ROOK:
                landing_squares |= rook_landings(piece_init_bit,  own_status, opponent_status);
                break;
        default:
            break;
    }
    return landing_squares;
}

void ChessBoard::append_moves(Piece p, Moves& move_collector, uint64_t own_status, uint64_t opponent_status){
    uint64_t all_moves;
    auto piece_positions = board[p];
    // loop through all pieces of type p
    while(piece_positions){
        auto piece_init_bit = pop_LSB(piece_positions);
        switch (p.piece_type) {
            case QUEEN:
                all_moves = queen_landings(piece_init_bit, own_status, opponent_status) & ~own_status;
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
                all_moves = bishop_landings(piece_init_bit, own_status, opponent_status) & ~own_status;
                break;
            case ROOK:
                all_moves = rook_landings(piece_init_bit, own_status, opponent_status) & ~own_status;
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

void ChessBoard::update_landing_squares(){
    board_landingsquares[WHITE] = 0ULL;
    board_landingsquares[BLACK] = 0ULL;
    board_landingsquares_throughKing[WHITE] = 0ULL;
    board_landingsquares_throughKing[BLACK] = 0ULL;

    int piece_init_bit;
    uint64_t piece_positions;
    for (Piece KindOfPiece : AllKindOfPieces){            // loop over all kindofpieces
        // if (KindOfPiece.piece_type == KING) continue;   // skip if king
        piece_positions = board[KindOfPiece];
        while(piece_positions){                         // loop over each piece of that type
            piece_init_bit = pop_LSB(piece_positions);
            board_landingsquares[KindOfPiece.color] |= get_landing_squares(KindOfPiece, piece_init_bit);   
            board_landingsquares_throughKing[KindOfPiece.color] |= get_landing_squares(KindOfPiece, piece_init_bit, true);   
            // auto temp = get_landing_squares(KindOfPiece, piece_init_bit);
            // while(temp) {
            //     auto final_bit_value = pop_LSB(temp);
            //     std::cout << "The " << convert_color[KindOfPiece.color] << " "
            //           << convert_enum[KindOfPiece.piece_type]
            //           << " lands from " << bit2notation(piece_init_bit)
            //           << " to " << bit2notation(final_bit_value)<<std::endl;
            // } 
        }
    }
}


Moves ChessBoard::calculate_moves(Color color){
    Moves move_collector;
    if (color == WHITE){
        append_moves(whitePawn, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        append_moves(whiteKnight, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        append_moves(whiteBishop, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        append_moves(whiteRook, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        append_moves(whiteQueen, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        append_moves(whiteKing, move_collector, board_occupancy[WHITE], board_occupancy[BLACK]);
        }
    else if (color == BLACK){
        append_moves(blackPawn, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
        append_moves(blackKnight, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
        append_moves(blackBishop, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
        append_moves(blackRook, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
        append_moves(blackQueen, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
        append_moves(blackKing, move_collector, board_occupancy[BLACK], board_occupancy[WHITE]);
    }
    return move_collector;
}


bool ChessBoard::isCheck(){
    if (board_turn == WHITE){
        if ((board[whiteKing] & board_landingsquares[BLACK])!=0)
            return true;
    }
    if (board_turn == BLACK){
        if ((board[blackKing] & board_landingsquares[WHITE])!=0)
            return true;
    }
    return false;
}
bool ChessBoard::isMate(){
    if (!isCheck())  return false;
    else{
        uint64_t king_board;
        uint64_t evading_squares;
        bool isMate = false;
        if (board_turn == WHITE){
            king_board = board[whiteKing];
            // legal evading squares are not occupied by own pieces, not in check, reachable by the king
            evading_squares = get_landing_squares(whiteKing, pop_LSB(king_board))  & ~board_landingsquares_throughKing[BLACK] & ~board_occupancy[WHITE];
            if ( !evading_squares )
                return true;
        }
        if (board_turn == BLACK){
            king_board = board[blackKing];
            // legal evading squares are not occupied by own pieces, not in check, reachable by the king
            evading_squares = get_landing_squares(blackKing, pop_LSB(king_board))  & ~board_landingsquares_throughKing[WHITE] & ~board_occupancy[BLACK];
            if ( !evading_squares )
                return true;
        }
    }
    return false;
}


// bool ChessBoard::isStaleMate(){
//     if (isCheck())  return false;
//     else{
//         uint64_t king_board;
//         uint64_t evading_squares;
//         bool isMate = false;
//         if (board_turn == WHITE){
//             king_board = board[whiteKing];
//             // legal evading squares are not occupied by own pieces, not in check, reachable by the king
//             legalKing_squares = get_landing_squares(whiteKing, pop_LSB(king_board))  & ~board_landingsquares_throughKing[BLACK] & ~board_occupancy[WHITE];
//             legalOtherPieces_squares = get_landing_squares(whiteKing, pop_LSB(king_board))
//             if ( !evading_squares )
//                 return true;
//         }
//         if (board_turn == BLACK){
//             king_board = board[blackKing];
//             // legal evading squares are not occupied by own pieces, not in check, reachable by the king
//             evading_squares = get_landing_squares(blackKing, pop_LSB(king_board))  & ~board_landingsquares_throughKing[WHITE] & ~board_occupancy[BLACK];
//             if ( !evading_squares )
//                 return true;
//         }
//     }
//     return false;
// }