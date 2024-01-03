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
        Square from = ((theMove>>6) & 0x3f);
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
        to = ((theMove) & 0x3f);
        Square from = ((theMove>>6) & 0x3f);
        if(make_bitboard(to) & target)
            theBitboard |= (make_bitboard(from));
    }
    return theBitboard;
    
}

// given a piece generate pseudomoves
MoveList generate_pseudomoves(MoveList& moveList,Color Us, Square sq, const PieceType pt){
    Bitboard bb;
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
            bb = king_lut[sq] & ~Position::BitboardsByColor[Us] & ~(Position::BitboardsByColor[!Us] & Position::BitboardsByType[KING]);
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
