#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE boardValidity
#endif

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "print.hpp"
#include "types.hpp"
#include "position.hpp"
#include "position_eval.hpp"
#include "magic.hpp"

const int arrayLenght = 34;
std::string FEN_Array[] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                           "rnbqkbnr/p1p1pppp/8/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq b6 0 3",
                           "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 11",
                           "4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 7 19",
                           "rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14",
                           "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14",
                           "r3r1k1/2p2ppp/p1p1bn2/8/1q2P3/2NPQN2/PPP3PP/R4RK1 b - - 2 15",
                           "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
                           "r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",
                           "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
                           "2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
                           "r1bq1r1k/b1p1npp1/p2p3p/1p6/3PP3/1B2NN2/PP3PPP/R2Q1RK1 w - - 1 16",
                           "3r1rk1/p5pp/bpp1pp2/8/q1PP1P2/b3P3/P2NQRPP/1R2B1K1 b - - 6 22",
                           "r1q2rk1/2p1bppp/2Pp4/p6b/Q1PNp3/4B3/PP1R1PPP/2K4R w - - 2 18",
                           "4k2r/1pb2ppp/1p2p3/1R1p4/3P4/2r1PN2/P4PPP/1R4K1 b - - 3 22",
                           "3q2k1/pb3p1p/4pbp1/2r5/PpN2N2/1P2P2P/5PP1/Q2R2K1 b - - 4 26",
                           "6k1/6p1/6Pp/ppp5/3pn2P/1P3K2/1PP2P2/3N4 b - - 0 1",
                           "3b4/5kp1/1p1p1p1p/pP1PpP1P/P1P1P3/3KN3/8/8 w - - 0 1",
                           "8/6pk/1p6/8/PP3p1p/5P2/4KP1q/3Q4 w - - 0 1",
                           "7k/3p2pp/4q3/8/4Q3/5Kp1/P6b/8 w - - 0 1",
                           "8/2p5/8/2kPKp1p/2p4P/2P5/3P4/8 w - - 0 1",
                           "8/1p3pp1/7p/5P1P/2k3P1/8/2K2P2/8 w - - 0 1",
                           "8/pp2r1k1/2p1p3/3pP2p/1P1P1P1P/P5KR/8/8 w - - 0 1",
                           "8/3p4/p1bk3p/Pp6/1Kp1PpPp/2P2P1P/2P5/5B2 b - - 0 1",
                           "5k2/7R/4P2p/5K2/p1r2P1p/8/8/8 b - - 0 1",
                           "6k1/6p1/P6p/r1N5/5p2/7P/1b3PP1/4R1K1 w - - 0 1",
                           "1r3k2/4q3/2Pp3b/3Bp3/2Q2p2/1p1P2P1/1P2KP2/3N4 w - - 0 1",
                           "6k1/4pp1p/3p2p1/P1pPb3/R7/1r2P1PP/3B1P2/6K1 w - - 0 1",
                           "8/3p3B/5p2/5P2/p7/PP5b/k7/6K1 w - - 0 1",
                           "5rk1/q6p/2p3bR/1pPp1rP1/1P1Pp3/P3B1Q1/1K3P2/R7 w - - 93 90",
                           "4rrk1/1p1nq3/p7/2p1P1pp/3P2bp/3Q1Bn1/PPPB4/1K2R1NR w - - 40 21",
                           "r3k2r/3nnpbp/q2pp1p1/p7/Pp1PPPP1/4BNN1/1P5P/R2Q1RK1 w kq - 0 16",
                           "3Qb1k1/1r2ppb1/pN1n2q1/Pp1Pp1Pr/4P2p/4BP2/4B1R1/1R5K b - - 11 40",
                           "4k3/3q1r2/1N2r1b1/3ppN2/2nPP3/1B1R2n1/2R1Q3/3K4 w - - 5 1"
};

