//LegalMoveMaker.cpp
#include <iostream>
# include <bitset>
#include "chessboard.hpp"
#include "def.hpp"



using chessboard::ChessBoard;

// generic method to evaluate a check
// check if the King of color c is in check
// based on the opponent landings
bool ChessBoard::_isCheck(Color c, uint64_t opponent_landings){
    Piece theMovingKing = pieces_array[ColorlessKing|c];
    if ((_board[theMovingKing] & opponent_landings)!=0)
            return true;
    return false;
}
// to be fixed
bool ChessBoard::_isMate(){
    Moves blkMoves;
    if (!isCheck)  return false;
    else{
        uint64_t evading_squares;
        bool _isMate = false;
        // legal evading squares are not occupied by own pieces, not in check, reachable by the king
        evading_squares = king_landingsquares[board_turn]  & ~(pieces_landingsquares_throughKing[!board_turn] | king_landingsquares[!board_turn]) & ~board_occupancy[board_turn];
        if ( !evading_squares ){
            // check for blocking moves
            Moves blocking_moves = _BlockingMoves();
            // if no blocking moves available --> mate
            if (blocking_moves.size() == 0)
                return true;
            }
        }
    return false;
}

bool ChessBoard::_isStaleMate(){
    if (isCheck)  return false;
    else{
        uint64_t legal_king_moves;
        uint64_t legal_pieces_moves;
        bool _isMate = false;
        legal_king_moves = king_landingsquares[board_turn]  & ~(pieces_landingsquares_throughKing[!board_turn] | king_landingsquares[!board_turn]) & ~board_occupancy[board_turn];
        legal_pieces_moves = pieces_landingsquares[board_turn] & ~board_occupancy[board_turn];
        if ( !(legal_king_moves) )
            return true;
        }
    return false;
}


bool ChessBoard::_isDrawInsufficientMaterial(){
    // from https://en.wikipedia.org/wiki/Draw_(chess)
    int pawns_left = countBitsOn(_board[whitePawn]) + countBitsOn(_board[blackPawn]);
    int rooks_left = countBitsOn(_board[whiteRook]) + countBitsOn(_board[blackRook]);
    int queens_left = countBitsOn(_board[whiteQueen]) + countBitsOn(_board[blackQueen]);
    if (pawns_left != 0 | rooks_left != 0 | queens_left != 0) //A king + any(pawn, rook, queen) is not draw
        return false;
    
    int wbishops_left = countBitsOn(_board[whiteBishop]);
    int bbishops_left = countBitsOn(_board[blackBishop]);
    int wknights_left = countBitsOn(_board[whiteKnight]);
    int bknights_left = countBitsOn(_board[blackKnight]);
    
    if (wbishops_left + bbishops_left + wknights_left + bknights_left == 0) // king vs king
        return true;

    if ( wbishops_left == 1 & (bbishops_left  + wknights_left + bknights_left) == 0) // king and bishop versus king
        return true;
    if ( bbishops_left == 1 & (wbishops_left  + wknights_left + bknights_left) == 0) // king and bishop versus king
        return true;
    if ( wknights_left == 1 & (bbishops_left  + wbishops_left + bknights_left) == 0) // king and knight versus king
        return true;
    if ( bknights_left == 1 & (wbishops_left  + wknights_left + bbishops_left) == 0) // king and knight versus king
        return true;

    // TODO king and bishop versus king and bishop with the bishops on the same color.

    return false;

}
bool ChessBoard::_isDrawForRepetition(){
    return false; // to be implemented
}
bool ChessBoard::_isDrawFor50MovesRule(){
    return false; // to be implemented
}

bool ChessBoard::_ValidateMove(Move &m){
    if (m.isChecked == true)
        return m.isValid;
    else{
        m.isChecked = true;
        Piece theOpponentKing = pieces_array[ColorlessKing|!m.piece.color];
        //The move would capture the opponent king
        //Illegal and most likely previous moves were illegal too
        if (( _board[theOpponentKing] & (1ULL << m.final_bit)) )
            return false;

        // move ends on its onw pieces
        if ( (1ULL << m.final_bit) & board_occupancy[m.piece.color])
            return false;

        //check whether the move leaves its own king in check        
        
        nextMove_board = _board;
        nextMove_board = _MakeMove(m,nextMove_board);
        bool MoveLeavesCheck = _ChainAssertCheck(nextMove_board, 
                                                 nextMove_board_occupancy, 
                                                 nextMove_board_occupancy_noKing, 
                                                 nextMove_pieces_landingsquares, 
                                                 nextMove_pieces_landingsquares_throughKing,
                                                 nextMove_king_landingsquares,
                                                 nextMove_PM_collector,
                                                 m.piece.color);
        // despite having just moved, m.piece.color is still in check
        // thereofore this move isn't legal
        if (MoveLeavesCheck){
            return false;
    }

    // TODO
    //if (piece is pinned check with ignore square?) 
    m.isValid = true;
    return true;
    }
}

// Move goes on the board and board is updated
// board_turn stays untouched 
Board ChessBoard::_MakeMove(Move &m, Board theBoard){
    Color thisColorMoves = m.piece.color;
    
    // the move captures something?
    // loop over the opponent pieces
    // check if the move captures an opponent piece
    // if so, update the board and the move
    for (Piece pp : pieces_array){
        if (pp.color != !thisColorMoves) continue; 
        if (pp.piece_type == KING) continue; // shouldn't happen if the move is properly validated valid
        
        if( get_bit(theBoard[pp], m.final_bit)){
            theBoard[pp] = clear_bit(theBoard[pp],m.final_bit);
            m.captured_piece = pp;
            break;
        }
    }
    theBoard[m.piece] = clear_bit( theBoard[m.piece], m.initial_bit); // remove thiscolor piece
    theBoard[m.piece] = set_bit(theBoard[m.piece],m.final_bit);

    return theBoard;
}

Board ChessBoard::_UndoMove(Move &m, Board theBoard){
    if( m.captured_piece.piece_type != NOTHING){
        theBoard[m.captured_piece] = set_bit(theBoard[m.captured_piece],m.final_bit);
        m.captured_piece = nullPiece;
    }
    theBoard[m.piece] = clear_bit(theBoard[m.piece],m.final_bit);
    theBoard[m.piece] = set_bit(theBoard[m.piece],m.initial_bit);

    return theBoard;
}

// legalizes valid moves that block a check
Moves ChessBoard::_BlockingMoves(){
    Moves blockingMoves;
    int moves_checked = 0;
    // Blocking Moves done by other than king
    for (PieceType pt : piecetypes_array){
        if (pt == KING ) continue; 
        for(Move mv : PM_collector[board_turn][pt]){
            // std::cout<<"Checked "<<moves_checked<<" moves"<<std::endl;
            moves_checked++;
            // printMove(mv);
            if(_ValidateMove(mv)){
                blockingMoves.push_back(mv);
            }
        }
    }
    return blockingMoves;
}


bool ChessBoard::PublicValidate(Move &theMove){
    return _ValidateMove(theMove);
}
// Move ChessBoard::PublicMove(Move theMove){
//     return _MakeMove(theMove);
// }
// Move ChessBoard::PublicUndo(Move theMove){
//     return _UndoMove(theMove);
// }
Moves ChessBoard::PublicBlocking(){
    return _BlockingMoves();
}

