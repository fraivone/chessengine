#include <move_maker.hpp>

std::string stringmv(Move theMove){
    Square from = mv_from(theMove);
    Square to = mv_to(theMove);
    std::string out;
    out += char('a'+from%nCols);
    out += char('1'+from/nRows);
    out += char('a'+to%nCols);
    out += char('1'+to/nRows);

    MoveType mt = MoveType( theMove & 0xC000);
    PieceType pt = PieceType(((theMove >> 12) & (0x3))+2);
    if(mt==PROMOTION)
        out += PieceCharMap[BLACK][pt];

    return out;
}
    


void MakeMove(Move mv){
    // update the board with the new move
    // put/remove/move piece automatically update the pinners[],checkersBB,pinmap
    // i need to update
    // [DONE] pointer to the previous state info 
    // [DONE] captured piece
    // [DONE] castling rights 
    // [DONE] rule50 
    // [DONE] gamePly (in position::)
    // [DONE] sideToMove(in position::)
    // [DONE] Update the board moving the pieces
    // [DONE] Lastly since all the pieces are placed now, check if enpassant should be enabled/disabled   
    
    
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
    
    // if a ROOK is captured, remove opponent castling rights
    if( (P_to != NO_PIECE) && (!whoMoves == WHITE) && (Position::st.castlingRights & WHITE_CASTLING)  && (type_of(P_to) == ROOK)){
        if(to == White_Rook_QueenSide)
            clear_bit(Position::st.castlingRights, 1);
        if(to == White_Rook_KingSide)
            clear_bit(Position::st.castlingRights, 0);
    }
    if( (P_to != NO_PIECE) && (!whoMoves == BLACK) && (Position::st.castlingRights & BLACK_CASTLING)  && (type_of(P_to) == ROOK)){
        if(to == Black_Rook_QueenSide)
            clear_bit(Position::st.castlingRights, 3);
        if(to == Black_Rook_KingSide)
            clear_bit(Position::st.castlingRights, 2);
    }

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

    Position::gamePly++;
    Position::sideToMove = Color(!Position::sideToMove);

    if(mt == PROMOTION){
        PieceType pt = PieceType(((mv >> 12) & (0x3))+2);
        remove_piece(from,P_from);
        if(P_to != NO_PIECE){
            remove_piece(to,P_to);
            // update PST for opponents P_to that gets captured
            SubtractPSTScore(Color(!whoMoves), to, type_of(P_to));
            if (PieceValue[P_to] != PawnValue)
                // Decrease opponent's nonpawnmaterial by the value of pt
                UpdateMaterialCount(Color(!whoMoves), -PieceValue[P_to] );
            
        }
        put_piece(to,make_piece(whoMoves,pt));
        // Increase nonpawnmaterial by the value of pt
        // pt is a promoting piece, can't be pawn!
        UpdateMaterialCount(whoMoves, PieceValue[pt] );
        // update PST for a pawn that became pt
        AddPSTScore(whoMoves,  getPieceSquareTableValue(whoMoves, to, pt) - getPieceSquareTableValue(whoMoves, from, PAWN)  );
        }
    else if(mt == ENPASSANT){
        Square victim = Position::st.previous->epSquare + square_fw[!whoMoves];
        Piece P_cap_enp = Position::board[victim];
        remove_piece(victim,P_cap_enp);
        Position::st.capturedPiece = P_cap_enp;
        move_piece(from,to,P_from);
        // update PST and material count for opponents pawn that gets captured
        SubtractPSTScore(Color(!whoMoves), victim, PAWN );
        UpdateMaterialCount(Color(!whoMoves), -PieceValue[PAWN] );
        // update PST for my pawn that moves from - to
        AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves, to, PAWN) - getPieceSquareTableValue(whoMoves, from, PAWN) );
        
    }
    else if( mt == CASTLING){
        // castle is queenside
        if( (to - from) == Castle_QueenSide_KingDelta){
            move_piece(from,to, make_piece(whoMoves,KING));
            Square start = whoMoves == WHITE? White_Rook_QueenSide : Black_Rook_QueenSide;
            move_piece(start, start + Castle_QueenSide_RookDelta, make_piece(whoMoves,ROOK));
            AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves, to, KING) - getPieceSquareTableValue(whoMoves, from, KING) );
            AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves, start + Castle_QueenSide_RookDelta, ROOK) - getPieceSquareTableValue(whoMoves, start, ROOK) );
        }
        // castle is kingside
        if( (to - from) == Castle_KingSide_KingDelta){
            move_piece(from,to, make_piece(whoMoves,KING));
            Square start = whoMoves == WHITE? White_Rook_KingSide : Black_Rook_KingSide;
            move_piece(start, start + Castle_KingSide_RookDelta, make_piece(whoMoves,ROOK));
            AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves, to, KING) - getPieceSquareTableValue(whoMoves, from, KING) );
            AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves, start + Castle_KingSide_RookDelta, ROOK) - getPieceSquareTableValue(whoMoves, start, ROOK) );
        }
    }
    // Normal move
    else{
        // there is a capture
        if(P_to != NO_PIECE){
            remove_piece(to,P_to);
            AddPSTScore(Color(!whoMoves), -getPieceSquareTableValue(Color(!whoMoves), to, type_of(P_to)) );
            if (PieceValue[P_to] != PawnValue)
                // Decrease opponent's nonpawnmaterial by the value of pt
                UpdateMaterialCount(Color(!whoMoves), -PieceValue[P_to] );
            }
        move_piece(from,to,P_from);
        AddPSTScore(whoMoves, getPieceSquareTableValue(whoMoves,to,type_of(P_from))- getPieceSquareTableValue(whoMoves, from, type_of(P_from)) );
    }

    // en passant might need to be enabled if enpassant was available, it's a pawn move, it moved 2 squares
    if((type_of(P_from) == PAWN) && (abs(from-to)==16)){
        // check for enpassant
        Square epSquare = to  + square_bw[whoMoves];
        Bitboard epBB = make_bitboard(epSquare);
        // enpassant rules
        // a) side to move has a pawn threatening epSquare
        // b) there is an enemy pawn in front of epSquare --> Granted, we just moved a pawn there
        // c) there is no piece on epSquare or behind epSquare
        
        if ((pieces(Color(!whoMoves),PAWN) && (  (epBB >>1) | (epBB << 1) ) )                            //a)
            &&(Position::board[epSquare] == NO_PIECE) & (Position::board[epSquare+square_bw[whoMoves]] == NO_PIECE))  //c)
            Position::st.epSquare = epSquare;
        else
            Position::st.epSquare = ENPSNT_UNAVAILABLE;
    }
    else
        Position::st.epSquare = ENPSNT_UNAVAILABLE;



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
        Square victim = Position::st.previous->epSquare + square_fw[!whoMoved];
        put_piece(victim, make_piece(Color(!whoMoved), PAWN));
        // put back our pawn
        move_piece(to,from,P_to);
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

    
uint64_t StupidPerftCount(MoveList mvList, uint8_t depth,uint8_t MaxDepth, bool verbose, uint64_t count){
    if (depth == 0){
        // std::cout<<std::string(3-depth-1, '\t')<<"Move list has size "<<mvList.size<<std::endl;
        count += mvList.size;
        return count;
    }
    MoveList legalMoves;
    StateInfo previousState;
    uint64_t prevCount = count;
    
    previousState = Position::st;
    for(int i=0; i<mvList.size; i++){
        // std::cout<<std::string(4-depth-1, '\t')<<"Depth"<<+depth<<"   ";
        MakeMove(mvList.list[i].move);
        // std::cout<<"Made"<<std::endl;
        legalMoves.Clear();
        legalMoves = generate_legal(Position::sideToMove);
        // std::cout<<std::string(depth-1, '\t')<<"Depth "<<+depth<<depth<<"\tMove["<<i<<"] has "<<legalMoves.size<<std::endl;
        // std::cout<<std::string(3-depth-1, '\t')<<"Depth "<<+depth<<" - Before Move["<<i<<"]"<<"\tNow Count is "<<count<<std::endl;
        if(depth>1)
            count = StupidPerftCount(legalMoves,depth-1,MaxDepth,verbose, count);
        else
            count += legalMoves.size;
        
        if((depth == MaxDepth) & verbose)
            std::cout<<stringmv(mvList.list[i].move)<<": " <<(count - prevCount)<<std::endl;
        prevCount = count;
        // std::cout<<std::string(3-depth-1, '\t')<<"Depth "<<+depth<<" - After Move["<<i<<"] "<<"\tNow Count is "<<count<<"\n"<<std::endl;
        // UNDO
        // the previous status might have gotten lost
        // if depth > 1
        *(Position::st.previous) = previousState;
        UndoMove(mvList.list[i].move);
    }
    return count;
}

uint64_t StupidPerftCount(std::string FEN, uint8_t depth, bool verbose){
    init_position(FEN);
    MoveList mvl;
    mvl = generate_legal(Position::sideToMove);
    return StupidPerftCount(mvl,depth,depth,verbose);
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