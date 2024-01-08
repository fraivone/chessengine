#include <gen_move.hpp>


// template<bool Enemy>
MoveList make_promotions(MoveList& moveList,Square from, Square to) {
        moveList.Add(make<PROMOTION>(from, to, QUEEN));
        moveList.Add(make<PROMOTION>(from, to, ROOK));
        moveList.Add(make<PROMOTION>(from, to, BISHOP));
        moveList.Add(make<PROMOTION>(from, to, KNIGHT));

    return moveList;
}

MoveList PawnAnyMoves(MoveList& moveList, Color c, Square s) {
    // Move generation is steps:
    // Step1(abc). NO PROMOTION
    // Step2(ab). PROMOTION
    
    if(Position::board[s] != make_piece(c,PAWN))
        return moveList;
    
    Bitboard RankBeforePromotion    = c==WHITE? Rank7BB:Rank2BB;
    Bitboard NotRankBeforePromotion = c==WHITE? ~Rank7BB:~Rank2BB;
    Bitboard thisSq_bb = make_bitboard(s);
    Square fw_sq = c==WHITE? s + 8: s - 8;
    Square fw2_sq = c==WHITE? s + 16: s - 16;
    Bitboard westAttack_bb = c==WHITE? make_bitboard(s + 7): make_bitboard(s - 9);
    Bitboard eastAttack_bb = c==WHITE? make_bitboard(s + 9): make_bitboard(s - 7);
    Bitboard PawnInitRow = c == WHITE? Rank2BB: Rank7BB;
    Bitboard emptySquares = ~pieces();
    Bitboard Step1_enpassant = 0ULL;
    // STEPS1 - NO PROMOTION
    if(thisSq_bb & NotRankBeforePromotion){
        
        
        // Step 1a, no captures & no promotion. Only fw 
        // if square in front is empty allow pawn move fw
        Bitboard Step1_fw1 = emptySquares & PawnFW[c][s];
        // if pawn could be pushed forward and starts from its init row, check for double FW moves
        Bitboard Step1_fw2 = ( ((thisSq_bb) & PawnInitRow) * (Step1_fw1!=0) ) ? Pawn2FW[c][s] & emptySquares: 0ULL;
        // Step1b capture without promotion
        Bitboard Step1_nonPromotional_captures = PawnAttacks[c][s] & (westAttack_bb | eastAttack_bb) & Position::BitboardsByColor[!c];

        // Step1c capture enpassant (of course without promotion)
        if (Position::st.epSquare != ENPSNT_UNAVAILABLE){
            Step1_enpassant |= (c == Position::sideToMove)*(PawnAttacks[c][s] & make_bitboard(Position::st.epSquare));
        }
        
        while(Step1_fw1)
            moveList.Add(make_move(s, pop_LSB(Step1_fw1)));
        while(Step1_fw2)
            moveList.Add(make_move(s, Square(pop_LSB(Step1_fw2))));
        while(Step1_nonPromotional_captures)
            moveList.Add(make_move(s, Square(pop_LSB(Step1_nonPromotional_captures))));
        while(Step1_enpassant)
            moveList.Add(make<ENPASSANT>(s, Square(pop_LSB(Step1_enpassant))));
    }
    
    // STEPS2 - PROMOTION
    else{
        // Step2a promotion only
        Bitboard Step2_fw1 = emptySquares & PawnFW[c][s];
        if(Step2_fw1)
            moveList = make_promotions(moveList,s,pop_LSB(Step2_fw1));
        
        // Step2b captures with promotion
        Bitboard Step2_promotional_captures = PawnAttacks[c][s] & (westAttack_bb | eastAttack_bb) & Position::BitboardsByColor[!c];
        while(Step2_promotional_captures)
            moveList = make_promotions(moveList,s,pop_LSB(Step2_promotional_captures));
    }
    
    return moveList;
}

