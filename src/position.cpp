#include <position.hpp>

namespace Position {
    Piece board[nCols*nRows];
    Bitboard BitboardsByType[PIECE_TYPE_NB];
    Bitboard BitboardsByColor[COLOR_NB];
    Color sideToMove;
    StateInfo st;
    int gamePly;
    Bitboard pinnedPieces[COLOR_NB];
    Square PinMap[COLOR_NB][nRows*nCols];
    StateInfo tempState;

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
        /// Initializing tempstate to null StateInfo
        std::memset(&tempState, 0, sizeof(StateInfo));
        st.previous = &tempState;
        gamePly = 0;
    }

    void UpdatePosition(){
        // Update pinnedpieces
        Position::pinnedPieces[BLACK] = PinnedPieces(BLACK, (pieces(BLACK,KING)));
        Position::pinnedPieces[WHITE] = PinnedPieces(WHITE, (pieces(WHITE,KING)));
        // update checkers
        Position::st.checkersBB = Checkers(Position::sideToMove, pieces(Position::sideToMove,KING));

    }
}


void init_position(std::string FEN){
    Position::init();
    // set boards and bitboards from FEN
    setBBFromFEN(FEN);
    Position::UpdatePosition();
    // calc and store material count
    CalculateMaterial();
    // calc and store PST score
    CalculatePSTScore();
}

void put_piece(Square square, Piece PP){
    Position::board[square] = PP;
    Position::BitboardsByType[ALL_PIECES] |= 1ULL << square;
    Position::BitboardsByType[type_of(PP)] |= 1ULL << square;
    Position::BitboardsByColor[color_of(PP)] |= 1ULL << square;
}
void remove_piece(Square square,Piece PP){
    Position::board[square] = NO_PIECE;
    clear_bit(Position::BitboardsByType[ALL_PIECES],square);
    clear_bit(Position::BitboardsByType[type_of(PP)],square);
    clear_bit(Position::BitboardsByColor[color_of(PP)],square);
}
void remove_piece(Square square){
    Position::board[square] = NO_PIECE;
    for(int i = 0; i<PIECE_TYPE_NB; i++)
        clear_bit(Position::BitboardsByType[i], square);
    for(int i = 0; i<COLOR_NB; i++)
        clear_bit(Position::BitboardsByColor[i], square);
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
    clear_bit(Position::BitboardsByType[ALL_PIECES], startsquare);
    clear_bit(Position::BitboardsByType[type_of(PP)], startsquare);
    // add
    Position::BitboardsByType[type_of(PP)] |= 1ULL << endsquare;
    Position::BitboardsByType[ALL_PIECES] |= 1ULL << endsquare;

    // remove
    clear_bit(Position::BitboardsByColor[color_of(PP)],startsquare);
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
    Bitboard OpponentsSlidingBB = pieces(Color(!Us), QUEEN,ROOK,BISHOP);
    Bitboard OnlyMyPiecesInBetween;
    Bitboard PinnedBB = 0ULL;
    Square enemySq;
    std::fill_n(Position::PinMap[Us], nCols*nRows, ENPSNT_UNAVAILABLE);
    while (OpponentsSlidingBB){
        enemySq = pop_LSB(OpponentsSlidingBB);
        PieceType pt = type_of(Position::board[enemySq]);
        // bishops only count if on the same diagonal
        if( (pt == BISHOP) && !same_diago(OurKingSquare,enemySq))
            continue;
        // rooks only count if on the same row, col
        if( (pt == ROOK) && !(same_row(OurKingSquare,enemySq) | same_col(OurKingSquare,enemySq)) )
            continue;

        // Check own pieces in the way 
        // if there are enemy pieces, there is no pin
        OnlyMyPiecesInBetween = (BetweenBB[OurKingSquare][enemySq] & Position::BitboardsByColor[Us]) * ((BetweenBB[OurKingSquare][enemySq] & Position::BitboardsByColor[!Us])==0ULL);
        // if there is only 1 of my pieces in betwee, it is a pin
        if (countBitsOn(OnlyMyPiecesInBetween)==1){
            PinnedBB |= OnlyMyPiecesInBetween;
            Position::st.pinners[!Us] |= make_bitboard(enemySq);
            Position::PinMap[Us][pop_LSB(OnlyMyPiecesInBetween)] = enemySq;
        }
    }
    return PinnedBB;
}


/// returns the FEN of the current position
std::string MakeFEN(){
    std::string output = "";
    Square current = 56;
    char pieceChar;
    int emptyAdjacent = 0;
    while(1){
        // std::cout<<"current square "<<+current<<"   "<<output<<"Position::board[current] "<<PieceTypeNames[type_of(Position::board[current])]<<std::endl;
        if(Position::board[current]==NO_PIECE)
            emptyAdjacent++;
        else{
            pieceChar = PieceCharMap[color_of(Position::board[current])][type_of(Position::board[current])];
            if(emptyAdjacent!=0)
                output += std::to_string(emptyAdjacent);
            output += pieceChar;
            emptyAdjacent = 0;
            
        }

        // update current to point to the next square
        // easy case, we are finished
        if(current == 7){
            if(emptyAdjacent!=0)
                output += std::to_string(emptyAdjacent);
            break;
        }
        // stay on the same row
        else if( (current+1)%8!=0)
            current++;
        else{
            // we have to change row
            // let's dump some chars
            if(emptyAdjacent!=0)
                output += std::to_string(emptyAdjacent);
            emptyAdjacent = 0;
            current = ((current/8) -1)*8;
            output += "/";
        }
    }
    
    output += ' ';
    // who moves
    output += Position::sideToMove == WHITE? 'w':'b';
    output += ' ';
    // castl rigths
    output += castlingString[Position::st.castlingRights];
    output += ' ';
    // enpassant square
    if(Position::st.epSquare==ENPSNT_UNAVAILABLE)
        output += '-';
    else{
        Square row = '1' + Position::st.epSquare/nRows;
        Square col = 'a' + Position::st.epSquare%nCols;
        output += col;
        output += row;
    }
    output += ' ';
    output += std::to_string(Position::st.rule50);
    output += ' ';
    output += std::to_string(1 + Position::gamePly/2);

    return output;

}

void UpdateMaterialCount(Color color,int v ){
    Position::st.nonPawnMaterial[color] += v;
}
void CalculateMaterial(){
    PieceType pt = KNIGHT;
    // update values for KNIGHTs, BISHOPs, ROOKs, QUEENs
    while(pt < KING){
        UpdateMaterialCount(WHITE,countBitsOn(pieces(WHITE,pt))*PieceValue[pt]);
        UpdateMaterialCount(BLACK,countBitsOn(pieces(BLACK,pt))*PieceValue[pt]);
        pt = PieceType(int(pt) + 1);
    }
}

void AddPSTScore(Color Us, Square sq, PieceType pt){
    Position::st.PSTScore[Us] += getPieceSquareTableValue(Us, sq, pt);
}
void SubtractPSTScore(Color Us, Square sq, PieceType pt){
    Position::st.PSTScore[Us] -= getPieceSquareTableValue(Us, sq, pt);
}

void AddPSTScore(Color c,int value ){
    Position::st.PSTScore[c] += value;
}

void CalculatePSTScore(){
    PieceType pt;
    Color c;
    int value = 0;
    // update values for KNIGHTs, BISHOPs, ROOKs, QUEENs
    for(Square sq=0; sq<nRows*nCols; sq++){
        pt = type_of(Position::board[sq]);
        if(pt==NO_PIECE_TYPE)
            continue;

        c = color_of(Position::board[sq]);
        AddPSTScore(c,sq,pt);
    }
}