BOOST_AUTO_TEST_SUITE(ChessEngine)

    BOOST_AUTO_TEST_CASE(CheckMagicBitboards){
        init_magics();
        BOOST_CHECK_EQUAL(get_sliding_landings(ROOK, 0,  0xc19da1890c182089),0x10101010e);
		BOOST_CHECK_EQUAL(get_sliding_landings(ROOK, 35, 0xc19da1890c182089),0x808f708000000);
		BOOST_CHECK_EQUAL(get_sliding_landings(ROOK, 52, 0xc19da1890c182089),0x10e8101010100000);
		BOOST_CHECK_EQUAL(get_sliding_landings(ROOK, 63, 0xc19da1890c182089),0x4080000000000000);

		
        BOOST_CHECK_EQUAL(get_sliding_landings(BISHOP,0,  0xc19da1890c182089),0x8040200);
		BOOST_CHECK_EQUAL(get_sliding_landings(BISHOP,35, 0xc19da1890c182089),0x4122140014204080);
        BOOST_CHECK_EQUAL(get_sliding_landings(BISHOP,50, 0xc19da1890c182089),0xa000a1120408000);
		BOOST_CHECK_EQUAL(get_sliding_landings(BISHOP,52, 0xc19da1890c182089),0x2800280402010000);
		BOOST_CHECK_EQUAL(get_sliding_landings(BISHOP,63, 0xc19da1890c182089),0x40200000000000);
    }
    
    BOOST_AUTO_TEST_CASE(CheckCastlingRightsFromFEN){
        int FEN_Results[] = {15,15,0,0,0,0,0,12,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,12,0,0};
        
        for(auto i=0; i< arrayLenght; i++){
            init_position(FEN_Array[i]);
            if (VERBOSE > 2) std::cout<<FEN_Array[i]<<std::endl;
            BOOST_CHECK_EQUAL(Position::st.castlingRights,FEN_Results[i]);
            if (VERBOSE > 2) std::cout<<"\n\n";
        }

    }
    BOOST_AUTO_TEST_CASE(CheckColorsFromFEN){
        int FEN_Results[] = {WHITE,WHITE,WHITE,BLACK,WHITE,WHITE,BLACK,WHITE,
                             WHITE,WHITE,BLACK,WHITE,BLACK,WHITE,BLACK,BLACK,
                             BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,
                             BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,
                             BLACK,WHITE
                             };
        
        for(auto i=0; i< arrayLenght; i++){
            init_position(FEN_Array[i]);
            if (VERBOSE > 2) std::cout<<FEN_Array[i]<<std::endl;
            BOOST_CHECK_EQUAL(Position::sideToMove,FEN_Results[i]);
            if (VERBOSE > 2) std::cout<<"\n\n";
        }

    }

    BOOST_AUTO_TEST_CASE(CheckPiecesFunction){
        init_position("rnbqkbnr/ppp1pppp/3P4/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq b6 0 3");
        BOOST_CHECK_EQUAL(pieces(), 0xfff7080e0000fbff);
        BOOST_CHECK_EQUAL(~pieces(), 0x8F7F1FFFF0400);
        BOOST_CHECK_EQUAL(pieces(ROOK), 0x8100000000000081);
        BOOST_CHECK_EQUAL(pieces(BLACK,ROOK), 0x8100000000000000);
        BOOST_CHECK_EQUAL(pieces(WHITE,ROOK), 0x81);
        BOOST_CHECK_EQUAL(pieces(WHITE,ROOK,QUEEN), 0x89);
    }
    BOOST_AUTO_TEST_CASE(CheckPawnAnyMovesFunction){
        MoveList theMoves;
        Bitboard theBitboard = 0ULL;
        Bitboard solutions[] = {0,0,0,0,0,0,0,0,
                                0x1010000,0x2020000,0,0x8080000,0x10100000,0x20200000,0x40400000,0x80800000,
                                0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,
                                0,0x2000000,0x40000000000,0x8000000,0,0,0,0,
                                0,0,0,0x1C000000000000,0,0,0,0,
                                0x10100000000,0x20000000000,0xC0000000000,0,0x181000000000,0x202000000000,0x404000000000,0x808000000000,
                                0,0,0,0,0,0,0,0,
                                };
        init_position("rnbqkbnr/ppp1pppp/3P4/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq b6 0 3");
        for(Square s = 0; s<64 ; s++){
            theMoves.Clear();
            theMoves = PawnAnyMoves(theMoves, s);
            // PrintMoveList(theMoves);
            theBitboard = Attacked2Bitboard(theMoves);
            BOOST_CHECK_EQUAL(theBitboard, solutions[s]);
        }

        // removing extra pawn makes enpassant possible
        init_position("rnbqkbnr/p1p1pppp/3P4/1pPp4/8/8/PP1PPPPP/RNBQKBNR w KQkq b6 0 3");
        // updating solutions
        solutions[34] = 0x60000000000;
        solutions[49] = 0x0;
        for(Square s = 0; s<64 ; s++){
            theMoves.Clear();
            theMoves = PawnAnyMoves(theMoves, s);
            // PrintMoveList(theMoves);
            theBitboard = Attacked2Bitboard(theMoves);
            BOOST_CHECK_EQUAL(theBitboard, solutions[s]);
        }
    }

    BOOST_AUTO_TEST_CASE(DetectCheck){
        // init_magics(); // previously initialized
        init_position("3Rk3/5prp/p5b1/6B1/r6P/2P5/PP3P2/2K3R1 b - - 2 23");
        BOOST_CHECK(countBitsOn(Checkers(WHITE))==0);
        BOOST_CHECK(countBitsOn(Checkers(BLACK))==1);
        init_position("8/2k5/4R3/8/8/8/7r/3K3r w - - 0 1");
        BOOST_CHECK(countBitsOn(Checkers(WHITE))==1);
        BOOST_CHECK(countBitsOn(Checkers(BLACK))==0);
        init_position("1k6/P7/8/8/8/K7/8/8 b - - 0 1");
        BOOST_CHECK(countBitsOn(Checkers(WHITE))==0);
        BOOST_CHECK(countBitsOn(Checkers(BLACK))==1);

        // DOUBLE CHECK
        init_position("4R3/2k5/4R3/8/8/4n3/8/3K3r w - - 0 1");
        BOOST_CHECK(countBitsOn(Checkers(WHITE))==2);
        BOOST_CHECK(countBitsOn(Checkers(BLACK))==0);



    }


BOOST_AUTO_TEST_SUITE_END()