MoveList PawnAnyMoves(MoveList& moveList, Square s){
    if (Position::board[s] == NO_PIECE) 
        return moveList;
    else
        return PawnAnyMoves(moveList, color_of(Position::board[s]),s);
}



MoveList PawnAnyEvasionsMoves(MoveList& moveList, Color c, Square s, Bitboard Checkers, Bitboard possibleBlockersBB) {
    // Only blocking or captures of the checker piece can be evasions
    // generate the pawn moves that lead to evasion
    // function to be called exclusively when there's only 1 checker
    
    if(Position::board[s] != make_piece(c,PAWN))
        return moveList;
    Bitboard thisSq_bb = make_bitboard(s);
    // tldr
    // if it is not pinned, a pawn can block attack/capture checker
    // if a pawn is pinned, it can only block!
    // If a pawn is pinned
    // it can still move forward as long as those squares are blocking squares. (INCLUDED)
    // BUT it CANNOT capture the checker.  (EXCLUDED)
    // Indeed if a pawn is pinned along a line (row/col)
    // it can't caputre anything, cause captures are diagonally -->forbidden
    // Conversely a pawn is pinned diagonally, its attacker would not be delivering check (it's blocked!)
    // so the pawn would need to capture another piece that's not the attacker -->forbidden
    
    

    // From now on we assume only 1Checker
    Bitboard RankBeforePromotion    = c==WHITE? Rank7BB:Rank2BB;
    Bitboard NotRankBeforePromotion = c==WHITE? ~Rank7BB:~Rank2BB;
    Square fw_sq = c==WHITE? s + 8: s - 8;
    Square fw2_sq = c==WHITE? s + 16: s - 16;
    Bitboard westAttack_bb = c==WHITE? make_bitboard(s + 7): make_bitboard(s - 9);
    Bitboard eastAttack_bb = c==WHITE? make_bitboard(s + 9): make_bitboard(s - 7);
    // bitboard that represent the checker captured by the move enpassant
    Bitboard CheckerEnpassant = c == WHITE? Checkers << 8: Checkers >> 8;
    Bitboard PawnInitRow = c == WHITE? Rank2BB: Rank7BB;
    Bitboard emptySquares = ~pieces();
    Bitboard Step1_enpassant = 0ULL;
    // STEPS1 - EVADE WITHOUT PROMOTING
    if(thisSq_bb & NotRankBeforePromotion){
        
        // if square in front is empty. I will add the blocker
        // in a couple of line as this variable is also
        // used for the fw2 movement
        Bitboard Step1_fw1 = emptySquares & PawnFW[c][s];
        // if pawn could be pushed forward and starts from its init row, check for double FW moves
        Bitboard Step1_fw2 = ( ((thisSq_bb) & PawnInitRow) * (Step1_fw1!=0) ) ? (Pawn2FW[c][s] & emptySquares) & possibleBlockersBB: 0ULL;
        Step1_fw1 &= possibleBlockersBB;
        
        // capture the opponent checker if NOT pinned
        if(Position::PinMap[c][s] == ENPSNT_UNAVAILABLE){
            Bitboard Step1_nonPromotional_captures = PawnAttacks[c][s] & (westAttack_bb | eastAttack_bb) & Position::BitboardsByColor[!c] & Checkers;
            while(Step1_nonPromotional_captures)
            moveList.Add(make_move(s, Square(pop_LSB(Step1_nonPromotional_captures))));
        }

        // Capture enpassant the opponent checker enpassant(of course without promotion)
        if (Position::st.epSquare != ENPSNT_UNAVAILABLE){
            Step1_enpassant |= (c == Position::sideToMove)*(PawnAttacks[c][s] & make_bitboard(Position::st.epSquare) & CheckerEnpassant);
        }
        
        while(Step1_fw1)
            moveList.Add(make_move(s, pop_LSB(Step1_fw1)));
        while(Step1_fw2)
            moveList.Add(make_move(s, Square(pop_LSB(Step1_fw2))));
        while(Step1_enpassant)
            moveList.Add(make<ENPASSANT>(s, Square(pop_LSB(Step1_enpassant))));
    }
    
    // STEPS2 - PROMOTION WITH CAPTURE
    else{
        // Only promotion on a blocker square
        Bitboard Step2_fw1 = emptySquares & PawnFW[c][s] & possibleBlockersBB;
        if(Step2_fw1)
            moveList = make_promotions(moveList,s,pop_LSB(Step2_fw1));

        // promotion and capture the opponent checker if NOT pinned
        if(Position::PinMap[c][s] == ENPSNT_UNAVAILABLE){
            Bitboard Step2_promotional_captures = PawnAttacks[c][s] & (westAttack_bb | eastAttack_bb) & Position::BitboardsByColor[!c] & Checkers;
            while(Step2_promotional_captures)
                moveList = make_promotions(moveList,s,pop_LSB(Step2_promotional_captures));
        }
    }
    
    return moveList;
}

