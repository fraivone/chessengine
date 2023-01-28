#include "eval.hpp"

std::string mvhuman(Move theMove){
    std::string out;
    if(theMove == MOVE_NONE)
        out = "0000";
    else{
    Square from = mv_from(theMove);
    Square to = mv_to(theMove);
    
    out += char('a'+from%nCols);
    out += char('1'+from/nRows);
    out += char('a'+to%nCols);
    out += char('1'+to/nRows);

    MoveType mt = MoveType( theMove & 0xC000);
    PieceType pt = PieceType(((theMove >> 12) & (0x3))+2);
    if(mt==PROMOTION)
        out += PieceCharMap[BLACK][pt];
    }
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
    Hashkey zob;
    for(int i = 0; i<mvList.size; i++){
        // MakeMove(mvList.list[i].move);
        // zob = Position::st.ZobristHash;
        // UndoMove(mvList.list[i].move);
        // // the evaluation of this move can be taken from the transp table
        // if(HashTables::tableMatch(zob))
        //     mvList.list[i].value = HashTables::table[zob%TABLE_SIZE].eval();
        
        // // otherwise it's the material evaluation
        // else
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


ExtMove minmax(Color Us, int alpha, int beta, int depth, int maxdepth, uint64_t& counter, bool verbose){
    Hashkey zob = Position::st.ZobristHash;
    int index = zob % TABLE_SIZE;
    ExtMove bestMove,bestGuess;
    bestMove = Move(0);
    bestGuess = Move(0);
    int alphaOrig = alpha;
    int alphaBeta = beta;
    
    // this is draw by repetition
    // don't store it in the transposition table
    // as it is not always a draw
    // but depends on the game history
    if(Position::st.repetition >=3){
        bestMove.value = 0;
        return bestMove;
    }

    // this position contains repetitions
    // better to clean the transposition table
    // TODO CHECKME

    // check for useful info in the transposition table
    // only if this position is not a repetition
    if( HashTables::tableMatch(zob) & (Position::st.repetition == 0)){
        // sufficient depth
        if(HashTables::table[index].depth() >= depth){
            if(HashTables::table[index].scoretype() == EXACT){
                bestMove = HashTables::table[index].move();
                bestMove.value = HashTables::table[index].eval();
                // check for draw by repetition
                MakeMove(bestMove.move);
                // this move would lead to draw by repetition
                if(Position::st.repetition >=3){
                    UndoMove(bestMove.move);
                }
                else{
                    UndoMove(bestMove.move);
                    return bestMove;
                }

            }
            // Not all move in this position where searched
            // all we know is that the score is at least "eval()"
            // which means updating alpha if possible
            else if(HashTables::table[index].scoretype() == P_BETA){
                alpha = std::max(alpha, HashTables::table[index].eval());
            }
            // Not all move in this position where searched
            // all we know is that the score is no more than "eval()"
            // which means updating beta if possible
            else if(HashTables::table[index].scoretype() == P_ALPHA){
                beta = std::min(beta, HashTables::table[index].eval());
            }

            // BUGGY PART
            // if (alpha >= beta){
            //     bestMove = HashTables::table[index].move();
            //     bestMove.value = HashTables::table[index].eval();
            //     // std::cout<<"Alpha is greater than beta on. Best move is "<<mvhuman(bestMove)<<" ev "<<bestMove.value<<std::endl;
            //     return bestMove;
            // }
        }
        
        else{
            // Insufficient depth
            // however this move can be used for move ordering
            // so that we can start searching from this one
            bestGuess = HashTables::table[index].move();
            bestGuess.value = HashTables::table[index].eval();
        }
    }
    
    MoveList legal = generate_legal(Us);
    
    // this is either checkmate or stalemate
    // this referes to the move previously mate
    // so I won't store anything in the Transp Table
    if (legal.size == 0){    
        if(Position::st.checkersBB == 0ULL)
            bestMove.value = 0; // stalemate
        else{
            int mateIn = maxdepth - depth;
            bestMove.value = ColorScoreFactor[!Us]*(VALUE_MATE_0 - (mateIn << 16)); // opponent did checkmate in mateIn plys
        }
        return bestMove;
    }
    legal = EvalMoveList(legal);
    StateInfo previousState;
    // uint64_t prevCount = count;
    
    previousState = Position::st;

    // white to play
    if(Us == WHITE){
        std::sort(&legal.list[0], &legal.list[legal.size], &white_sorter);
        if(bestGuess!=MOVE_NONE)
            legal.Add(bestGuess,0);
        if (depth == 0){
            counter +=legal.size;
            // if(verbose)
                //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBest "<<ColorNames[Us]<< "move " << mvhuman(legal.list[0].move) << " e:"<<legal.list[0].value<<std::endl;
            return legal.list[0];
        }
        bestMove.value = -VALUE_INFINITE;
        for(int i=0; i<legal.size; i++){
            // if(verbose)
                //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tTrying " <<ColorNames[Us]<<" "<<mvhuman(legal.list[i].move)<< " e:"<<legal.list[i].value<<std::endl;
            counter ++;
            MakeMove(legal.list[i].move);
            legal.list[i].value = minmax(Color(!Us), alpha, beta, depth-1, maxdepth,counter, verbose).value;

            *(Position::st.previous) = previousState;
            UndoMove(legal.list[i].move);
            if ( legal.list[i].value >bestMove.value){
                bestMove = legal.list[i].move;
                bestMove.value = legal.list[i].value;
            }
            
            // Searching as white, I found a move that
            // scores more than the best move my father BLACK
            // has found. There is no need to keep searching
            // My father node BLACK will prune me as node
            if (bestMove.value > beta)
                break;
            alpha = std::max(alpha, bestMove.value);    
            // if(((maxdepth - depth) == 0) & verbose)
                //std::cout<<mvhuman(legal.list[i])<<" evaluated "<<legal.list[i].value<< " at depth "<<depth<<"\tBest is "<<mvhuman(bestMove)<<" "<<bestMove.value<<"\tLoadfactor "<<float(HashTables::tableLoadFactor)/TABLE_SIZE<<std::endl;
        } // searched all moves

        if (bestMove.value > beta)
            HashTables::addToTable(zob, bestMove.move, depth, bestMove.value, P_BETA);
        else
            HashTables::addToTable(zob, bestMove.move, depth, bestMove.value, EXACT);


        // if(verbose)
            //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBEST IS "<<mvhuman(bestMove)<<" e:"<<bestMove.value<<std::endl;
        return bestMove;
    }       
    
    // black to play
    else{
        std::sort(&legal.list[0], &legal.list[legal.size], &black_sorter);
        if(bestGuess!=MOVE_NONE)
            legal.Add(bestGuess,0);
        if (depth == 0){
            counter +=legal.size;
            // if(verbose)
                //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBest "<<ColorNames[Us]<< "move " << mvhuman(legal.list[0].move) << " e:"<<legal.list[0].value<<std::endl;
            return legal.list[0];
        }
        bestMove.value = VALUE_INFINITE;
        for(int i=0; i<legal.size; i++){
            // if(verbose)
                //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tTrying " <<ColorNames[Us]<<" "<<mvhuman(legal.list[i].move)<< " e:"<<legal.list[i].value<<std::endl;
            MakeMove(legal.list[i].move);
            legal.list[i].value = minmax(Color(!Us), alpha, beta, depth-1, maxdepth, counter, verbose).value;
            
            *(Position::st.previous) = previousState;
            UndoMove(legal.list[i].move);
            if ( legal.list[i].value <bestMove.value){
                bestMove = legal.list[i].move;
                bestMove.value = legal.list[i].value;
            }

            // Searching as black, I found a move that
            // scores less than the best move my father node WHITE
            // has found. There is no need to keep searching
            // My father node WHITE will prune me as node
            if (bestMove.value < alpha)
                break;
            beta  = std::min(beta , bestMove.value);    
            // if(((maxdepth - depth) == 0) & verbose)
                //std::cout<<mvhuman(legal.list[i])<<" evaluated "<<legal.list[i].value<< " at depth "<<depth<<"\tBest is "<<mvhuman(bestMove)<<" "<<bestMove.value<<"\tLoadfactor "<<float(HashTables::tableLoadFactor)/TABLE_SIZE<<std::endl;
        } // searched all moves
        
        if (bestMove.value < alpha)
            HashTables::addToTable(zob, bestMove.move, depth, bestMove.value, P_ALPHA);
        else
            HashTables::addToTable(zob, bestMove.move, depth, bestMove.value, EXACT);
        
        // if(verbose)
            //std::cout<<std::string(maxdepth-depth, '\t')<<"Depth "<<+depth<<"\tBEST IS "<<mvhuman(bestMove)<<" e:"<<bestMove.value<<std::endl;
        return bestMove;
    }
}


ExtMove iterativeDeepening(Color Us, int depth, uint64_t& counter, bool verbose){
    uint8_t d = 1;
    ExtMove best;
    while(d<=depth){
        best = minmax(Us, -10000000, +10000000, d,d, counter, verbose);
        d++;
    }
    return best;
}
