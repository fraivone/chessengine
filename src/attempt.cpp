#include "attempt.hpp"

bool BoardisLegal(Board theBoard, LUT king_lut){
    bool _islegal = true;

    // exactly one king each
    if ( countBitsOn(theBoard[blackKing]) != 1 or countBitsOn(theBoard[whiteKing]) != 1){
        _islegal = false;
        // std::cout<<"Too many/few kings on the board.Illegal position\nExiting..."<<std::endl;
        return false;
    }
    int bking_bit = get_LSB(theBoard[blackKing]);
    int wking_bit = get_LSB(theBoard[whiteKing]);
    auto _x = king_lut[bking_bit] & theBoard[whiteKing]; //white king is "seen" by the black king
    auto _y = king_lut[wking_bit] & theBoard[blackKing]; //black king is "seen" by the white king
    if( (_x !=0) | (_y !=0)  ){
        _islegal = false;
        // std::cout<<"Kings are checking each other.Illegal position.\nExiting..."<<std::endl;
    }

    // pieces don't overlap
    for (const auto& first_pair : theBoard) {
        if (_islegal == false) break;
        Piece firstkey = first_pair.first;
        uint64_t firstvalue = first_pair.second;
        for (const auto& second_pair : theBoard) {
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


void RepresentBitset(uint64_t number){
    // init the grid
    int nCols = 8;
    int nRows = 8;
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



BoardStatus BoardStatusFromFEN(const std::string& fen){
    BoardStatus theBoardStatus; // the board is a map therefore it is always initialized with 0s.
    const size_t size = fen.size();
    Color whoMoves;
    size_t iter = 0;
    int index = 56;

    // pieces on the board
    for (; (iter < size) and (fen[iter] != ' '); iter++){
        if (fen[iter] == '/'){
            index = index -  16;
            continue;
        }
        if (isdigit(fen[iter]))
            index += (fen[iter] - '0'); // converts char digit to int. `5` to 5
        else{
            // std::cout<<"board["<<index<<"] = "<<fen[iter]<<std::endl;
            Piece p = KindOfPiecesLUT.at(fen[iter]);
            theBoardStatus.board[p] |=  uint64_t(pow(2,index));
            ++index;
        }
    }
    theBoardStatus.whoMoves = fen[iter + 1] == 'w' ? Color::WHITE : Color::BLACK;

    for (iter += 3; (iter < size )and (fen[iter] != ' '); iter++)
    {

        if (fen[iter] == 'k')
            theBoardStatus.castle_rights[Color::BLACK][Castle::king_side] = true;

        else if (fen[iter] == 'K')
            theBoardStatus.castle_rights[Color::WHITE][Castle::king_side] = true;

        else if (fen[iter] == 'q')
            theBoardStatus.castle_rights[Color::BLACK][Castle::queen_side] = true;

        else if (fen[iter] == 'Q')
            theBoardStatus.castle_rights[Color::WHITE][Castle::queen_side] = true;
    }

    if (fen[iter+1] == '-') // no enpassant
        theBoardStatus.enpassant_bit = -1;
    else
        theBoardStatus.enpassant_bit = chars_to_bit(fen[iter + 1], fen[iter + 2]);
    //TODO Implemnt half move and full move for draw for repetition
    
    return theBoardStatus;
}
void Reset_uint64_2Darray(uint64_t (&t)[2]){
    t[BLACK] = 0ULL;
    t[WHITE] = 0ULL;
}
void update_board_occupancy(uint64_t (&t)[2], Board theBoard, bool noking){
    Reset_uint64_2Darray(t);
    t[BLACK] = theBoard[blackPawn] | theBoard[blackBishop] | theBoard[blackKnight] | theBoard[blackRook] | theBoard[blackQueen];
    t[WHITE] = theBoard[whitePawn] | theBoard[whiteBishop] | theBoard[whiteKnight] | theBoard[whiteRook] | theBoard[whiteQueen];
    if (!noking){
        t[BLACK] |= theBoard[blackKing];
        t[WHITE] |= theBoard[whiteKing];
    }
}
void ResetPseudoMoves(Moves (&array)[2][nPieceTypes]){
    // Clear the vector and reduce its capacity
    for(int color = 0; color <2; color++){
        for (Piece pp : pieces_array){
            array[color][pp.piece_type].clear();
            array[color][pp.piece_type].shrink_to_fit();
            }
    }
}

uint64_t get_landing_squares(uint64_t bOcc[2], uint64_t bOcc_noKing[2], Piece p, int piece_init_bit, Color whoMoves = BLACK, int en_passant_bit = -1, bool attacking_squares = false){
    uint64_t own_status,opponent_status;
    own_status = p.color == WHITE ? bOcc[WHITE] : bOcc[BLACK];
    if (!attacking_squares){
        opponent_status = p.color == WHITE ? bOcc[BLACK] : bOcc[WHITE];
    }
    else{
        opponent_status = p.color == WHITE ? bOcc_noKing[BLACK] : bOcc_noKing[WHITE];
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
                    if (p.color == whoMoves) // enable enpassant current turn color
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

// function overloaded : With PSEUDOMOVES
void update_landing_squares(uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Moves (&pseudomoves)[2][nPieceTypes],Board current_board, Color whoMoves, int en_passant_bit){
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
            temp_landing = get_landing_squares(bOcc,bOcc_noKing, pp, piece_init_bit, whoMoves, en_passant_bit, false);
            temp_landing_attack = get_landing_squares(bOcc, bOcc_noKing, pp, piece_init_bit, whoMoves, en_passant_bit , true);   
            if (pp.piece_type != KING){
                landing_sq[pp.color] |= temp_landing;
                landing_sq_throughKing[pp.color] |= temp_landing_attack;

                // UPDATE PSEUDOMOVES AS WELL
                // landing squares allow landing on its own pieces
                // because it serves to "protect" a piece from capture
                // not allowed for pseudo moves, so removing it
                temp_landing = temp_landing & ~bOcc[pp.color];
                
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
    king_landing_sq[WHITE] = king_landing_sq[WHITE] & ~king_lut[get_LSB(current_board[blackKing])];
    king_landing_sq[BLACK] = king_landing_sq[BLACK] & ~king_lut[get_LSB(current_board[whiteKing])];
    
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

// function overloaded : No PSEUDOMOVES
void update_landing_squares(uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_sq)[2],uint64_t (&landing_sq_throughKing)[2],uint64_t (&king_landing_sq)[2],Board current_board, Color whoMoves, int en_passant_bit){
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
            temp_landing = get_landing_squares(bOcc,bOcc_noKing, pp, piece_init_bit, whoMoves, en_passant_bit, false);
            temp_landing_attack = get_landing_squares(bOcc, bOcc_noKing, pp, piece_init_bit, whoMoves, en_passant_bit , true);   
            if (pp.piece_type != KING){
                landing_sq[pp.color] |= temp_landing;
                landing_sq_throughKing[pp.color] |= temp_landing_attack;
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
    king_landing_sq[WHITE] = king_landing_sq[WHITE] & ~king_lut[get_LSB(current_board[blackKing])];
    king_landing_sq[BLACK] = king_landing_sq[BLACK] & ~king_lut[get_LSB(current_board[whiteKing])];

}



bool isCheck(Board theBoard, Color c, uint64_t opponent_landings){
    Piece theMovingKing = pieces_array[ColorlessKing|c];
    if ((theBoard[theMovingKing] & opponent_landings)!=0)
            return true;
    return false;
}

// function overloaded TrackBitBoard
bool ChainAssertCheck(Board theBoard,uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_squares)[2], uint64_t (&landing_squares_throughKing)[2], uint64_t (&king_landing_squares)[2],Color theColor,int en_passant_bit){   
    Reset_uint64_2Darray(bOcc);
    Reset_uint64_2Darray(bOcc_noKing);
    Reset_uint64_2Darray(landing_squares);
    Reset_uint64_2Darray(landing_squares_throughKing);
    Reset_uint64_2Darray(king_landing_squares);
    update_board_occupancy(bOcc, theBoard, false);
    update_board_occupancy(bOcc_noKing, theBoard, true);
    // ResetPseudoMoves(ps);
    update_landing_squares(bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, theBoard, theColor, en_passant_bit);
    bool _temp_check = isCheck(theBoard, theColor, landing_squares[!theColor]); // isCheck?
    return _temp_check;
}
// function overloaded DontTrack
bool ChainAssertCheck(Board theBoard,Color theColor,int en_passant_bit){   
    // declare and initialized to 0
    uint64_t bOcc[2] = {};
    uint64_t bOcc_noKing[2] = {};
    uint64_t landing_squares[2] = {};
    uint64_t landing_squares_throughKing[2] = {};
    uint64_t king_landing_squares[2] = {};
    update_board_occupancy(bOcc, theBoard, false);
    update_board_occupancy(bOcc_noKing, theBoard, true);
    update_landing_squares(bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, theBoard, theColor, en_passant_bit);
    bool _temp_check = isCheck(theBoard, theColor, landing_squares[!theColor]); // isCheck?
    return _temp_check;
}
// function overloaded TrackAll
bool ChainAssertCheck(Board theBoard,uint64_t (&bOcc)[2], uint64_t (&bOcc_noKing)[2], uint64_t (&landing_squares)[2], uint64_t (&landing_squares_throughKing)[2], uint64_t (&king_landing_squares)[2], Moves (&ps)[2][nPieceTypes],Color theColor,int en_passant_bit){   
    Reset_uint64_2Darray(bOcc);
    Reset_uint64_2Darray(bOcc_noKing);
    Reset_uint64_2Darray(landing_squares);
    Reset_uint64_2Darray(landing_squares_throughKing);
    Reset_uint64_2Darray(king_landing_squares);
    update_board_occupancy(bOcc, theBoard, false);
    update_board_occupancy(bOcc_noKing, theBoard, true);
    ResetPseudoMoves(ps);
    update_landing_squares(bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, ps,theBoard, theColor, en_passant_bit);    
    bool _temp_check = isCheck(theBoard, theColor, landing_squares[!theColor]); // isCheck?
    return _temp_check;
}
// function overloaded TrackPseudoMoves
bool ChainAssertCheck(Board theBoard,Moves (&ps)[2][nPieceTypes],Color theColor,int en_passant_bit){   
    // declare and initialized to 0
    uint64_t bOcc[2] = {};
    uint64_t bOcc_noKing[2] = {};
    uint64_t landing_squares[2] = {};
    uint64_t landing_squares_throughKing[2] = {};
    uint64_t king_landing_squares[2] = {};
    // Reset_uint64_2Darray(bOcc);
    // Reset_uint64_2Darray(bOcc_noKing);
    // Reset_uint64_2Darray(landing_squares);
    // Reset_uint64_2Darray(landing_squares_throughKing);
    // Reset_uint64_2Darray(king_landing_squares);
    update_board_occupancy(bOcc, theBoard, false);
    update_board_occupancy(bOcc_noKing, theBoard, true);
    ResetPseudoMoves(ps);
    update_landing_squares(bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, ps,theBoard, theColor, en_passant_bit);
    bool _temp_check = isCheck(theBoard, theColor, landing_squares[!theColor]); // isCheck?
    return _temp_check;
}


// to be fixed
bool isMate(BoardStatus theBoardStatus, Moves (&ps)[2][nPieceTypes], uint64_t (&bOcc)[2], uint64_t (&king_landing_squares)[2], uint64_t (&landing_squares_throughKing)[2]){
    Moves lmvs;
    Color theColor = theBoardStatus.whoMoves;
    if (theBoardStatus.check == 0)  return false;
    else{
        uint64_t evading_squares;
        bool _isMate = false;
        // legal evading squares are not occupied by own pieces, not in check, reachable by the king
        evading_squares = king_landing_squares[theColor]  & ~(landing_squares_throughKing[!theColor] | king_landing_squares[!theColor]) & ~bOcc[theColor];
        // RepresentBitset(pieces_landingsquares_throughKing[!theColor]);
        if ( evading_squares == 0 ){
            // check for blocking moves
            lmvs = getLegalMoves(theBoardStatus, ps, bOcc);
            // if no legal moves available --> mate
            if (lmvs.size() == 0)
                return true;
            }
        }
    return false;
}

bool ChainAssertDraw(BoardStatus theBoardStatus){
    if (theBoardStatus.check == 0){  // no check
        uint64_t king_landingsquares[2] = {};
        uint64_t bOcc[2] = {};
        uint64_t bOcc_noKing[2] = {};
        uint64_t landing_squares[2] = {};
        uint64_t landing_squares_throughKing[2] = {};
        uint64_t king_landing_squares[2] = {};
        update_board_occupancy(bOcc, theBoardStatus.board, false);
        update_board_occupancy(bOcc_noKing, theBoardStatus.board, true);
        update_landing_squares(bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing,     king_landing_squares,theBoardStatus.board, theBoardStatus.whoMoves, theBoardStatus.enpassant_bit);

        theBoardStatus.draw = theBoardStatus.draw == -1 ? isStaleMate(theBoardStatus, bOcc, king_landingsquares,landing_squares_throughKing, landing_squares) : theBoardStatus.draw | isStaleMate(theBoardStatus, bOcc, king_landingsquares,landing_squares_throughKing, landing_squares);
        theBoardStatus.draw = theBoardStatus.draw == -1 ? isDrawInsufficientMaterial(theBoardStatus.board) : theBoardStatus.draw | isDrawInsufficientMaterial(theBoardStatus.board);
        return theBoardStatus.draw;

    }
    return false;
}
bool isStaleMate(BoardStatus theBoardStatus, uint64_t* bOcc, uint64_t* king_landingsquares,uint64_t* landing_squares_throughKing, uint64_t* landing_squares){
    if (theBoardStatus.check == 0){  // no check
        uint64_t legal_king_moves;
        uint64_t legal_pieces_moves;
        legal_king_moves = king_landingsquares[theBoardStatus.whoMoves]  & ~(landing_squares_throughKing    [!theBoardStatus.whoMoves] | king_landingsquares[!theBoardStatus.whoMoves]) & ~bOcc[theBoardStatus. whoMoves];
        legal_pieces_moves = landing_squares[theBoardStatus.whoMoves] & ~bOcc[!theBoardStatus.whoMoves];
        if ( !(legal_king_moves) & !legal_pieces_moves )
            return true;
    }
    return false;
}

bool isDrawInsufficientMaterial(Board theBoard){
    // from https://en.wikipedia.org/wiki/Draw_(chess)
    int pawns_left = countBitsOn(theBoard[whitePawn]) + countBitsOn(theBoard[blackPawn]);
    int rooks_left = countBitsOn(theBoard[whiteRook]) + countBitsOn(theBoard[blackRook]);
    int queens_left = countBitsOn(theBoard[whiteQueen]) + countBitsOn(theBoard[blackQueen]);
    if (pawns_left != 0 | rooks_left != 0 | queens_left != 0) //A king + any(pawn, rook, queen) is not draw
        return false;
    
    int wbishops_left = countBitsOn(theBoard[whiteBishop]);
    int bbishops_left = countBitsOn(theBoard[blackBishop]);
    int wknights_left = countBitsOn(theBoard[whiteKnight]);
    int bknights_left = countBitsOn(theBoard[blackKnight]);
    
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
bool isDrawForRepetition(){
    return false; // to be implemented
}
bool isDrawFor50MovesRule(){
    return false; // to be implemented
}

bool ValidateMove(Board theBoard, Move &m, uint64_t* bOcc, int enpassant_bit){
    if (m.isChecked == true)
        return m.isValid;
    
    
    else{
        m.isChecked = true;
        Piece theOpponentKing = pieces_array[ColorlessKing|!m.piece.color];
        //The move would capture the opponent king
        //Illegal and most likely previous moves were illegal too
        if (( theBoard[theOpponentKing] & (1ULL << m.final_bit)) ){
            // printMove(m);
            // std::cout<<"Invalid because it captures a king"<<std::endl;
            return false;
        }

        // move ends on its onw pieces
        if ( (1ULL << m.final_bit) & (bOcc[m.piece.color])){
            // std::cout<<"Invalid because it ends on its onw pieces"<<std::endl;
            return false;
        }

        //check whether the move leaves its own king in check        
        
        Board nextMove_board = theBoard;
        nextMove_board = MakeMove(m,nextMove_board);
        // printBoard(nextMove_board);
        // ChainAssertCheck(Board theBoard,Moves (&ps)[2][nPieceTypes],Color theColor,int en_passant_bit)
        bool nextMove_isCheck = ChainAssertCheck(nextMove_board, m.piece.color, enpassant_bit);
        // despite having just moved, m.piece.color is still in check
        // thereofore this move isn't legal
        if (nextMove_isCheck){
            // printMove(m);
            // std::cout<<"Invalid because still in check"<<std::endl;
            return false;
    }

    // TODO
    //if (piece is pinned check with ignore square?) 
    m.isValid = true;
    return true;
    }
}

// Move goes on the next_board and next_board is returned
// board_turn stays untouched 
Board MakeMove(Move &m, Board theBoard){
    Color thisColorMoves = m.piece.color;
    
    // the move captures something?
    // loop over the opponent pieces
    // check if the move captures an opponent piece
    // if so, update the board and the move
    for (Piece pp : pieces_array){
        if (pp.color != !thisColorMoves) continue; 
        if (pp.piece_type == KING) continue; // shouldn't happen if the move is properly validated valid
        
        if( get_bit(theBoard[pp], m.final_bit)){
            theBoard[pp] = clear_bit(theBoard[pp],m.final_bit);
            m.captured_piece = pp;
            break;
        }
    }
    theBoard[m.piece] = clear_bit( theBoard[m.piece], m.initial_bit); // remove thiscolor piece
    theBoard[m.piece] = set_bit(theBoard[m.piece],m.final_bit);

    return theBoard;
}

// Board ChessBoard::PublicMakeMove(Move &m, Board theBoard){
//     return _MakeMove(m, theBoard);
// }

// Board ChessBoard::_UndoMove(Move &m, Board theBoard){
//     if( m.captured_piece.piece_type != NOTHING){
//         theBoard[m.captured_piece] = set_bit(theBoard[m.captured_piece],m.final_bit);
//         m.captured_piece = nullPiece;
//     }
//     theBoard[m.piece] = clear_bit(theBoard[m.piece],m.final_bit);
//     theBoard[m.piece] = set_bit(theBoard[m.piece],m.initial_bit);

//     return theBoard;
// }

// legalizes moves, discard the illegals
Moves getLegalMoves(BoardStatus theBoardStatus, Moves (&ps)[2][nPieceTypes],uint64_t* bOcc){
    Moves local_legalMoves;
    BoardStatus temp_copy;
    Color color = theBoardStatus.whoMoves;
    for (PieceType pt : piecetypes_array){
        // Loop to pop all color moves
        int size = ps[color][pt].size();
        int t = 0;
        while (t<size) {
            Move mv = ps[color][pt][t];//.back();
            // ps[color][pt].pop_back();
            if(  ValidateMove(theBoardStatus.board, mv, bOcc, theBoardStatus.enpassant_bit) ){
                temp_copy = theBoardStatus;
                temp_copy.board = MakeMove(mv, temp_copy.board);
                mv.guessedScore = guessMoveScore(temp_copy, mv);
                local_legalMoves.push_back(mv);
            }
            t++;
        }
        
        
    }
    //append to vector of legal moves
    // LegalMoves[color].insert(LegalMoves[color].end(), local_legalMoves.begin(), local_legalMoves.end());
    // return LegalMoves[color];
    return local_legalMoves;
}


float evalBoard(BoardStatus theBoardStatus){
    float white_score = 0.;
    float black_score = 0.;
    if (theBoardStatus.mate == 1)
        return Mate_Score * (ColorMultiplier(!theBoardStatus.whoMoves)); // the other player scored a mate
    if (theBoardStatus.draw == 1)
        return 0;

    for (const auto& pair : theBoardStatus.board) {
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


// Given move and the board status that this move will generate
// guess value for this move
float guessMoveScore(BoardStatus theFutureStatus, Move mv){
    float material_score = evalBoard(theFutureStatus);
    if (mv.captured_piece.piece_type != NOTHING)
        material_score +=  10*(mv.captured_piece.piece_value)*ColorMultiplier(mv.captured_piece.color) - (mv.piece.piece_value)*(ColorMultiplier(mv.piece.color));
    // check gives 3 guessed points
    if (ChainAssertCheck(theFutureStatus.board,!(mv.piece.color),-1))
        material_score += 3*ColorMultiplier(mv.piece.color);

    return material_score;
}

bool bestWhiteByscore(const Move &a, const Move &b)
{
    return a.guessedScore > b.guessedScore;
}
bool bestBlackByscore(const Move &a, const Move &b)
{
    return a.guessedScore < b.guessedScore;
}

// void sortBestMoves(Moves moves, Color theColor){
//     if (theColor == WHITE)
//         std::sort(moves.begin(), moves.end(), compareByscore);
//     else
//         std::sort(moves.rbegin(), moves.rend(), compareByscore);
// }


ScorenMove minimax(Board theBoard, Color theColor,float alpha, float beta, int depth, int& moveCounter){
    moveCounter++;
    uint64_t bOcc[2] = {};
    uint64_t bOcc_noKing[2] = {};
    uint64_t landing_squares[2] = {};
    uint64_t landing_squares_throughKing[2] = {};
    uint64_t king_landing_squares[2] = {};
    Moves PM_collector[2][nPieceTypes];
    
    BoardStatus bs;
    bs.check = ChainAssertCheck(theBoard,bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, PM_collector,theColor,-1);
    bs.board = theBoard;
    bs.whoMoves = theColor;


    bs.mate = isMate(bs, PM_collector, bOcc, king_landing_squares, landing_squares_throughKing);
    bs.draw = ChainAssertDraw(bs);
    Moves legals = getLegalMoves(bs, PM_collector, bOcc);
    // sort move to get the best first
    if (theColor == WHITE)
        std::sort(legals.begin(), legals.end(), bestWhiteByscore);
    else
        std::sort(legals.begin(), legals.end(), bestBlackByscore);
    
    float best_score,new_score;
    Move best_move = NULL_MOVE;

    // exiting
    if (depth==0 | bs.mate | bs.draw ){
        // if (depth!=0 )
        // std::cout<<"movecounter at exit "<<moveCounter<<"\tdepth "<<depth<<std::endl;;
            // printMove(mv);
        return std::make_pair(evalBoard(bs), best_move);
    }
    
    if (theColor == WHITE){
        best_score = MINUS_INF;
        for (Move mv : legals){
            // std::cout<<"depth = "<<depth<<"\t"<<convert_color[mv.piece.color]<<" \t GuessedScore "<<mv.guessedScore<<"\t";
            // printMove(mv);
            Board theNewBoard = bs.board;
            theNewBoard = MakeMove(mv,theNewBoard);
            new_score = minimax(theNewBoard, (!theColor),alpha,beta, depth - 1, moveCounter).first;
            if ( new_score >best_score){
                best_score = new_score;
                best_move = mv;
            }
            if (best_score > beta)
                break;
            alpha = std::max(alpha, best_score);
        }
        return std::make_pair(best_score, best_move);;
    }
    else{
        best_score = PLUS_INF;
        for (Move mv : legals){
            // std::cout<<"depth = "<<depth<<"\t"<<convert_color[mv.piece.color]<<" \t GuessedScore "<<mv.guessedScore<<"\t";
            // printMove(mv);
            // std::cout<<"\n\n";
            Board theNewBoard = bs.board;
            theNewBoard = MakeMove(mv,theNewBoard);
            new_score = minimax(theNewBoard, (!theColor),alpha,beta, depth - 1,moveCounter).first;
            if(new_score < best_score){
            best_score = new_score;
            best_move = mv;
            }
            if (best_score < alpha)
                break;
            beta = std::min(beta, best_score);
        }

        return std::make_pair(best_score, best_move);
    }
}
