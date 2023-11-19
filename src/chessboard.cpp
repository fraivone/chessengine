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
    
    // std::cout<<"Initialization\n";
    // _updateAfterMove();   
    isCheck = _ChainAssertCheck(_board, 
                                board_occupancy, 
                                board_occupancy_noKing, 
                                pieces_landingsquares, 
                                pieces_landingsquares_throughKing,
                                king_landingsquares,
                                PM_collector,
                                board_turn);
    _update_game_status(); 
    
}
// void ChessBoard::_updateAfterMove(){

//     _update_board_occupancy(board_occupancy, _board); // all positions occupied by black/white
//     _update_board_occupancy(board_occupancy_noKing, _board); // all positions occupied by black/white
//     // _update_landing_squares(); // all possible landing squares for black/white
//     _update_landing_squares(pieces_landingsquares, pieces_landingsquares_throughKing, king_landingsquares, PM_collector,_board);// all possible landing squares for black/white
//     isCheck = _isCheck(board_turn, pieces_landingsquares[!board_turn]); // isCheck?


    

//     // RepresentBitset(king_landingsquares[WHITE]);
//     // RepresentBitset(pieces_landingsquares_throughKing[BLACK] & pieces_landingsquares[WHITE]);
    
// }

bool ChessBoard::_ChainAssertCheck(Board theBoard, 
                                   uint64_t (&bOcc)[2], 
                                   uint64_t (&bOcc_noKing)[2], 
                                   uint64_t (&landing_squares)[2],
                                   uint64_t (&landing_squares_throughKing)[2], 
                                   uint64_t (&king_landing_squares)[2], 
                                   Moves (&ps)[2][nPieceTypes],
                                   Color theColor)
{   
    _update_board_occupancy(bOcc, theBoard);
    _update_board_occupancy(bOcc_noKing, theBoard);
    _update_landing_squares(landing_squares, landing_squares_throughKing, king_landing_squares, ps,theBoard);
    bool _temp_check = _isCheck(theColor, landing_squares[!theColor]); // isCheck?
    return _temp_check;

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
    wpawn_doublefw_lut = wpawn_doublestraight_lut();
    wpawn_cap_lut = wpawn_diagcapture_lut();
    bpawn_fw_lut = bpawn_straight_lut();
    bpawn_doublefw_lut = bpawn_doublestraight_lut();
    bpawn_cap_lut = bpawn_diagcapture_lut();
    king_lut =  king_position_lut();
}


void ChessBoard::_ResetLandingSquares(uint64_t (&t)[2]){
    t[BLACK] = 0ULL;
    t[WHITE] = 0ULL;
}
void ChessBoard::_ResetPseudoMoves(Moves (&array)[2][nPieceTypes]){
    // Clear the vector and reduce its capacity
    for(int color = 0; color <2; color++){
        for (Piece pp : pieces_array){
            array[color][pp.piece_type].clear();
            array[color][pp.piece_type].shrink_to_fit();
            }
    }
}
void ChessBoard::_ResetOccupancySquares(uint64_t (&t)[2]){
    t[BLACK] = 0ULL;
    t[WHITE] = 0ULL;
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


void ChessBoard::_update_board_occupancy(uint64_t (&t)[2], Board theBoard){
    _ResetOccupancySquares(t);

    t[BLACK] = theBoard[blackPawn] | theBoard[blackBishop] | theBoard[blackKnight] | theBoard[blackRook] | theBoard[blackQueen] | theBoard[blackKing];
    t[WHITE] = _board[whitePawn] | theBoard[whiteBishop] | theBoard[whiteKnight] | theBoard[whiteRook] | theBoard[whiteQueen] | theBoard[whiteKing];
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
                    // 2 squares fw if the 2 squares are free
                    landing_squares |= p.color ? (wpawn_fw_lut[piece_init_bit] & ~opponent_status) | (wpawn_doublefw_lut[piece_init_bit] & ~opponent_status & ~(opponent_status << 8) & ~(own_status << 8)) |(wpawn_cap_lut[piece_init_bit] & opponent_status) : (bpawn_fw_lut[piece_init_bit] & ~opponent_status) | (bpawn_doublefw_lut[piece_init_bit] & ~opponent_status & ~(opponent_status >> 8) & ~(own_status >> 8)) | (bpawn_cap_lut[piece_init_bit] & opponent_status);
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

void ChessBoard::_update_landing_squares(uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Moves (&pseudomoves)[2][nPieceTypes],Board current_board){
    _ResetLandingSquares(landing_sq);
    _ResetLandingSquares(landing_sq_throughKing);
    _ResetLandingSquares(king_landing_sq);
    _ResetPseudoMoves(pseudomoves);
    uint64_t temp_landing = 0ULL;
    uint64_t temp_landing_attack = 0ULL;
    uint64_t piece_positions;
    int piece_init_bit;
    int final_bit_value;
    ;

    for (Piece pp : pieces_array){            // loop over all kindofpieces
        piece_positions = current_board[pp];
        while(piece_positions){                         // loop over each piece of that type
            piece_init_bit = pop_LSB(piece_positions);
            temp_landing = _get_landing_squares(pp, piece_init_bit);
            temp_landing_attack = _get_landing_squares(pp, piece_init_bit, true);   
            if (pp.piece_type != KING){
                landing_sq[pp.color] |= temp_landing;
                landing_sq_throughKing[pp.color] |= temp_landing_attack;

                // UPDATE PSEUDOMOVES AS WELL
                // landing squares allow landing on its own pieces
                // because it serves to "protect" a piece from capture
                // not allowed for pseudo moves, so removing it
                temp_landing = temp_landing & ~board_occupancy[pp.color];
                
                // loop through all possible landing moves
                while(temp_landing){
                    final_bit_value = pop_LSB(temp_landing);
                    Move theMove = {piece_init_bit, final_bit_value, pp};
                    pseudomoves[pp.color][pp.piece_type].push_back(theMove);
                }
            } // not king

            else{
                // king can't move to controlled square
                // it works because KINGS are the last in pieces_array
                // so landing_sq_throughKing is fully updated
                temp_landing = temp_landing & ~landing_sq_throughKing[!pp.color];
                king_landing_sq[pp.color] |= temp_landing;
            } // is king
        }
    }
    // Prevents kings to check each other
    piece_positions = king_landing_sq[WHITE];  // reusing variable for temp storage
    king_landing_sq[WHITE] = king_landing_sq[WHITE] & ~king_landing_sq[BLACK];
    king_landing_sq[BLACK] = king_landing_sq[BLACK] & ~piece_positions;
    
    // the KING pseudo moves have not been updated before
    // now it's sure they won't try to check each other
    // UPDATE PSEUDOMOVES AS WELL
    for(int cc = 0; cc < 2; cc++){
        uint64_t copy_king_landing = king_landing_sq[cc];
        int theKingIndex = ColorlessKing|cc;
        Piece theKingPiece = pieces_array[theKingIndex];
        int king_init_bit = get_LSB(current_board[theKingPiece]);
        
        while(copy_king_landing){
                    final_bit_value = pop_LSB(copy_king_landing);
                    Move theMove = {king_init_bit, final_bit_value, theKingPiece};
                    pseudomoves[theKingPiece.color][theKingPiece.piece_type].push_back(theMove);
                }
    } // update pseudo moves

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