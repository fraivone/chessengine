#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE boardValidity
#endif

#include <boost/test/unit_test.hpp>
#include  <boost/test/tools/output_test_stream.hpp>
#include <iostream>
#include "lut.hpp"
#include "def.hpp"
#include "attempt.hpp"

uint64_t occ[2];
BoardStatus bs;
Moves PM_collector[2][nPieceTypes];

// fen -> (legal?,nextmove,enpassant,bk,bq,wk,wq)
std::unordered_map<std::string, std::array<uint8_t,7>> FENs = {
    {"3rkbnr/1p1bp3/1q1p3p/p5pQ/3n4/PPR5/5PPP/6K1 b - - 2 2", {1,0,0xff,0,0,0,0}},
    {"r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4", {1,0,0xff,1,1,1,1}},
    {"k6R/8/2N5/8/8/8/8/1R2K3 b - - 0 1", {1,0,0xff,0,0,0,0}},
    {"4r1k1/ppp3pp/2p5/8/5P2/8/PP3R2/R1Q2K1q w - - 2 34", {1,1,0xff,0,0,0,0}},
    {"k6R/P7/K7/8/8/8/8/8 b - - 0 1", {1,0,0xff,0,0,0,0}},
    {"kbK5/pp6/1P6/8/8/8/8/R7 w - -", {1,1,0xff,0,0,0,0}},
    {"3Rk3/5prp/p5b1/6B1/r6P/2P5/PP3P2/2K3R1 b - - 2 23", {1,0,0xff,0,0,0,0}},
    {"8/2k5/4R3/8/8/8/7r/3K3r w - - 0 1", {1,1,0xff,0,0,0,0}},
    {"1k6/P7/8/8/8/K7/8/8 b - - 0 1", {1,0,0xff,0,0,0,0}},
    {"4R3/2k5/4R3/8/8/4n3/8/3K3r w - - 0 1", {1,1,0xff,0,0,0,0}},
    {"8/8/8/8/3b4/3k4/2p5/2K5 w - - 1 1", {1,1,0xff,0,0,0,0}},
    {"8/8/8/8/3b4/3k4/p3p3/2K5 w - - 1 1", {1,1,0xff,0,0,0,0}},
    {"K7/8/8/8/8/8/5Q2/7k b - - 1 1", {1,0,0xff,0,0,0,0}},
    {"k7/1pp5/8/8/8/6q1/5r2/7K w - - 0 2", {1,1,0xff,0,0,0,0}},
    {"8/8/6pk/8/6P1/8/2q5/K7 w - - 0 1", {1,1,0xff,0,0,0,0}},
    {"8/2p5/1p6/pP1p2pp/P6k/8/5q2/7K w - - 0 2", {1,1,0xff,0,0,0,0}},
    {"k7/P7/1K6/8/8/8/8/8 b - - 0 1", {1,0,0xff,0,0,0,0}},
    {"8/1k6/8/8/3N1K2/8/8/8 b - - 0 4", {1,0,0xff,0,0,0,0}},
    {"kK6/pp6/1P6/8/8/8/8/R7 w - -", {0,1,0xff,0,0,0,0}},
    {"r1bq1b1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4", {0,0,0xff,1,1,1,1}}
    };

std::unordered_map<std::string,bool> FENMate = {
    {"3rkbnr/1p1bp3/1q1p3p/p5pQ/3n4/PPR5/5PPP/6K1 b - - 2 2", true},
    {"r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4", true},
    {"k6R/8/2N5/8/8/8/8/1R2K3 b - - 0 1", true},
    {"4r1k1/ppp3pp/2p5/8/5P2/8/PP3R2/R1Q2K1q w - - 2 34", true},
    {"k6R/P7/K7/8/8/8/8/8 b - - 0 1", true},
};

std::unordered_map<std::string, bool> FENCheck = {
    {"3Rk3/5prp/p5b1/6B1/r6P/2P5/PP3P2/2K3R1 b - - 2 23", true},
    {"8/2k5/4R3/8/8/8/7r/3K3r w - - 0 1", true},
    {"1k6/P7/8/8/8/K7/8/8 b - - 0 1", true},
    {"4R3/2k5/4R3/8/8/4n3/8/3K3r w - - 0 1", true},
};
std::unordered_map<std::string, bool> FENStaleMate = {
    {"8/8/8/8/3b4/3k4/2p5/2K5 w - - 1 1", false},
    {"8/8/8/8/3b4/3k4/p3p3/2K5 w - - 1 1", false},
    {"K7/8/8/8/8/8/5Q2/7k b - - 1 1", false},
    {"k7/1pp5/8/8/8/6q1/5r2/7K w - - 0 2", false},
    // {"8/8/6pk/8/6P1/8/2q5/K7 w - - 0 1", false}, // stalemate in 2 moves
    {"8/2p5/1p6/pP1p2pp/P6k/8/5q2/7K w - - 0 2", false},
    {"k7/P7/1K6/8/8/8/8/8 b - - 0 1", false},
    };

