#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE boardValidity
#endif

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "lut.hpp"
#include "def.hpp"
#include "attempt.hpp"

uint64_t occ[2];
BoardStatus bs;

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

BOOST_AUTO_TEST_SUITE(attempt)

    BOOST_AUTO_TEST_CASE(test_BoardStatusFromFEN)
    {
        LUT knight_lut = knight_position_lut();
        LUT wpawn_fw_lut = wpawn_straight_lut();
        LUT wpawn_doublefw_lut = wpawn_doublestraight_lut();
        LUT wpawn_cap_lut = wpawn_diagcapture_lut();
        LUT bpawn_fw_lut = bpawn_straight_lut();
        LUT bpawn_doublefw_lut = bpawn_doublestraight_lut();
        LUT bpawn_cap_lut = bpawn_diagcapture_lut();
        LUT king_lut =  king_position_lut();

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
        
    }

BOOST_AUTO_TEST_SUITE_END()