MoveList PawnAnyEvasionsMoves(MoveList& moveList, Square s, Bitboard Checkers, Bitboard possibleBlockersBB){
    if (Position::board[s] == NO_PIECE) 
        return moveList;
    else
        return PawnAnyEvasionsMoves(moveList, color_of(Position::board[s]), s, Checkers, possibleBlockersBB);
}

Bitboard Attacked2Bitboard(const MoveList moveList){
    Move theMove;
    Square to;
    Bitboard theBitboard = 0ULL;
    for(int i = 0; i<moveList.size; i++){
        theMove = moveList.list[i].move;
        to = ((theMove) & 0x3f);
        theBitboard |= make_bitboard(to);
    }
    return theBitboard;
}

Bitboard Attackers2Bitboard(const MoveList moveList){
    Move theMove;
    Square from;
    Bitboard theBitboard = 0ULL;
    for(int i = 0; i<moveList.size; i++){
        theMove = moveList.list[i].move;
        Square from = mv_from(theMove);
        theBitboard |= make_bitboard(from);
    }
    return theBitboard;
}

Bitboard Attackers2Bitboard(const MoveList moveList, Bitboard target){
    Move theMove;
    Square from;
    Square to;
    Bitboard theBitboard = 0ULL;
    for(int i = 0; i<moveList.size; i++){
        theMove = moveList.list[i].move;
        to = mv_to(theMove);
        Square from = mv_from(theMove);
        if(make_bitboard(to) & target)
            theBitboard |= (make_bitboard(from));
    }
    return theBitboard;
    
}

