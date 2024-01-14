#include "hash.hpp"

namespace HashTables{
    Hashkey PRN_pieces[PIECE_NB][nRows*nCols];
    Hashkey PRN_enpassant[nCols];
    Hashkey PRN_castling[CASTLING_RIGHT_NB];
    Hashkey whomoves;
    void init(){
        // As stockfish https://github.com/official-stockfish/Stockfish/blob/b5e8169a85f6937d7d9d90612863fe5eec72d6ca/src/position.cpp#L117
        PRNG rng(1070372);
        // init pieces
        for(int ic = 0; ic < COLOR_NB; ic++)
            for(int pti=int(PAWN); pti<=int(KING); pti++)
                for (Square s = 0; s < nRows*nCols; ++s){
                    Color c = Color(ic);
                    PieceType pt = PieceType(pti);
                    Piece p = make_piece(c,pt);
                    PRN_pieces[p][s] = rng.rand<Hashkey>();
                }
        // init enpassant
        for (int col_index = 0; col_index < nCols; col_index++)
            PRN_enpassant[col_index] = rng.rand<Hashkey>();
        // init castling
        for (int cr = NO_CASTLING; cr <= ANY_CASTLING; ++cr)
            PRN_castling[cr] = rng.rand<Hashkey>();
        // init color
        whomoves = rng.rand<Hashkey>();
    }
}
