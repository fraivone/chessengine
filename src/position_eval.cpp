#include <position_eval.hpp>
#include <print.hpp>

Bitboard Checkers(MoveList OpponentMoveList, Bitboard OurKingBitboard){
    return Attackers2Bitboard(OpponentMoveList,OurKingBitboard);

}
Bitboard Checkers(Color Us){
    // opponent move lsit
    MoveList theOpponentMoveList;
    theOpponentMoveList = generate_all(theOpponentMoveList, Color(!Us));
    return Checkers(theOpponentMoveList,Position::BitboardsByColor[Us]&Position::BitboardsByType[KING]);
}


Bitboard PossibleBlockerBitboard(Color Us){
    // opponent move lsit
    MoveList theOpponentMoveList;
    theOpponentMoveList = generate_all(theOpponentMoveList, Color(!Us));
    return PossibleBlockerBitboard(theOpponentMoveList, Us);
}

Bitboard PossibleBlockerBitboard(MoveList& opponentMoveList, Color Us){   
    Bitboard OurKing = Position::BitboardsByColor[Us]&Position::BitboardsByType[KING];
    Bitboard WhoIsAttacking = Checkers(opponentMoveList,OurKing);

    int NumberOfAttackers = countBitsOn(WhoIsAttacking);
    Square theAttackerSquare = pop_LSB(WhoIsAttacking);
    PieceType theAttackerType = type_of(Position::board[theAttackerSquare]);
    Bitboard possible_blockers_bb = 0ULL;
    // NO CHECKS, DOUBLE CHECKS, PAWNS and KNIGHTS ATTACKS CAN'T BE BLOCKED
    if (NumberOfAttackers !=1 | (theAttackerType == PAWN) | (theAttackerType == KNIGHT) )
        return possible_blockers_bb;

    // Only 1 blockable attacker
    else{
        Square OurKingSquare = pop_LSB(OurKing);
        possible_blockers_bb |= BetweenBB[theAttackerSquare][OurKingSquare];
    }
    return possible_blockers_bb;
}