MoveList generate_pseudomoves(MoveList& moveList,Color Us, Square sq, const PieceType pt){
    Bitboard bb;
    Bitboard opponent_pawns = Position::BitboardsByType[PAWN] & Position::BitboardsByColor[!Us];
    Bitboard opponent_knights = Position::BitboardsByType[KNIGHT] & Position::BitboardsByColor[!Us];
    Bitboard opponent_rooks = Position::BitboardsByType[ROOK] & Position::BitboardsByColor[!Us];
    Bitboard opponent_bishops = Position::BitboardsByType[BISHOP] & Position::BitboardsByColor[!Us];
    Bitboard opponent_queens = Position::BitboardsByType[QUEEN] & Position::BitboardsByColor[!Us];
    Bitboard opponents_attacked_squares = 0ULL;
    Bitboard kgBB;
    Bitboard OpponentsKing = pieces(Color(!Us),KING);
        switch(pt) {
        case PAWN:
            return PawnAnyMoves(moveList, Us, sq);
        case KNIGHT:
            bb = knight_lut[sq] & ~Position::BitboardsByColor[Us];
            break;
        case BISHOP:
        case ROOK :
        case QUEEN:
            bb = get_sliding_landings(pt, sq, pieces())& ~Position::BitboardsByColor[Us];
            break;
        case KING:
            kgBB = make_bitboard(sq);
            // KING can't capture it's own piece
            bb = king_lut[sq] & ~Position::BitboardsByColor[Us];
            
            // don't put the king in check
            
            // opponents king attacks
            opponents_attacked_squares = king_lut[pop_LSB(OpponentsKing)];
            // opponents pawns attacks
            while(opponent_pawns)
                opponents_attacked_squares |= PawnAttacks[!Us][pop_LSB(opponent_pawns)];
            // opponents knights attacks
            while(opponent_knights)
                opponents_attacked_squares |= knight_lut[pop_LSB(opponent_knights)];
            // opponents rooks attacks. The rook attacks the whole space as if the king wasn't there
            while(opponent_rooks)
                opponents_attacked_squares |= get_sliding_landings(ROOK, pop_LSB(opponent_rooks), (pieces()&~kgBB));
            // opponents bishops attacks. The bishop attacks the whole space as if the king wasn't there
            while(opponent_bishops)
                opponents_attacked_squares |= get_sliding_landings(BISHOP, pop_LSB(opponent_bishops), (pieces()&~kgBB));            
            // opponents queens attacks. The queen attacks the whole space as if the king wasn't there
            while(opponent_queens)
                opponents_attacked_squares |= get_sliding_landings(QUEEN, pop_LSB(opponent_queens), (pieces()&~kgBB));
            bb &= ~opponents_attacked_squares;
            // castling moves:
            // relies entirely on the correctness of the stateinfo
            // 0. Castles is still valid
            // 1. No friendly/enemy pieces between the king and the rook
            // 2. no attackers pointing at the squares between where the king is and where it wants to go
            // However the BetweenBB doesn't include the landing squares. So I have added an extra check to make sure that (2) includes the king's landing square
            // WHITE CASTLES
            // TODO improve this block of poorly written code (that works)
            if((Us == WHITE) && (Position::st.castlingRights & WHITE_CASTLING)){
                
                if( (WHITE_OOO & Position::st.castlingRights) 
                    && !(BetweenBB[White_King_Start][White_Rook_QueenSide] & pieces()) 
                    && !( (BetweenBB[White_King_Start][White_King_Start + Castle_QueenSide_KingDelta] | make_bitboard(White_King_Start + Castle_QueenSide_KingDelta)) & opponents_attacked_squares))
                    moveList.Add(make<CASTLING>(White_King_Start, White_King_Start + Castle_QueenSide_KingDelta));
                if( (WHITE_OO & Position::st.castlingRights) 
                    && !(BetweenBB[White_King_Start][White_Rook_KingSide] & pieces()) 
                    && !( ( BetweenBB[White_King_Start][White_King_Start + Castle_KingSide_KingDelta] | make_bitboard(White_King_Start + Castle_KingSide_KingDelta)) & opponents_attacked_squares) )
                    moveList.Add(make<CASTLING>(White_King_Start, White_King_Start + Castle_KingSide_KingDelta));
            }
            
            // BLACK CASTLES
            if((Us == BLACK) && (Position::st.castlingRights & BLACK_CASTLING)){
                if( (BLACK_OOO & Position::st.castlingRights) 
                    && !(BetweenBB[Black_King_Start][Black_Rook_QueenSide] & pieces()) 
                    && !( (BetweenBB[Black_King_Start][Black_King_Start + Castle_QueenSide_KingDelta]| make_bitboard(Black_King_Start + Castle_QueenSide_KingDelta)) & opponents_attacked_squares))
                    moveList.Add(make<CASTLING>(Black_King_Start, Black_King_Start + Castle_QueenSide_KingDelta));
                if((BLACK_OO & Position::st.castlingRights) 
                    && !(BetweenBB[Black_King_Start][Black_Rook_KingSide] & pieces()) 
                    && !( (BetweenBB[Black_King_Start][Black_King_Start + Castle_KingSide_KingDelta] | make_bitboard(Black_King_Start + Castle_KingSide_KingDelta)) & opponents_attacked_squares))
                    moveList.Add(make<CASTLING>(Black_King_Start, Black_King_Start + Castle_KingSide_KingDelta));
            }
            break;
        default: 
            bb = 0ULL;
        }
        while(bb)
            moveList.Add(make_move(sq,pop_LSB(bb)));
        
    return moveList;
}



