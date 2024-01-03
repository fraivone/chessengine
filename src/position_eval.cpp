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


Bitboard BlockerPossibleBitboard(Color Us){
    // opponent move lsit
    MoveList theOpponentMoveList;
    theOpponentMoveList = generate_all(theOpponentMoveList, Color(!Us));
    
    Bitboard OurKing = Position::BitboardsByColor[Us]&Position::BitboardsByType[KING];
    Bitboard WhoIsAttacking = Checkers(theOpponentMoveList,OurKing);

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
        int KingRow = OurKingSquare/nRows;
        int KingCol = OurKingSquare%nCols;
        int AttackerRow = theAttackerSquare/nRows;
        int AttackerCol = theAttackerSquare%nCols;

        int deltaRows = AttackerRow - KingRow;
        int deltaCols = AttackerCol - KingCol;

        // Attacker is touching the king. Cant be blocked
        if(   ( (abs(deltaRows) == 1)  & (abs(deltaCols) == 0) )
            ||( (abs(deltaRows) == 0)  & (abs(deltaCols) == 1) )
            ||( (abs(deltaRows) == 1)  & (abs(deltaCols) == 1) ))
            return possible_blockers_bb;
        else{
            // attack is along a file
            if(AttackerCol == KingCol ){
                while(AttackerRow<KingRow-1){
                    AttackerRow++;
                    possible_blockers_bb |= make_bitboard(make_square(AttackerCol,AttackerRow));
                } 
                while(KingRow<AttackerRow-1){
                    KingRow++;
                    possible_blockers_bb |= make_bitboard(make_square(KingCol,KingRow));
                } 
            }
            // attack is along a row
            else if(AttackerRow == KingRow ){
                while(AttackerCol<KingCol-1){
                    AttackerCol++;
                    possible_blockers_bb |= make_bitboard(make_square(AttackerCol,AttackerRow));
                } 
                while(KingCol<AttackerCol-1){
                    KingCol++;
                    possible_blockers_bb |= make_bitboard(make_square(KingCol,KingRow));
                } 
            }
            // attack is along a diago
            else{
                while(AttackerCol<KingCol-1){
                    AttackerCol++;
                    AttackerRow++;
                    possible_blockers_bb |= make_bitboard(make_square(AttackerCol,AttackerRow));
                } 
                while(KingCol<AttackerCol-1){
                    KingCol++;
                    KingRow++;
                    possible_blockers_bb |= make_bitboard(make_square(KingCol,KingRow));
                } 
            }
        }
    }
    return possible_blockers_bb;
}