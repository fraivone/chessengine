#include <iostream>
# include <bitset>
#include <thread>
#include <chrono>
#include "chessboard.hpp"


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
    _init_board();
    _legalPosition(); // check if starting position is valid, 2 kings, Kings aren't in mutual check, pieces don't overlap
    _load_luts();   // load the look up tables for landing squares of leaping pieces

    std::cout<<"Initialization\n";
    _updateAfterMove();   
    _update_game_status(); 

    // RepresentBitset(king_landingsquares[WHITE]);
    // RepresentBitset(pieces_landingsquares_throughKing[BLACK] & pieces_landingsquares[WHITE]);
    
}
void ChessBoard::_updateAfterMove(){

    _update_board_occupancy(); // all positions occupied by black/white
    _update_landing_squares(); // all possible landing squares for black/white
    isCheck = _isCheck(board_turn, pieces_landingsquares[!board_turn]); // isCheck?


    

    // RepresentBitset(king_landingsquares[WHITE]);
    // RepresentBitset(pieces_landingsquares_throughKing[BLACK] & pieces_landingsquares[WHITE]);
    
}

void ChessBoard::_init_board(){
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
void ChessBoard::_ResetPseudoMoves(){
    // Clear the vector and reduce its capacity
    for(int color = 0; color <2; color++){
        for (Piece pp : pieces_array){
            PM_collector[color][pp.piece_type].clear();
            PM_collector[color][pp.piece_type].shrink_to_fit();
            PM_collector_throughKing[color][pp.piece_type].clear();
            PM_collector_throughKing[color][pp.piece_type].shrink_to_fit();
            }
    }
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
                    grid[row][col] = " " + std::string(1,key.name) +" ";
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

void ChessBoard::_update_landing_squares(){
    _ResetLandingSquares();
    _ResetPseudoMoves();
    uint64_t temp_landing = 0ULL;
    uint64_t temp_landing_attack = 0ULL;
    uint64_t piece_positions;
    int piece_init_bit;
    int final_bit_value;
    ;

    for (Piece pp : pieces_array){            // loop over all kindofpieces
        piece_positions = _board[pp];
        while(piece_positions){                         // loop over each piece of that type
            piece_init_bit = pop_LSB(piece_positions);
            temp_landing = _get_landing_squares(pp, piece_init_bit);
            temp_landing_attack = _get_landing_squares(pp, piece_init_bit, true);   
            if (pp.piece_type != KING){
                pieces_landingsquares[pp.color] |= temp_landing;
                pieces_landingsquares_throughKing[pp.color] |= temp_landing_attack;
            }
            else
                king_landingsquares[pp.color] |= temp_landing;
            
            // can't move on its own piece
            temp_landing = temp_landing & ~board_occupancy[pp.color];
            // king can't move toward controlled square
            // it works because KINGS are the last in pieces_array
            // so pieces_landingsquares_throughKing is fully updated
            if (pp.piece_type == KING)
                temp_landing = temp_landing & ~pieces_landingsquares_throughKing[!pp.color];
            // loop through all possible landing moves
            while(temp_landing){
                final_bit_value = pop_LSB(temp_landing);
                // std::cout<<"Generating a MOVE with "<<piece_init_bit<<" "<<final_bit_value<<" "<< KindOfPiece.name<<std::endl;
                Move theMove = {piece_init_bit, final_bit_value, pp};
                // std::cout<<"Generated a MOVE with "<<theMove.initial_bit<<" "<<theMove.final_bit<<" "<< theMove.piece.name<<std::endl;    
                PM_collector[pp.color][pp.piece_type].push_back(theMove);
            }
            // loop through all possible landing moves
            while(temp_landing_attack){
                final_bit_value = pop_LSB(temp_landing_attack);
                PM_collector_throughKing[pp.color][pp.piece_type].push_back(Move(piece_init_bit, final_bit_value, pp));
            }
        }
    }

    piece_positions = king_landingsquares[WHITE];  // reusing variable for temp storage
    king_landingsquares[WHITE] = king_landingsquares[WHITE] & ~king_landingsquares[BLACK];
    king_landingsquares[BLACK] = king_landingsquares[BLACK] & ~piece_positions;

}


// Moves ChessBoard::calculate_moves(Color color){
//     Moves move_collector;
//     return move_collector;
// }


void ChessBoard::printStatusInfo(){

    std::cout<<convert_color[board_turn]<<" to move"
                 <<"\nGameOver = "<<GameOver
                 <<"\nCheck = "<<isCheck
                 <<"\nMate = "<<isMate
                 <<"\nDraw = "<<isDraw
                 <<"\nStaleMate = "<<isStaleMate
                 <<"\nFifty move rule = "<<isDraw50Moves
                 <<"\nMove repetition = "<<isDrawRepetition
                 <<"\nInsufficient Material = "<<isDrawInsufficientMaterial<<std::endl;
    
}

void ChessBoard::_update_game_status(){
    isMate = _isMate();
    isStaleMate = _isStaleMate();
    isDraw50Moves = _isDrawFor50MovesRule();
    isDrawRepetition = _isDrawForRepetition();
    isDrawInsufficientMaterial = _isDrawInsufficientMaterial();

    if (isStaleMate | isDraw50Moves | isDrawRepetition | isDrawInsufficientMaterial){
        isDraw = true;
        GameOver = true;
    }
    else if (isMate){
        GameOver = true;
    }
    else
        GameOver = false;
}