/** @file */
#pragma once
#include "magic.hpp"

/// handle methods for PRN to work with the Zobrist hashing
/// Zobrist hash has to (almost) uniquely identify a position
/// So we need all pieces positions + enpassant + castling rights + whomoves
/// basically a FEN without the clock numbers
namespace HashTables{
    /// pseudo random numbers for each piece (color+type), each square
    extern Hashkey PRN_pieces[PIECE_NB][nRows*nCols];
    /// pseudo random numbers for each column 
    extern Hashkey PRN_enpassant[nCols];
    /// pseudo random numbers for each of the
    ///  2^4 = 16 combinations of castling right statuses
    extern Hashkey PRN_castling[CASTLING_RIGHT_NB];
    /// pseudo random number for side to play  
    extern Hashkey whomoves;
    // Stockfish uses another PRN for the pawns
    // in order to tell what is the pawn strucutre
    // For now I don't need that

    /// mask to get the sign from the SearcSummary
    /// corresponds to the 28th bit
    const int SignMask = 0x10000000;
    /// mask to get the evaluation from the SearcSummary
    /// corresponds to the bits [8,27]
    const int EvaluationMask = 0xFFFFF00;
    /// mask to get the scoretype from the SearcSummary
    /// corresponds to the bits [6,7]
    const int ScoreTypeMask = 0xC0;
    /// mask to get the depth from the SearcSummary
    /// corresponds to the bits [0,5]
    const int DepthMask = 0x3F;

    /// mask to get the first 48 MostSignificantBit of a uint64
    const uint64_t MaskZobrist48MSB1 = 0xFFFFFFFFFFFF0000;

    /// struct to hold 48 bits
    struct MS48B {

        uint64_t value:48;
        MS48B(uint64_t v){
            value = (v & MaskZobrist48MSB1) >> 16;
        }
        MS48B(){
            value = 0;
        }
    } __attribute__((packed));
    
    /// single entry in of a transposition table
    /// It is packed so it won't get padded
    /// It should be 12bytes
    #pragma pack(push, 1)
    struct TableEntry {
        
        MS48B z;
        Move BestMove;
        SearchSummary DSE;
        
        
        uint64_t ms48b_zobrist() const {return z.value;}
        Move move() const {return BestMove;}
        SearchSummary ssummary() const {return DSE;}

        bool sign() const {return bool( (DSE & SignMask) >> 28 );}
        uint8_t depth() const {return uint8_t(DSE & DepthMask);}
        int eval() const {return int( (sign()?1:-1) *  ( (DSE & EvaluationMask) >> 8)  );}
        ScoreType scoretype() const {return ScoreType( (DSE & ScoreTypeMask) >> 6 );}
        
        


        // empty constructor
        TableEntry() : z(0ULL), BestMove(MOVE_NONE), DSE(0) {}
        // constructor
        TableEntry(Hashkey ZobristHash, Move theMove, uint8_t depth, int evaluation, ScoreType st) : z(ZobristHash),BestMove(theMove),DSE( ( bool(evaluation > 0)<<28) | ((abs(evaluation)<<8) &  EvaluationMask) | ((st<<6)&ScoreTypeMask) | (depth & DepthMask) ){}
    };
    #pragma pack(pop)
    extern std::vector<TableEntry> table;
    // number of entries in the table
    extern int tableLoadFactor;

    /// init function to be called at startup 
    /// to initialize the hash variables with PRNs
    void init();

    void init_transposition_table();

    /// Checks whether this zobrist has a key in the table
    /// If true it can be either: 1. Same position 2. Different position Collision
    bool tableKey(Hashkey zob);
    
    /// Checks whether this position is stored in the table
    /// Match on:  Zobrist key (16LSB Zobrist) && Zobrist hash (48LSB Zobrist)
    bool tableMatch(Hashkey zob);
    
    /// Checks whether this position is stored in the table
    /// at a depth greater or equal than depth
    bool tableMatch(Hashkey zob, int depth);
    
    /// Add a table entry to the transposition table
    void addToTable(Hashkey ZobristHash, Move theMove, int depth, int evaluation, ScoreType st);
    
    
}
