#include <position.hpp>

namespace Position {
    Piece board[nCols*nRows];
    Bitboard BitboardsByType[PIECE_TYPE_NB];
    Bitboard BitboardsByColor[COLOR_NB];
    Color sideToMove;
    StateInfo st;
    int gamePly;
    Bitboard pinnedPieces;

    void init(){
        // squares are empty
        for(int i = 0; i<nRows*nCols; i++)
            board[i] = NO_PIECE;
        // all bitboards are empty
        for(int i = 0; i<PIECE_TYPE_NB; i++)
            BitboardsByType[i] = 0;
        // all bitboards are empty
        for(int i = 0; i<COLOR_NB; i++)
            BitboardsByColor[i] = 0;
        
        std::memset(&st, 0, sizeof(StateInfo));
        gamePly = 0;
    }
}


void init_position(std::string FEN){
    Position::init();
    /// set boards and bitboards from FEN
    setBBFromFEN(FEN);
    Position::pinnedPieces = PinnedPieces(Position::sideToMove, (Position::BitboardsByType[KING] & Position::BitboardsByColor[Position::sideToMove] ));
}

void put_piece(Square square, Piece PP){
    Position::board[square] = PP;
    Position::BitboardsByType[ALL_PIECES] |= 1ULL << square;
    Position::BitboardsByType[type_of(PP)] |= 1ULL << square;
    Position::BitboardsByColor[color_of(PP)] |= 1ULL << square;
}
void remove_piece(Square square,Piece PP){
    Position::board[square] = NO_PIECE;
    Position::BitboardsByType[ALL_PIECES] &= 0ULL << square;
    Position::BitboardsByType[type_of(PP)] &= 0ULL << square;
    Position::BitboardsByColor[color_of(PP)] &= 0ULL << square;
}
void remove_piece(Square square){
    Position::board[square] = NO_PIECE;
    for(int i = 0; i<PIECE_TYPE_NB; i++)
        Position::BitboardsByType[i] &= 0ULL << square;
    for(int i = 0; i<COLOR_NB; i++)
        Position::BitboardsByColor[i] &= 0ULL << square;
}

void move_piece(Square startsquare,Square endsquare, Piece PP){
    remove_piece(startsquare,PP);
    put_piece(endsquare,PP);
}

void move_piece(Square startsquare,Square endsquare){
    Piece PP = Position::board[startsquare];    
    // remove
    Position::board[startsquare] = NO_PIECE;
    // add
    Position::board[endsquare] = PP;

    // remove
    Position::BitboardsByType[ALL_PIECES] &= 0ULL << startsquare;
    Position::BitboardsByType[type_of(PP)] &= 0ULL << startsquare;
    // add
    Position::BitboardsByType[type_of(PP)] |= 1ULL << endsquare;
    Position::BitboardsByType[ALL_PIECES] |= 1ULL << endsquare;

    // remove
    Position::BitboardsByColor[color_of(PP)] &= 0ULL << startsquare;
    // add
    Position::BitboardsByColor[color_of(PP)] |= 1ULL << endsquare;
}

void set_castling_right(Color c, char FEN_char){
    // castle king side for color c
    if(char(FEN_char) == 'K')
        Position::st.castlingRights |= (WHITE_OO << (2*c));
    // castle queen side for color c
    if(char(FEN_char) == 'Q')
        Position::st.castlingRights |= (WHITE_OOO << (2*c));
}


void setBBFromFEN(std::string FEN){
    unsigned char      col, row, token;
    size_t             idx;
    Square             sq = 56;
    std::istringstream ss(FEN);
    ss >> std::noskipws; // don't skip white spaces
    


    while ((ss >> token) && !isspace(token))
    {
        if (isdigit(token))
            sq += (token - '0') * EAST;  // Advance the given number of files

        else if (token == '/')
            sq += 2 * SOUTH;

        else if ((idx = PieceToChar.find(token)) != std::string::npos)
        {
            put_piece(sq, Piece(idx));
            ++sq;
        }
    }

    // 2. Active color
    ss >> token;
    Position::sideToMove = (token == 'w' ? WHITE : BLACK);
    ss >> token;

    // 3. Castling availability. Compatible with 3 standards: Normal FEN standard,
    // Shredder-FEN that uses the letters of the columns on which the rooks began
    // the game instead of KQkq and also X-FEN standard that, in case of Chess960,
    // if an inner rook is associated with the castling right, the castling tag is
    // replaced by the file letter of the involved rook, as for the Shredder-FEN.
    while ((ss >> token) && !isspace(token))
    {
        Square rsq;
        Color  c    = islower(token) ? BLACK : WHITE;
        Piece  rook = make_piece(c, ROOK);
        token = char(toupper(token));
        set_castling_right(c, token);
    }
    if (VERBOSE > 2) std::cout<<"Castling rights is "<<CastlingNames(Position::st.castlingRights)<<std::endl;

    // 4. En passant square.
    // Ignore if square is invalid or not on side to move relative rank 6.
    bool enpassant = false;
    if (((ss >> col) && (col >= 'a' && col <= 'h'))
        && ((ss >> row) && (row == (Position::sideToMove == WHITE ? '6' : '3')))){
        Position::st.epSquare = Square((col - 'a') +(row - '1')*8);

        // En passant square will be considered only if
        // a) side to move has a pawn threatening epSquare
        // b) there is an enemy pawn in front of epSquare
        // c) there is no piece on epSquare or behind epSquare
        
        enpassant = PawnAttacks[Color(!Position::sideToMove)][Position::st.epSquare] & pieces(Color(Position::sideToMove), PAWN)
                    && (pieces(Color(!Position::sideToMove), PAWN) & make_bitboard((Position::st.epSquare + (Position::sideToMove==WHITE? -8: +8))))
                    && !(pieces() & (make_bitboard(Position::st.epSquare) | make_bitboard(Position::st.epSquare + (Position::sideToMove==WHITE? 8: -8))));
    }
    if (!enpassant)
        Position::st.epSquare = ENPSNT_UNAVAILABLE;

    // 5-6. Halfmove clock and fullmove number
    ss >> std::skipws >> Position::st.rule50 >> Position::gamePly;

    // Convert from fullmove starting from 1 to gamePly starting from 0,
    // handle also common incorrect FEN with fullmove = 0.
    Position::gamePly = std::max(2 * (Position::gamePly - 1), 0) + (Position::sideToMove == BLACK);
}

Bitboard PinnedPieces(Color Us, Bitboard OurKingBB){
    Square OurKingSquare = pop_LSB(OurKingBB);
    Bitboard OpponentsSlidingBB = (Position::BitboardsByType[QUEEN] | Position::BitboardsByType[ROOK] | Position::BitboardsByType[BISHOP]) & Position::BitboardsByColor[!Us];
    Bitboard OnlyMyPiecesInBetween;
    Bitboard PinnedBB = 0ULL;
    Square enemySq;
    while (OpponentsSlidingBB){
        enemySq = pop_LSB(OpponentsSlidingBB);
        // Check own pieces in the way 
        // if there are enemy pieces, there is no pin
        OnlyMyPiecesInBetween = (BetweenBB[OurKingSquare][enemySq] & Position::BitboardsByColor[Us]) * ((BetweenBB[OurKingSquare][enemySq] & Position::BitboardsByColor[!Us])==0ULL);
        PinnedBB |= countBitsOn(OnlyMyPiecesInBetween) == 1 ? OnlyMyPiecesInBetween : 0ULL;
    }
    return PinnedBB;
}