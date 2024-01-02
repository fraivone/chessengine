#include <gen_move.hpp>

Bitboard PawnAnyMove(Color c, Square s) {      
    Square fw_sq = c==WHITE? s + 8: s - 8;
    Square fw2_sq = c==WHITE? s + 16: s - 16;
    Square westAttack_sq = c==WHITE? s + 7: s - 9;
    Square eastAttack_sq = c==WHITE? s + 9: s - 7;

    // if square in front is empty allow pawn move fw
    Bitboard movesBB = Position::board[fw_sq] == NO_PIECE? PawnFW[c][s] : 0ULL;

    // Black, 7th rank, 2 empty squares in front ==> allow 2fw
    if ((c == BLACK) && (s>= 48) && (s<56) && (movesBB!=0ULL) && (Position::board[fw2_sq] == NO_PIECE))
            movesBB |= Pawn2FW[c][s];
    // White, 2nd rank, 2 empty squares in front ==> allow 2fw
    if ((c == WHITE) && (s>= 8) && (s<16) && (movesBB!=0ULL) && (Position::board[fw2_sq] == NO_PIECE))
            movesBB |= Pawn2FW[c][s];

    // if there is a piece of the opposite color and it is not the king
    if( (Position::BitboardsByColor[~c] & 1ULL<< westAttack_sq)  &&  (type_of(Position::board[westAttack_sq]) != KING) )
        movesBB |= PawnAttacks[c][s] & (1ULL << westAttack_sq);
    // if there is a piece of the opposite color and it is not the king
    if( (Position::BitboardsByColor[~c] & 1ULL<< eastAttack_sq)  &&  (type_of(Position::board[eastAttack_sq]) != KING) )
        movesBB |= PawnAttacks[c][s] & (1ULL << eastAttack_sq);

    // allow enpassant captures
    movesBB |= c == Position::sideToMove? (PawnAttacks[c][s] & make_bitboard(Position::st.epSquare)) : 0ULL;

    return movesBB;
}