BOOST_AUTO_TEST_SUITE(attempt)

    BOOST_AUTO_TEST_CASE(test_BoardStatusFromFEN)
    {

        for (auto const& x : FENs){
            auto f = x.first;
            auto status = x.second;
            bs = BoardStatusFromFEN(f);
            BOOST_CHECK_EQUAL(BoardisLegal(bs.board, king_lut), status[0]);
            BOOST_CHECK_EQUAL(bs.whoMoves, status[1]);
            BOOST_CHECK_EQUAL(bs.enpassant_bit, status[2]);
            BOOST_CHECK_EQUAL(bs.castle_rights[0][0], status[3]);
            BOOST_CHECK_EQUAL(bs.castle_rights[0][1], status[4]);
            BOOST_CHECK_EQUAL(bs.castle_rights[1][0], status[5]);
            BOOST_CHECK_EQUAL(bs.castle_rights[1][1], status[6]);
        }
    }
    BOOST_AUTO_TEST_CASE(test_occufunctions){
        Reset_uint64_2Darray(occ);
        BOOST_CHECK_EQUAL(occ[1],0);
        BOOST_CHECK_EQUAL(occ[0],0);

        update_board_occupancy(occ, bs.board);
        BOOST_TEST_MESSAGE("AAAAAA");
        
    }

    BOOST_AUTO_TEST_CASE(test_isCheck){
        for (auto const& x : FENCheck){
            auto f = x.first;
            auto isCheck = x.second;
            bs = BoardStatusFromFEN(f);
            bs.check = ChainAssertCheck(bs.board, PM_collector, bs.whoMoves, bs.enpassant_bit);
            BOOST_CHECK_EQUAL(bs.check, isCheck);

        }
        for (auto const& x : FENStaleMate){
            auto f = x.first;
            auto isCheck = x.second;
            bs = BoardStatusFromFEN(f);
            bs.check = ChainAssertCheck(bs.board, PM_collector, bs.whoMoves, bs.enpassant_bit);
            BOOST_CHECK_EQUAL(bs.check, isCheck);
            BOOST_CHECK_EQUAL(ChainAssertDraw(bs), 1);
            
        }
    }
    
    BOOST_AUTO_TEST_CASE(test_validMoveKingChecked){
        Move newMove {33,40,whitePawn};
        update_board_occupancy(occ, bs.board);
        // Constructor with member initialization
        BOOST_CHECK_EQUAL(ValidateMove(bs.board, newMove, occ, bs.enpassant_bit),1);
        
    }
    BOOST_AUTO_TEST_CASE(test_validMoveEnpassant){
        bs = BoardStatusFromFEN("rnbqkbnr/p1p1pppp/8/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq d6 0 3");
        Move theMove {34,60,whitePawn}; // capture king
        update_board_occupancy(occ, bs.board);
        // Constructor with member initialization
        BOOST_CHECK_EQUAL(ValidateMove(bs.board, theMove, occ, bs.enpassant_bit),0);        
    }

    BOOST_AUTO_TEST_CASE(test_mate){
        // std::cerr << boost::unit_test::framework::current_test_case().p_name << std::endl;
        uint64_t bOcc[2] = {};
        uint64_t bOcc_noKing[2] = {};
        uint64_t landing_squares[2] = {};
        uint64_t landing_squares_throughKing[2] = {};
        uint64_t king_landing_squares[2] = {};
        for (auto const& x : FENMate){
            auto f = x.first;
            auto target_mate = x.second;
            bs = BoardStatusFromFEN(f);
               
            bs.check = ChainAssertCheck(bs.board,bOcc, bOcc_noKing, landing_squares, landing_squares_throughKing, king_landing_squares, PM_collector,bs.whoMoves,bs.enpassant_bit);
            bool mate = isMate(bs, PM_collector, bOcc, king_landing_squares, landing_squares_throughKing);
            BOOST_CHECK_EQUAL(mate, target_mate);
        }

    }

    BOOST_AUTO_TEST_CASE(test_search){
        bs = BoardStatusFromFEN("8/4R3/2P5/3kp3/n2b4/7r/2b1r2n/7K b - - 2 7");
        int max_depth = 2;
        for(int ply = 0; ply < 3; ply++){
        int count = 0;
        ScorenMove thisBest = minimax(bs.board, bs.whoMoves,-Mate_Score+1, Mate_Score-1, max_depth, count);
        std::cout<<"tested "<<count<<" moves.\n";
        std::cout<<convert_color[bs.whoMoves]<<" to play. The best has score "<<thisBest.first<<" and it is ";
        printMove(thisBest.second);
        std::cout<<"\n";
        

        bs.board = MakeMove(thisBest.second, bs.board);
        bs.whoMoves = !bs.whoMoves;
        }
        BOOST_CHECK_EQUAL(1,2);
    }
        

    BOOST_AUTO_TEST_CASE(test_insufficienctMaterial){
        bs = BoardStatusFromFEN("8/1k6/8/8/3N1K2/8/8/8 b - - 0 4");
        BOOST_CHECK_EQUAL(isDrawInsufficientMaterial(bs.board), 1);
    }



BOOST_AUTO_TEST_SUITE_END()