#include "eval.hpp"

std::string ee(Move theMove){
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


int EvalPosition(){
    Color whoMoves = Position::sideToMove;
    Color opponent_c = Color(!whoMoves);
    return ColorScoreFactor[whoMoves] * ( Position::st.PSTScore[whoMoves]*PST_CONVERSION_FACTOR + countBitsOn(pieces(whoMoves,PAWN))*PawnValue + Position::st.nonPawnMaterial[whoMoves] - Position::st.PSTScore[opponent_c]*PST_CONVERSION_FACTOR - countBitsOn(pieces(opponent_c,PAWN))*PawnValue - Position::st.nonPawnMaterial[opponent_c]);
}

ExtMove EvalExtMove(ExtMove& extmove){
    Move mv = extmove.move;
    
    Square from = mv_from(mv);
    Square to = mv_to(mv);
    MoveType mt = MoveType( mv & 0xC000);
    Piece P_from = Position::board[from];
    Piece P_to = Position::board[to];
    Color whoMoves = color_of(P_from);
    Color opponent_c = Color(!whoMoves);
    int score = 0;

    // the moves captures something
    // get the contribution of that piece
    // since it'd be score that gets removed from them
    // I might as well add it to US
    if(P_to != NO_PIECE){
        score += getPieceSquareTableValue(opponent_c, to, type_of(P_to)) * PST_CONVERSION_FACTOR;
        score += PieceValue[P_to];
    }
    //opponents score eval is now completed

    if(mt == PROMOTION){
        PieceType pt = PieceType(((mv >> 12) & (0x3))+2);
        // add PST of the new piece on "to", subtract the PST of the old PAWN on "from"
        score += (getPieceSquareTableValue(whoMoves, to, pt) - getPieceSquareTableValue(whoMoves, from, PAWN)) * PST_CONVERSION_FACTOR;
        // add piece value of new piece, remove that of the pawn
        score += PieceValue[pt] - PieceValue[PAWN];
    }
    else if(mt == ENPASSANT)
        // Add delta PST from -to for the PAWN
        score +=   (getPieceSquareTableValue(whoMoves, to, PAWN) - getPieceSquareTableValue(whoMoves, from, PAWN))* PST_CONVERSION_FACTOR;
        // PST and material count of the victim opponent pawn are already calculated in the first step
    
    else if( mt == CASTLING){
        // castle is queenside
        if( (to - from) == Castle_QueenSide_KingDelta){
            Square start = whoMoves == WHITE? White_Rook_QueenSide : Black_Rook_QueenSide;
            score += (getPieceSquareTableValue(whoMoves, to, KING) - getPieceSquareTableValue(whoMoves, from, KING))* PST_CONVERSION_FACTOR;
            score += (getPieceSquareTableValue(whoMoves, start + Castle_QueenSide_RookDelta, ROOK) - getPieceSquareTableValue(whoMoves, start, ROOK))* PST_CONVERSION_FACTOR;
        }
        // castle is kingside
        if( (to - from) == Castle_KingSide_KingDelta){
            Square start = whoMoves == WHITE? White_Rook_KingSide : Black_Rook_KingSide;
            score += (getPieceSquareTableValue(whoMoves, to, KING) - getPieceSquareTableValue(whoMoves, from, KING))* PST_CONVERSION_FACTOR;
            score += (getPieceSquareTableValue(whoMoves, start + Castle_KingSide_RookDelta, ROOK) - getPieceSquareTableValue(whoMoves, start, ROOK))* PST_CONVERSION_FACTOR;
        }
    }
    // Normal move
    else
        score += ( getPieceSquareTableValue(whoMoves, to, type_of(P_from)) - getPieceSquareTableValue(whoMoves, from, type_of(P_from)) )* PST_CONVERSION_FACTOR;

    score = ColorScoreFactor[whoMoves] * ( score + Position::st.PSTScore[whoMoves]*PST_CONVERSION_FACTOR + countBitsOn(pieces(whoMoves,PAWN))*PawnValue + Position::st.nonPawnMaterial[whoMoves] - Position::st.PSTScore[opponent_c]*PST_CONVERSION_FACTOR - countBitsOn(pieces(opponent_c,PAWN))*PawnValue - Position::st.nonPawnMaterial[opponent_c]);
    extmove.value = score;
    return extmove;
}


MoveList EvalMoveList(MoveList& mvList){
    for(int i = 0; i<mvList.size; i++){
        mvList.list[i] = EvalExtMove(mvList.list[i]);
    }
    return mvList;
}

bool black_sorter(ExtMove const& lhs, ExtMove const& rhs) {
    return lhs.value < rhs.value;
}
bool white_sorter(ExtMove const& lhs, ExtMove const& rhs) {
    return lhs.value > rhs.value;
}


ExtMove minmax(Color Us, int alpha, int beta, int depth, int maxdepth, int& counter, bool verbose){
    ExtMove bestMove,newMove;
    bestMove = Move(0);
    MoveList legal = generate_legal(Us);
    // this is either checkmate or stalemate
    if (legal.size == 0){
        
        if(Position::st.checkersBB == 0ULL)
            bestMove.value = 0; // stalemate
        else
            bestMove.value = ColorScoreFactor[!Us]*VALUE_INFINITE; // opponent did checkmate
        return bestMove;
    }
    legal = EvalMoveList(legal);
    StateInfo previousState;
    // uint64_t prevCount = count;
    
    previousState = Position::st;


    if(Us == WHITE){
        std::sort(&legal.list[0], &legal.list[legal.size], &white_sorter);
        if (depth == 0){
            counter +=legal.size;
            if(verbose)
                std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBest "<<ColorNames[Us]<< "move " << ee(legal.list[0].move) << " e:"<<legal.list[0].value<<std::endl;
            return legal.list[0];
        }
        bestMove.value = -VALUE_INFINITE;
        for(int i=0; i<legal.size; i++){
            if(verbose)
                std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tTrying " <<ColorNames[Us]<<" "<<ee(legal.list[i].move)<< " e:"<<legal.list[i].value<<std::endl;
            counter ++;
            MakeMove(legal.list[i].move);
            newMove = minmax(Color(!Us), alpha, beta, depth-1, maxdepth,counter, verbose);
            legal.list[i].value = newMove.value; // update eval of the move for iterative deepning
            *(Position::st.previous) = previousState;
            UndoMove(legal.list[i].move);
            if ( newMove.value >bestMove.value){
                bestMove = legal.list[i].move;
                bestMove.value = newMove.value;
            }
            if (bestMove.value > beta)
                break;
            alpha = std::max(alpha, bestMove.value);    
        }
        if(verbose)
            std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBEST IS "<<ee(bestMove)<<" e:"<<bestMove.value<<std::endl;
        return bestMove;
    }       
    
    
    else{
        std::sort(&legal.list[0], &legal.list[legal.size], &black_sorter);
        if (depth == 0){
            counter +=legal.size;
            if(verbose)
                std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBest "<<ColorNames[Us]<< "move " << ee(legal.list[0].move) << " e:"<<legal.list[0].value<<std::endl;
            return legal.list[0];
        }
        bestMove.value = VALUE_INFINITE;
        for(int i=0; i<legal.size; i++){
            if(verbose)
                std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tTrying " <<ColorNames[Us]<<" "<<ee(legal.list[i].move)<< " e:"<<legal.list[i].value<<std::endl;
            MakeMove(legal.list[i].move);
            newMove = minmax(Color(!Us), alpha, beta, depth-1, maxdepth, counter, verbose);
            legal.list[i].value = newMove.value; // update eval of the move for iterative deepning
            *(Position::st.previous) = previousState;
            UndoMove(legal.list[i].move);
            if ( newMove.value <bestMove.value){
                bestMove = legal.list[i].move;
                bestMove.value = newMove.value;
            }
            if (bestMove.value < alpha)
                break;
            beta  = std::min(beta , bestMove.value);    
        }
        if(verbose)
            std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBEST IS "<<ee(bestMove)<<" e:"<<bestMove.value<<std::endl;
        return bestMove;
    }
}