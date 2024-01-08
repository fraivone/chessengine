#include <move_maker.hpp>


void MakeMove(Move mv){
    // update the board with the new move
    // put/remove/move piece automatically update the pinners[],checkersBB,pinmap
    // i need to update
    // [DONE] pointer to the previous state info 
    // [DONE] captured piece
    // [DONE] castling rights 
    // [DONE] rule50 
    // [DONE] Square epSquare   
    // [DONE] gamePly (in position::)
    // [DONE] sideToMove(in position::)
    // [DONE] lastly I update the board moving the pieces
    
    
    // I start with copying the current state before any put/remove/move modifies it
    // now previous points is assigned with the value of Position::st
    // Changes in Position::st will not take effect on previous
    *(Position::st.previous) =  Position::st;
    
    Square from = mv_from(mv);
    Square to = mv_to(mv);

    Piece P_from = Position::board[from];
    Piece P_to = Position::board[to];
    Color whoMoves = color_of(P_from);
    MoveType mt = MoveType( mv & 0xC000);
    
    Position::st.capturedPiece = P_to;
    // Handle castles
    if((whoMoves == WHITE)){
        // remove castle WHITE_OOO
        if(from == White_Rook_QueenSide)
            clear_bit(Position::st.castlingRights, 1);
        // remove castle WHITE_OO
        if(from == White_Rook_KingSide)
            clear_bit(Position::st.castlingRights, 0);
        // remove all white castles
        if(from == White_King_Start){
            clear_bit(Position::st.castlingRights, 1);
            clear_bit(Position::st.castlingRights, 0);
        }
    }
    else{
        // remove castle BLACK_OOO
        if(from == Black_Rook_QueenSide)
            clear_bit(Position::st.castlingRights, 3);
        // remove castle BLACK_OO
        if(from == Black_Rook_KingSide)
            clear_bit(Position::st.castlingRights, 2);
        // remove all black castles
        if(from == Black_King_Start){
            clear_bit(Position::st.castlingRights, 2);
            clear_bit(Position::st.castlingRights, 3);
        }
    }
    
    // there was a capture or the pawn was moved 
    if((P_to != NO_PIECE) | (type_of(P_from) == PAWN))
        Position::st.rule50 = 0;
    else
        Position::st.rule50++;
    
    // it was a double push
    if((type_of(P_from) == PAWN) && (abs(from-to)==16))
        // epsquare is the one in between the from and push squares
        Position::st.epSquare = whoMoves == BLACK? from - 8: from + 8;

    Position::gamePly++;
    Position::sideToMove = Color(!Position::sideToMove);

    if(mt == PROMOTION){
        PieceType pt = PieceType(((mv >> 12) & (0x3))+2);
        remove_piece(from,P_from);
        if(P_to != NO_PIECE)
            remove_piece(to,P_to);
        put_piece(to,make_piece(whoMoves,pt));
        }
    else if(mt == ENPASSANT){
        remove_piece(Position::st.previous->epSquare);
        move_piece(from,to,P_from);
    }
    else if( mt == CASTLING){
        // castle is queenside
        if( (to - from) == Castle_QueenSide_KingDelta){
            move_piece(from,to, make_piece(whoMoves,KING));
            Square start = whoMoves == WHITE? White_Rook_QueenSide : Black_Rook_QueenSide;
            move_piece(start, start + Castle_QueenSide_RookDelta, make_piece(whoMoves,ROOK));
        }
        // castle is kingside
        if( (to - from) == Castle_KingSide_KingDelta){
            move_piece(from,to, make_piece(whoMoves,KING));
            Square start = whoMoves == WHITE? White_Rook_KingSide : Black_Rook_KingSide;
            move_piece(start, start + Castle_KingSide_RookDelta, make_piece(whoMoves,ROOK));
        }
    }
    // Normal move
    else{
        if(P_to != NO_PIECE)
            remove_piece(to,P_to);
        move_piece(from,to,P_from);
    }

    Position::UpdatePosition();
}

