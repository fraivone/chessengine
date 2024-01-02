#include <gen_move.hpp>
#include <print.hpp>

Bitboard PawnAnyMoves(Color c, Square s) {
    if(Position::board[s] != make_piece(c,PAWN))
        return 0ULL;
    Square fw_sq = c==WHITE? s + 8: s - 8;
    Square fw2_sq = c==WHITE? s + 16: s - 16;
    Bitboard westAttack_bb = c==WHITE? make_bitboard(s + 7): make_bitboard(s - 9);
    Bitboard eastAttack_bb = c==WHITE? make_bitboard(s + 9): make_bitboard(s - 7);
    Bitboard PawnstartingRow = c == WHITE? Rank2BB: Rank7BB;
    Bitboard emptySquares = ~pieces();

    // if square in front is empty allow pawn move fw
    Bitboard movesBB = emptySquares & PawnFW[c][s];
    // if pawn could be pushed forward and starts from its starting row, check for double FW moves
    movesBB |= ( ((1ULL<<s) & PawnstartingRow) * (movesBB!=0) ) ? Pawn2FW[c][s] & emptySquares: 0ULL;
    // add attacks if there is a piece of the opposite color to capture
    movesBB |= PawnAttacks[c][s] & (westAttack_bb | eastAttack_bb) & Position::BitboardsByColor[!c];
    // allow enpassant captures
    movesBB |= c == Position::sideToMove? (PawnAttacks[c][s] & make_bitboard(Position::st.epSquare)) : 0ULL;

    return movesBB;
}

Bitboard PawnAnyMoves(Square s){
    if (Position::board[s] == NO_PIECE) return 0ULL;
    else
        return PawnAnyMoves(color_of(Position::board[s]),s);
}