MoveList generate_evasion_moves(MoveList& moveList,Color Us, Square sq, const PieceType pt, Bitboard Checkers, Bitboard BlockersBB){
    // Only blocking or captures of the checker piece can be evasions
    // generate pseudomoves that lead to evasion
    // Exclude pinned pieces as they won't help now   

    Bitboard bb;
    Bitboard opponent_pawns = Position::BitboardsByType[PAWN] & Position::BitboardsByColor[!Us];
    Bitboard opponent_knights = Position::BitboardsByType[KNIGHT] & Position::BitboardsByColor[!Us];
    Bitboard opponent_rooks = Position::BitboardsByType[ROOK] & Position::BitboardsByColor[!Us];
    Bitboard opponent_bishops = Position::BitboardsByType[BISHOP] & Position::BitboardsByColor[!Us];
    Bitboard opponent_queens = Position::BitboardsByType[QUEEN] & Position::BitboardsByColor[!Us];
    Bitboard opponents_attacked_squares = 0ULL;
    Bitboard OpponentsKing = pieces(Color(!Us),KING);
    Bitboard kgBB;
        switch(pt) {
        case PAWN:
            return PawnAnyEvasionsMoves(moveList, Us, sq, Checkers, BlockersBB);
        case KNIGHT:
            // if the knight is pinned, it can't move whatsoever
            // so only check cases in which the knight isn't pinned
            if(!(make_bitboard(sq) & Position::pinnedPieces[Us]))
                // land the knight either on a blocker square or capture the checker
                bb = (knight_lut[sq] & ~Position::BitboardsByColor[Us] & (BlockersBB | Checkers) );
            break;
        case BISHOP:
        case ROOK :
        case QUEEN:
            // Sliding pieces, even if pinned, can always move along the 
            // rays of the in between squares (blockersBB) to block the attack (1)
            // If they are NOT pinned, they can also capture ANY checker
            // If they are pinned, they can only capture the checker along rays (3)
            // 
            // TODO check this logic :D // I checked and fixed it once
            bb = get_sliding_landings(pt, sq, pieces())& ~Position::BitboardsByColor[Us];

            bb &= ( BlockersBB | ( (Position::PinMap[Us][sq]==ENPSNT_UNAVAILABLE)*Checkers ) | ( (Position::PinMap[Us][sq]!=ENPSNT_UNAVAILABLE)*(Checkers& Ray[sq][Position::PinMap[Us][sq]]) ) ); //(1) + (2) + (3)
            break;
        case KING:
            kgBB = make_bitboard(sq);
            // all KING landings except those which capture it's own piece
            bb = king_lut[sq] & ~Position::BitboardsByColor[Us];
            // don't put the king in check
            
            // opponents king attacks
            opponents_attacked_squares = king_lut[pop_LSB(OpponentsKing)];
            // opponents pawns attacks
            while(opponent_pawns)
                opponents_attacked_squares |= PawnAttacks[!Us][pop_LSB(opponent_pawns)];
            // opponents knights attacks
            while(opponent_knights)
                opponents_attacked_squares |= knight_lut[pop_LSB(opponent_knights)];
            // opponents rooks attacks. The rook attacks the whole space as if the king wasn't there
            while(opponent_rooks)
                opponents_attacked_squares |= get_sliding_landings(ROOK, pop_LSB(opponent_rooks), (pieces()&~kgBB));
            // opponents bishops attacks. The bishop attacks the whole space as if the king wasn't there
            while(opponent_bishops)
                opponents_attacked_squares |= get_sliding_landings(BISHOP, pop_LSB(opponent_bishops), (pieces()&~kgBB));
            // opponents queens attacks. The queen attacks the whole space as if the king wasn't there
            while(opponent_queens)
                opponents_attacked_squares |= get_sliding_landings(QUEEN, pop_LSB(opponent_queens), (pieces()&~kgBB));
            bb &= ~opponents_attacked_squares;
            // avoid attacked squares
            bb &= ~opponents_attacked_squares;
            // No castle: if the king is checked, can't evade with castle
            break;
        default: 
            bb = 0ULL;
        }
        while(bb)
            moveList.Add(make_move(sq,pop_LSB(bb)));
        
    return moveList;
}

