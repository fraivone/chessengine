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
        _initializeCommon();
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

    _initializeCommon();
    
}

ChessBoard::~ChessBoard() = default; 


void ChessBoard::_initializeCommon(){
    _fill_board();
    _legalPosition(); // check if starting position is valid, 2 kings, Kings aren't in mutual check, pieces don't overlap
    _load_luts();   // load the look up tables for landing squares of leaping pieces

    _update_board_occupancy(); // all positions occupied by black/white
    _update_landing_squares(); // all possible landing squares for black/white
    _update_game_status();
    
}

void ChessBoard::_fill_board(){
    _board ={
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

void ChessBoard::_load_luts(){
    knight_lut = knight_position_lut();
    wpawn_fw_lut = wpawn_straight_lut();
    wpawn_cap_lut = wpawn_diagcapture_lut();
    bpawn_fw_lut = bpawn_straight_lut();
    bpawn_cap_lut = bpawn_diagcapture_lut();
    king_lut =  king_position_lut();
}


void ChessBoard::_ResetLandingSquares(){
    pieces_landingsquares[BLACK] = 0ULL;
    pieces_landingsquares[WHITE] = 0ULL;
    king_landingsquares[BLACK] = 0ULL;
    king_landingsquares[WHITE] = 0ULL;
    pieces_landingsquares_throughKing[BLACK] = 0ULL;
    pieces_landingsquares_throughKing[WHITE] = 0ULL;
}
void ChessBoard::_ResetOccupancySquares(){
    board_occupancy[BLACK] = 0ULL;
    board_occupancy[WHITE] = 0ULL;
    board_occupancy_noKing[BLACK] = 0ULL;
    board_occupancy_noKing[WHITE] = 0ULL;
}

float ChessBoard::evalPosition(){
    float white_score = 0.;
    float black_score = 0.;
    if (isMate)
        return Mate_Score;
    if (isDraw)
        return 0;

    for (const auto& pair : _board) {
        Piece this_piece = pair.first;
        uint64_t value = pair.second;
        int number_of_pieces = countBitsOn(value);
        
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
    for (const auto& pair : _board) {
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


void ChessBoard::RepresentBitset(uint64_t number){
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

bool ChessBoard::_legalPosition(){
    _islegal = true;

    // exactly one king each
    if ( countBitsOn(_board[blackKing]) != 1 or countBitsOn(_board[whiteKing]) != 1){
        _islegal = false;
        // std::cout<<"Too many/few kings on the board.Illegal position\nExiting..."<<std::endl;
        return false;
    }
    int bking_bit = get_LSB(_board[blackKing]);
    int wking_bit = get_LSB(_board[whiteKing]);
    auto _x = king_lut[bking_bit] & _board[whiteKing]; //white king is "seen" by the black king
    auto _y = king_lut[wking_bit] & _board[blackKing]; //black king is "seen" by the white king
    if( (_x !=0) | (_y !=0)  ){
        _islegal = false;
        // std::cout<<"Kings are checking each other.Illegal position.\nExiting..."<<std::endl;
    }

    // pieces don't overlap
    for (const auto& first_pair : _board) {
        if (_islegal == false) break;
        Piece firstkey = first_pair.first;
        uint64_t firstvalue = first_pair.second;
        for (const auto& second_pair : _board) {
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


void ChessBoard::_update_board_occupancy(){
    _ResetOccupancySquares();
    board_occupancy[BLACK] = _board[blackPawn] | _board[blackBishop] | _board[blackKnight] | _board[blackRook] | _board[blackQueen] | _board[blackKing];
    board_occupancy[WHITE] = _board[whitePawn] | _board[whiteBishop] | _board[whiteKnight] | _board[whiteRook] | _board[whiteQueen] | _board[whiteKing];
    
    board_occupancy_noKing[BLACK] = _board[blackPawn] | _board[blackBishop] | _board[blackKnight] | _board[blackRook] | _board[blackQueen];
    board_occupancy_noKing[WHITE] = _board[whitePawn] | _board[whiteBishop] | _board[whiteKnight] | _board[whiteRook] | _board[whiteQueen];
}

/*
Thus far 2 cases in which I care for possible landing squares
1. Calculate available moves
2. Calculate which squares are attacked so that check / mate / stalemate calculation is possible
point 2 is tricky for sliding pieces: the piece can't go through the king
yet it might attack the squares behind the king. Therefore I have added 
the bool "attacking_squares". When ON the opponent king does not occupy the board 
so that sliding pieces attack all visible squares.
Additionaly pawns attack diagonal squares
*/


uint64_t ChessBoard::_get_landing_squares(Piece p, int piece_init_bit, bool attacking_squares = false){
    uint64_t own_status,opponent_status;
    own_status = p.color == WHITE ? board_occupancy[WHITE] : board_occupancy[BLACK];
    if (!attacking_squares){
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
                if(!attacking_squares){
                    // normal move or capturing
                    // include forward movement if not opposed
                    // diagonal captures if opponent exists
                    // allow enpassant using enpassant info
                    
                    landing_squares |= p.color ? (wpawn_fw_lut[piece_init_bit] & ~opponent_status) | (wpawn_cap_lut[piece_init_bit] & opponent_status) : (bpawn_fw_lut[piece_init_bit] & ~opponent_status) | (bpawn_cap_lut[piece_init_bit] & opponent_status);
                    if (p.color == board_turn) // enable enpassant current turn color
                        landing_squares |= p.color ? ( wpawn_cap_lut[piece_init_bit] & uint64_t (pow(2,en_passant_bit)) ) : ( bpawn_cap_lut[piece_init_bit] & uint64_t (pow(2,en_passant_bit)) );
                }
                else{
                    // pawn attacks only diagonally 
                    landing_squares |= p.color ? (wpawn_cap_lut[piece_init_bit] ) : (bpawn_cap_lut[piece_init_bit] );
                }
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
    auto piece_positions = _board[p];
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
                if ( bool(get_bit(_board[blackKing],piece_end_bit)) ){ // white piece checks black king
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
                if( bool(get_bit(_board[whiteKing],piece_end_bit)) ){ // white piece checks black king
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

void ChessBoard::_update_landing_squares(){
    _ResetLandingSquares();

    int piece_init_bit;
    uint64_t piece_positions;
    for (Piece KindOfPiece : AllKindOfPieces){            // loop over all kindofpieces
        // if (KindOfPiece.piece_type == KING) continue;   // skip if king
        piece_positions = _board[KindOfPiece];
        while(piece_positions){                         // loop over each piece of that type
            piece_init_bit = pop_LSB(piece_positions);
            if (KindOfPiece.piece_type != KING){
                pieces_landingsquares[KindOfPiece.color] |= _get_landing_squares(KindOfPiece, piece_init_bit);   
                pieces_landingsquares_throughKing[KindOfPiece.color] |= _get_landing_squares(KindOfPiece, piece_init_bit, true);   
            }
            else
                king_landingsquares[KindOfPiece.color] |= _get_landing_squares(KindOfPiece, piece_init_bit);

            // auto temp = _get_landing_squares(KindOfPiece, piece_init_bit);
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
    return move_collector;
}


void ChessBoard::printStatusInfo(){
    
    std::cout<<"GameOver = "<<GameOver
                 <<"\nMate = "<<isMate
                 <<"\nDraw = "<<isDraw
                 <<"\nStaleMate = "<<isStaleMate
                 <<"\nFifty move rule = "<<isDraw50Moves
                 <<"\nMove repetition = "<<isDrawRepetition
                 <<"\nInsufficient Material = "<<isDrawInsufficientMaterial<<std::endl;
    
}


void ChessBoard::_update_game_status(){
    isCheck = _isCheck();
    isMate = _isMate();
    isStaleMate = _isStaleMate();
    isDraw50Moves = _isDrawFor50MovesRule();
    isDrawRepetition = _isDrawForRepetition();
    isDrawInsufficientMaterial = _isDrawInsufficientMaterial();

    if (isStaleMate | isDraw50Moves | isDrawRepetition | isDrawInsufficientMaterial){
        isDraw = true;
        GameOver = true;
        if (debug)
            std::cout<<"Game Over. Draw "<<std::endl;
    }
    if (isMate){
        GameOver = true;
        if (debug)
            std::cout<<"Game Over. "<<convert_color[!board_turn]<<" wins by checkmate"<<std::endl;
    }
}


bool ChessBoard::_isCheck(){
    if (board_turn == WHITE){
        if ((_board[whiteKing] & pieces_landingsquares[BLACK])!=0)
            return true;
    }
    if (board_turn == BLACK){
        if ((_board[blackKing] & pieces_landingsquares[WHITE])!=0)
            return true;
    }
    return false;
}
bool ChessBoard::_isMate(){
    if (!isCheck)  return false;
    else{
        uint64_t evading_squares;
        bool _isMate = false;
        if (board_turn == WHITE){
            // legal evading squares are not occupied by own pieces, not in check, reachable by the king
            evading_squares = king_landingsquares[WHITE]  & ~(pieces_landingsquares_throughKing[BLACK] | king_landingsquares[BLACK]) & ~board_occupancy[WHITE];
            if ( !evading_squares )
                return true;
        }
        if (board_turn == BLACK){
            // legal evading squares are not occupied by own pieces, not in check, reachable by the king
            evading_squares = king_landingsquares[BLACK]  & ~(pieces_landingsquares_throughKing[WHITE] | king_landingsquares[WHITE])& ~board_occupancy[BLACK];
            if ( !evading_squares )
                return true;
        }
    }
    return false;
}

bool ChessBoard::_isStaleMate(){
    if (isCheck)  return false;
    else{
        uint64_t legal_king_moves;
        uint64_t legal_pieces_moves;
        bool _isMate = false;
        if (board_turn == WHITE){
            legal_king_moves = king_landingsquares[WHITE]  & ~(pieces_landingsquares_throughKing[BLACK] | king_landingsquares[BLACK]) & ~board_occupancy[WHITE];
            legal_pieces_moves = pieces_landingsquares[WHITE] & ~board_occupancy[WHITE];
            if ( !(legal_king_moves) )
                return true;
        }
        if (board_turn == BLACK){
            legal_king_moves = king_landingsquares[BLACK]  & ~(pieces_landingsquares_throughKing[WHITE] | king_landingsquares[WHITE]) & ~board_occupancy[BLACK];
            legal_pieces_moves = pieces_landingsquares[BLACK] & ~board_occupancy[BLACK];
            if ( !(legal_king_moves | legal_pieces_moves) )
                return true;
        }
    }
    return false;
}


bool ChessBoard::_isDrawInsufficientMaterial(){
    // from https://en.wikipedia.org/wiki/Draw_(chess)
    int pawns_left = countBitsOn(_board[whitePawn]) + countBitsOn(_board[blackPawn]);
    int rooks_left = countBitsOn(_board[whiteRook]) + countBitsOn(_board[blackRook]);
    int queens_left = countBitsOn(_board[whiteQueen]) + countBitsOn(_board[blackQueen]);
    if (pawns_left != 0 | rooks_left != 0 | queens_left != 0) //A king + any(pawn, rook, queen) is not draw
        return false;
    
    int wbishops_left = countBitsOn(_board[whiteBishop]);
    int bbishops_left = countBitsOn(_board[blackBishop]);
    int wknights_left = countBitsOn(_board[whiteKnight]);
    int bknights_left = countBitsOn(_board[blackKnight]);
    
    if (wbishops_left + bbishops_left + wknights_left + bknights_left == 0) // king vs king
        return true;

    if ( wbishops_left == 1 & (bbishops_left  + wknights_left + bknights_left) == 0) // king and bishop versus king
        return true;
    if ( bbishops_left == 1 & (wbishops_left  + wknights_left + bknights_left) == 0) // king and bishop versus king
        return true;
    if ( wknights_left == 1 & (bbishops_left  + wbishops_left + bknights_left) == 0) // king and knight versus king
        return true;
    if ( bknights_left == 1 & (wbishops_left  + wknights_left + bbishops_left) == 0) // king and knight versus king
        return true;

    // TODO king and bishop versus king and bishop with the bishops on the same color.

    return false;

}
bool ChessBoard::_isDrawForRepetition(){
    return false; // to be implemented
}
bool ChessBoard::_isDrawFor50MovesRule(){
    return false; // to be implemented
}