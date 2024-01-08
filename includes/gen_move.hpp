/** @file */
#pragma once
#include <algorithm>
#include <position.hpp>
#include <lut.hpp>
#include <magic.hpp>



enum GenType {
    CAPTURES,
    EVASIONS,
    LEGAL
};

/// structure to hold a move and its score(?)
struct ExtMove {
    Move move;
    int  value;

    operator Move() const { return move; }
    void operator=(Move m) { move = m; }

    // Inhibit unwanted implicit conversions to Move
    // with an ambiguity that yields to a compile error.
    operator float() const = delete;
};

inline Square extmove_from(ExtMove extmv){return mv_from(extmv.move);}

/// Structure to store the ExtMove. Contains a the list 
/// itself as an array and the size. The size is different from MAX SIZE which is set to MAX MOVES
struct MoveList{
    ExtMove list[MAX_MOVES];
    int size = 0;
    // Append a move to this move list
    void Add(Move s) {list[size] = s; size++;}
    // Append a ExtMove to this move list
    void Add(ExtMove s) {list[size] = s; size++;}
    void Append(MoveList s) {for(int i=0; i<s.size; i++){list[size] = s.list[i];size++;}}
    // pop element from this move list
    void Pop(int index) {if(index<size){std::move(list + index + 1, list + size, list + index );size--;}}
    void Clear() {size = 0;}
};


/// Method to help promoting a pawn a given square to another to all possible pieces
/// taken from stockfish, simplified 
MoveList make_promotions(MoveList& moveList,Square from, Square to);

// template function to work with different piece types
// it returns a move as defined in types. 
// for pawns I need to know if I en passant is allowed, what is the occupancy of the enemy, what is my occupancy
// for kings and knights I need to know what is the occupancy of the enemy, what is my occupancy
// for queens, bishops and rooks I need to know what is the occupancy of the enemy, what is my occupancy

/// Pawns all pseudomoves including captures, fw, 2fw, enpassant and promotions
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contains the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
MoveList PawnAnyMoves(MoveList& moveList, Color c, Square s);
/// Pawns all pseudomoves including captures, fw, 2fw and enpassant
/// takes into account the current board status. Pseudomoves only exist when the starting square (s) actually contain the pawn
/// CAN capture opponent king 
/// not checking for checks or mate
MoveList PawnAnyMoves(MoveList& moveList, Square s);

/// Pawns all pseudomoves that allow evasion from check
/// includes blocking or capture of the opponent checker
/// TODO Can I skip the check on ONLY 1 CHECKERS and NonEmpty Blocker in this function?
MoveList PawnAnyEvasionsMoves(MoveList& moveList, Square s, Bitboard Checkers, Bitboard possibleBlockersBB);
MoveList PawnAnyEvasionsMoves(MoveList& moveList, Color c, Square s, Bitboard Checkers, Bitboard possibleBlockersBB);

/// Convenient method to OR all attacked squares in a movelist 
/// to a bitboard so that it's possible to easily test the move generator.
/// Used in boostTester
Bitboard Attacked2Bitboard(const MoveList moveList);
/// Convenient method to OR all starting squares currently attacking
Bitboard Attackers2Bitboard(const MoveList moveList);
/// Convenient method to OR all starting squares currently attacking target
Bitboard Attackers2Bitboard(const MoveList moveList, Bitboard target);

/// given a pieceType generate pseudomoves
/// requires magic bitboards to be init
MoveList generate_pseudomoves(MoveList&,Color, Square, const PieceType);

/// generate pseudomoves that allow to escape a check
/// includes evading by blocking or capture opponents checker. 
/// Pinned pieces are taken into account
MoveList generate_evasion_moves(MoveList& moveList,Color Us, Square sq, const PieceType pt, Bitboard Checkers, Bitboard BlockersBB);

/// generate all pseudomoves in this postion
MoveList generate_all(MoveList&,Color);

/// generate all pseudomoves for us
MoveList generate_all(MoveList& moveList,Color Us, const PieceType pt);

// LEGACY
// MoveList generate_evasions(MoveList& OurPseudoMoves, Bitboard Checkers, Color Us);
// /// Among the previously calcuted pseudomoves, 
// /// only return the ones that allow to block the checkers or to capture checker
// MoveList generate_BlockingMoves(Color Us, MoveList& OurPseudoMoves, Bitboard Checkers);

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers arbitrary number of piece types
template<typename... PieceTypes>
inline MoveList generate_all(MoveList& moveList,Color Us, PieceType pt, PieceTypes... pts) {
    moveList = generate_all(moveList, Us, pt);
    moveList = generate_all(moveList, Us, pts...);
    return moveList;
}

/// generate all evasion moves in this postion
MoveList generate_allevasion_moves(MoveList&,Color,Bitboard, Bitboard);
/// generate all evasion moves in this postion for PieceType
MoveList generate_allevasion_moves(MoveList&,Color, const PieceType,Bitboard, Bitboard);

/// overloaded function to get bitboard for a certain piece type(s), color, piece types and color.
/// Considers arbitrary number of piece types
template<typename... PieceTypes>
inline MoveList generate_allevasion_moves(MoveList& moveList,Color Us, PieceType pt,Bitboard Checkers, Bitboard BlockersBB, PieceTypes... pts) {
    moveList = generate_allevasion_moves(moveList, Us, pt, Checkers, BlockersBB);
    moveList = generate_allevasion_moves(moveList, Us, Checkers, BlockersBB, pts...);
    return moveList;
}

/// Given the move list for color THEM, check if Us is in check
/// returns a bitboard having the bits of checkers set
Bitboard Checkers(MoveList OpponentMoveList, Bitboard OurKingBitboard);
/// Checks if the Color "Us" is in check
/// returns a bitboard having the bits of checker pieces set
Bitboard Checkers(Color Us);
/// Checks if the Color "Us" is in check
/// returns a bitboard having the bits of checker pieces set
Bitboard Checkers(Color Us, Bitboard OurKingBB);

/// Calculate if there are and which squares allow to block
/// a check given by checkers. It has to be complemented 
/// with pseudomoves of non-pinned pieces to provide 
/// blocking moves
Bitboard PossibleBlockersBB(Color Us);

/// generate only legal moves
MoveList generate_legal(Color Us);