MoveList generate_all(MoveList& moveList,Color Us){
    Bitboard OurPieces = pieces(Us);
    while(OurPieces){
        Square sq = pop_LSB(OurPieces);
        moveList = generate_pseudomoves(moveList, Us, sq, type_of(Position::board[sq]));
    }
    return moveList;
}

MoveList generate_all(MoveList& moveList, Color Us, const PieceType pt){
    Bitboard OurPieces = Position::BitboardsByType[pt] & Position::BitboardsByColor[Us];
    while(OurPieces)
        moveList = generate_pseudomoves(moveList, Us, pop_LSB(OurPieces), pt);

    return moveList;
}
/// generate all evasion moves in this postion
MoveList generate_allevasion_moves(MoveList& moveList,Color Us,Bitboard Checkers, Bitboard blockersBB){
    Bitboard OurPieces = pieces(Us);
    while(OurPieces){
        Square sq = pop_LSB(OurPieces);
        moveList = generate_evasion_moves(moveList, Us, sq, type_of(Position::board[sq]), Checkers, blockersBB);
    }
    return moveList;
}
/// generate all evasion moves in this postion for PieceType
MoveList generate_allevasion_moves(MoveList& moveList, Color Us, const PieceType pt, Bitboard Checkers, Bitboard blockersBB){
    Bitboard OurPieces = pieces(Us,pt);
    while(OurPieces)
        moveList = generate_evasion_moves(moveList, Us, pop_LSB(OurPieces), pt, Checkers, blockersBB);

    return moveList;
}


MoveList generate_legal(Color Us){
    MoveList output;
    int numberOfChecks = countBitsOn(Position::st.checkersBB);
    Bitboard blockersBB = PossibleBlockersBB(Us);
    Bitboard movingTrace;
    Bitboard allowedPath;
    
    // There are 0 checkers, return pseudomvoes that don't involve pins
    if(numberOfChecks == 0){
        output = generate_all(output, Us);
        Square from;
        // discard moves of pinned pieces if there are any pieces pinned 
        if(countBitsOn(Position::pinnedPieces[Us]) != 0){
            for(int i=0; i<output.size; i++){
                Square from = mv_from(output.list[i].move);
                // check if this piece is actually pinned
                Square pinner = Position::PinMap[Us][from];
                if(pinner == ENPSNT_UNAVAILABLE)
                    continue;
                Square to = mv_to(output.list[i].move);
                Bitboard toBB = make_bitboard(to);
                // ray "from" to "to" including "to" but excluding "from"
                movingTrace = BetweenBB[from][to] | toBB;
                // ray "from" to "pinner" including "to" but excluding "pinner"
                allowedPath = Ray[from][pinner];
                // not only pinned, this move even tries to move the pinned piece
                // out of the allowed path. Pop the move
                if( (movingTrace & allowedPath) != movingTrace){
                    output.Pop(i);
                    
                    i--;
                }
            }
        }
        return output;
    }

    // only 1 check, blocking scenario
    else if( (numberOfChecks == 1)){
        output = generate_allevasion_moves(output, Us, Position::st.checkersBB, blockersBB);
        return output;
    }
    // number of checks > 1, run the king
    else{
        output = generate_all(output,Us,KING);
        return output;
    }

}