void UndoMove(Move mv){    
    // [DONE] sideToMove(in position::)
    // [DONE] gamePly (in position::)
    // [DONE] lastly I update the board moving the pieces
    

    // [DONE] pointer to the previous state info 
    // [DONE] captured piece
    // [DONE] castling rights 
    // [DONE] rule50 
    // [DONE] Square epSquare   
    
    // I have to undo the move for this color!
    Position::sideToMove = Color(!Position::sideToMove);
    Color whoMoved = Position::sideToMove;
    Position::gamePly--;

    // recreate board as it was
    Square from = mv_from(mv);
    Square to = mv_to(mv);

    Piece P_to = Position::board[to];
    MoveType mt = MoveType( mv & 0xC000);
    Piece cP = Position::st.capturedPiece;
    
    if(mt == PROMOTION){
        // PieceType PieceType_promotion = PieceType(((mv >> 12) & (0x3))+2);
        // remove the promoted piece
        remove_piece(to,P_to);
        if(cP != NO_PIECE)
            // put the captured piece
            put_piece(to,cP);
        // put back the pawn
        put_piece(from,make_piece(whoMoved,PAWN));
        }
    else if(mt == ENPASSANT){
        // put back the opponent pawn
        put_piece(Position::st.previous->epSquare + whoMoved == WHITE? 8:-8, make_piece(Color(!whoMoved), PAWN));
        // put back our pawn
        move_piece(from,to,P_to);
    }
    else if( mt == CASTLING){
        // castle was queenside
        if( (to - from) == Castle_QueenSide_KingDelta){
            move_piece(to,from, make_piece(whoMoved,KING));
            Square start = whoMoved == WHITE? White_Rook_QueenSide : Black_Rook_QueenSide;
            move_piece(start + Castle_QueenSide_RookDelta, start, make_piece(whoMoved,ROOK));
        }
        // castle is kingside
        if( (to - from) == Castle_KingSide_KingDelta){
            move_piece(to,from, make_piece(whoMoved,KING));
            Square start = whoMoved == WHITE? White_Rook_KingSide : Black_Rook_KingSide;
            move_piece(start + Castle_KingSide_RookDelta, start, make_piece(whoMoved,ROOK));
        }
    }
    // Normal move
    else{
        // first put the attacker back
        move_piece(to,from,P_to);
        // then put back the captured piece
        if(cP != NO_PIECE)
            put_piece(to,cP);
        

    }



    // LAST STEP
    // std::cout<<"Position::st.rule50\t"<<Position::st.rule50<<std::endl;
    // std::cout<<"Position::st.previous->rule50\t"<<Position::st.previous->rule50<<std::endl;
    // re assign previous state
    Position::st = *(Position::st.previous);
    // emptying previous state
    std::memset(Position::st.previous, 0, sizeof(StateInfo));
    // std::cout<<"Position::st.rule50\t"<<Position::st.rule50<<std::endl;
    // std::cout<<"Position::st.previous->rule50\t"<<Position::st.previous->rule50<<std::endl;

    Position::UpdatePosition();   
}


    
    
    
    

// struct StateInfo {

//     // Copied when making a move
//     /// To be understood
//     uint64_t    materialKey;
//     /// To be understood
//     uint64_t    pawnKey;
//     /// To be understood
//     int  nonPawnMaterial[COLOR_NB];
//     int    castlingRights;
//     number of plys since last pawn move or last capture
//     int    rule50;
//     int    pliesFromNull;
//     /// en passant square
//     Square epSquare;

//     // Not copied when making a move (will be recomputed anyhow)
//     /// To be understood
//     uint64_t        key;
//     /// To be understood
//     Bitboard   checkersBB;
//     StateInfo* previous;
//     Bitboard   blockersForKing[COLOR_NB];
//     Bitboard   pinners[COLOR_NB];
//     Bitboard   checkSquares[PIECE_TYPE_NB];
//     Piece      capturedPiece;
//     int        repetition;
// };