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