Bitboard Checkers(MoveList OpponentMoveList, Bitboard OurKingBitboard){
    return Attackers2Bitboard(OpponentMoveList,OurKingBitboard);

}


Bitboard Checkers(Color Us, Bitboard OurKingBB){
    Bitboard OpponentPieces = pieces(Color(!Us));
    Bitboard Checkers = 0ULL;
    Square theAttackerSquare;
    PieceType theAttackerType;

    while(OpponentPieces){
        theAttackerSquare = pop_LSB(OpponentPieces);
        theAttackerType = type_of(Position::board[theAttackerSquare]);
        switch(theAttackerType) {
        case PAWN:
            Checkers |= (OurKingBB & PawnAttacks[!Us][theAttackerSquare])? make_bitboard(theAttackerSquare) : 0ULL;
            break;
        case KNIGHT:
            Checkers |= (OurKingBB & knight_lut[theAttackerSquare]) ? make_bitboard(theAttackerSquare) : 0ULL;
            break;
        case BISHOP:
        case ROOK:
        case QUEEN:
            Checkers |= (OurKingBB & get_sliding_landings(theAttackerType, theAttackerSquare, pieces()) ) ? make_bitboard(theAttackerSquare) : 0ULL;
            break;
        }
    }
    return Checkers;
}

Bitboard Checkers(Color Us){
    return Checkers(Us,pieces(Us,KING));
}

Bitboard PossibleBlockersBB(Color Us){
    // can't block a double check
    if (countBitsOn(Position::st.checkersBB) != 1)
        return 0ULL;

    Square theAttackerSquare = get_LSB(Position::st.checkersBB);
    PieceType pt = type_of(Position::board[theAttackerSquare]);
    // can't block PAWNs or KNIGHTs checks
    if (pt == PAWN | pt == KNIGHT)
        return 0ULL;
    Bitboard ourKingBB = pieces(Us,KING);
    return BetweenBB[theAttackerSquare][pop_LSB(ourKingBB)];

}


// LEGACY
// MoveList generate_evasions(MoveList& OurPseudoMoves, Bitboard Checkers, Color Us){
//     MoveList evasionsList;
//     // generate blocking + capturing attacker moves
//     evasionsList = generate_BlockingMoves(Us, OurPseudoMoves, Checkers);
//     // generate remaining king legals
//     evasionsList = generate_all(evasionsList, Us, KING);
//     return evasionsList;
// }

// LEGACY
// /// Among the previously calcuted pseudomoves, 
// /// only return the ones that allow to block the checkers and involve non-pinned pieces
// MoveList generate_BlockingMoves(Color Us, MoveList& OurPseudoMoves, Bitboard Checkers){
//     Bitboard blockersBB = PossibleBlockersBB(Us, Checkers);
//     MoveList blockingMoves;
//     // if there are no possible blocking squares, no blocking moves
//     if(blockersBB == 0ULL)
//         return blockingMoves;
//     // if blocking is possible, there is only one checker
//     else{
//         Move mv;
//         Square to,from;
//         Square checkerSq = pop_LSB(Checkers);
//         // loop through the moves to find only the blocking ones from unpinned pieces
//         for(int i=0; i<OurPseudoMoves.size; i++){
//             mv = OurPseudoMoves.list[i].move;
//             to = mv_to(mv);
//             from = mv_from(mv);
//             // this move can block the check and this piece is not pinned
//             if( (make_bitboard(to)& blockersBB) && !(Position::pinnedPieces[Us] & make_bitboard(from)) )
//                 blockingMoves.Add(mv);
//             // this move captures the checker
//             if( checkerSq == to  )
//                 blockingMoves.Add(mv);
//         }
//         return blockingMoves;
//     }
// }