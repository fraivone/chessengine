#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE boardValidity
#endif

#include <boost/test/unit_test.hpp>
#include <iostream>
#include "print.hpp"
#include "types.hpp"
#include "position.hpp"
#include "magic.hpp"
#include "move_maker.hpp"
#include "eval.hpp"

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

std::unordered_map<std::string, int> PERFT2 = {{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 400},
                                                {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 2039},
                                                {"4k3/8/8/8/8/8/8/4K2R w K - 0 1", 66},
                                                {"4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 71},
                                                {"4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 75},
                                                {"r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 80},
                                                {"4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 112},
                                                {"r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 130},
                                                {"8/8/8/8/8/8/6k1/4K2R w K - 0 1", 38},
                                                {"8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 65},
                                                {"4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 32},
                                                {"r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 49},
                                                {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 568},
                                                {"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 567},
                                                {"r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 548},
                                                {"r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 547},
                                                {"1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 583},
                                                {"2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 560},
                                                {"r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 560},
                                                {"4k3/8/8/8/8/8/8/4K2R b K - 0 1", 75},
                                                {"4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 80},
                                                {"4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 66},
                                                {"r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 71},
                                                {"4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 130},
                                                {"r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 112},
                                                {"8/8/8/8/8/8/6k1/4K2R b K - 0 1", 32},
                                                {"8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 49},
                                                {"4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 38},
                                                {"r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 65},
                                                {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 568},
                                                {"r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 583},
                                                {"r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 560},
                                                {"r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 560},
                                                {"1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 567},
                                                {"2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 548},
                                                {"r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 547},
                                                {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 195},
                                                {"8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 156},
                                                {"8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 289},
                                                {"K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 51},
                                                {"k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 54},
                                                {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 193},
                                                {"8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 180},
                                                {"8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 68},
                                                {"K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 54},
                                                {"k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 51},
                                                {"B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 278},
                                                {"8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 316},
                                                {"k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 144},
                                                {"K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 143},
                                                {"B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 106},
                                                {"8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 309},
                                                {"k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 143},
                                                {"K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 144},
                                                {"7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 275},
                                                {"R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 1027},
                                                {"7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 275},
                                                {"R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 1027},
                                                {"6kq/8/8/8/8/8/8/7K w - - 0 1", 36},
                                                {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 36},
                                                {"K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 35},
                                                {"6qk/8/8/8/8/8/8/7K b - - 0 1", 43},
                                                {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 36},
                                                {"K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 35},
                                                {"8/8/8/8/8/K7/P7/k7 w - - 0 1", 7},
                                                {"8/8/8/8/8/7K/7P/7k w - - 0 1", 7},
                                                {"K7/p7/k7/8/8/8/8/8 w - - 0 1", 3},
                                                {"7K/7p/7k/8/8/8/8/8 w - - 0 1", 3},
                                                {"8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 35},
                                                {"8/8/8/8/8/K7/P7/k7 b - - 0 1", 3},
                                                {"8/8/8/8/8/7K/7P/7k b - - 0 1", 3},
                                                {"K7/p7/k7/8/8/8/8/8 b - - 0 1", 7},
                                                {"7K/7p/7k/8/8/8/8/8 b - - 0 1", 7},
                                                {"8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 35},
                                                {"8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 8},
                                                {"4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 8},
                                                {"8/8/7k/7p/7P/7K/8/8 w - - 0 1", 9},
                                                {"8/8/k7/p7/P7/K7/8/8 w - - 0 1", 9},
                                                {"8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 25},
                                                {"8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 61},
                                                {"8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 61},
                                                {"k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 15},
                                                {"8/8/7k/7p/7P/7K/8/8 b - - 0 1", 9},
                                                {"8/8/k7/p7/P7/K7/8/8 b - - 0 1", 9},
                                                {"8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 25},
                                                {"8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 61},
                                                {"8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 61},
                                                {"k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 15},
                                                {"7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 19},
                                                {"7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 19},
                                                {"k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 22},
                                                {"k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 16},
                                                {"k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 22},
                                                {"k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 16},
                                                {"k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 15},
                                                {"k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 16},
                                                {"7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 19},
                                                {"7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 19},
                                                {"k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 22},
                                                {"k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 16},
                                                {"k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 22},
                                                {"k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 16},
                                                {"k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 15},
                                                {"k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 16},
                                                {"7k/8/8/p7/1P6/8/8/7K w - - 0 1", 22},
                                                {"7k/8/p7/8/8/1P6/8/7K w - - 0 1", 16},
                                                {"7k/8/8/1p6/P7/8/8/7K w - - 0 1", 22},
                                                {"7k/8/1p6/8/8/P7/8/7K w - - 0 1", 16},
                                                {"k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 25},
                                                {"k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 25},
                                                {"3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 49},
                                                {"7k/8/8/p7/1P6/8/8/7K b - - 0 1", 22},
                                                {"7k/8/p7/8/8/1P6/8/7K b - - 0 1", 16},
                                                {"7k/8/8/1p6/P7/8/8/7K b - - 0 1", 22},
                                                {"7k/8/1p6/8/8/P7/8/7K b - - 0 1", 16},
                                                {"k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 25},
                                                {"k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 25},
                                                {"3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 49},
                                                {"8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 97},
                                                {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 421},
                                                {"8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 270},
                                                {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 496},
                                                {"8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 97},
                                                {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 421},
                                                {"8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 270},
                                                {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 496}
                                                };
std::unordered_map<std::string, int> PERFT1 = {
                                    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20},
                                    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", 48},
                                    {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - ", 14},
                                    {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 6},
                                    {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",44},
                                    {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",46},
                                    {"r6r/1b2k1bq/8/8/7B/8/8/R3K2R b KQ - 3 2",8},
                                    {"8/8/8/2k5/2pP4/8/B7/4K3 b - d3 0 3",8},
                                    {"r1bqkbnr/pppppppp/n7/8/8/P7/1PPPPPPP/RNBQKBNR w KQkq - 2 2",19},
                                    {"r3k2r/p1pp1pb1/bn2Qnp1/2qPN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQkq - 3 2",5},
                                    {"2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2",44},
                                    {"rnb2k1r/pp1Pbppp/2p5/q7/2B5/8/PPPQNnPP/RNB1K2R w KQ - 3 9", 39},
                                    {"2r5/3pk3/8/2P5/8/2K5/8/8 w - - 5 4",9},
                                    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48},
                                    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 20 },
                                    {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 48 },
                                    {"4k3/8/8/8/8/8/8/4K2R w K - 0 1", 15 },
                                    {"4k3/8/8/8/8/8/8/R3K3 w Q - 0 1", 16 },
                                    {"4k2r/8/8/8/8/8/8/4K3 w k - 0 1", 5 },
                                    {"r3k3/8/8/8/8/8/8/4K3 w q - 0 1", 5 },
                                    {"4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", 26 },
                                    {"r3k2r/8/8/8/8/8/8/4K3 w kq - 0 1", 5 },
                                    {"8/8/8/8/8/8/6k1/4K2R w K - 0 1", 12 },
                                    {"8/8/8/8/8/8/1k6/R3K3 w Q - 0 1", 15 },
                                    {"4k2r/6K1/8/8/8/8/8/8 w k - 0 1", 3 },
                                    {"r3k3/1K6/8/8/8/8/8/8 w q - 0 1", 4 },
                                    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", 26 },
                                    {"r3k2r/8/8/8/8/8/8/1R2K2R w Kkq - 0 1", 25 },
                                    {"r3k2r/8/8/8/8/8/8/2R1K2R w Kkq - 0 1", 25 },
                                    {"r3k2r/8/8/8/8/8/8/R3K1R1 w Qkq - 0 1", 25 },
                                    {"1r2k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 26 },
                                    {"2r1k2r/8/8/8/8/8/8/R3K2R w KQk - 0 1", 25 },
                                    {"r3k1r1/8/8/8/8/8/8/R3K2R w KQq - 0 1", 25 },
                                    {"4k3/8/8/8/8/8/8/4K2R b K - 0 1", 5 },
                                    {"4k3/8/8/8/8/8/8/R3K3 b Q - 0 1", 5 },
                                    {"4k2r/8/8/8/8/8/8/4K3 b k - 0 1", 15 },
                                    {"r3k3/8/8/8/8/8/8/4K3 b q - 0 1", 16 },
                                    {"4k3/8/8/8/8/8/8/R3K2R b KQ - 0 1", 5 },
                                    {"r3k2r/8/8/8/8/8/8/4K3 b kq - 0 1", 26 },
                                    {"8/8/8/8/8/8/6k1/4K2R b K - 0 1", 3 },
                                    {"8/8/8/8/8/8/1k6/R3K3 b Q - 0 1", 4 },
                                    {"4k2r/6K1/8/8/8/8/8/8 b k - 0 1", 12 },
                                    {"r3k3/1K6/8/8/8/8/8/8 b q - 0 1", 15 },
                                    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", 26 },
                                    {"r3k2r/8/8/8/8/8/8/1R2K2R b Kkq - 0 1", 26 },
                                    {"r3k2r/8/8/8/8/8/8/2R1K2R b Kkq - 0 1", 25 },
                                    {"r3k2r/8/8/8/8/8/8/R3K1R1 b Qkq - 0 1", 25 },
                                    {"1r2k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 25 },
                                    {"2r1k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", 25 },
                                    {"r3k1r1/8/8/8/8/8/8/R3K2R b KQq - 0 1", 25 },
                                    {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 w - - 0 1", 14 },
                                    {"8/1k6/8/5N2/8/4n3/8/2K5 w - - 0 1", 11 },
                                    {"8/8/4k3/3Nn3/3nN3/4K3/8/8 w - - 0 1", 19 },
                                    {"K7/8/2n5/1n6/8/8/8/k6N w - - 0 1", 3 },
                                    {"k7/8/2N5/1N6/8/8/8/K6n w - - 0 1", 17 },
                                    {"8/1n4N1/2k5/8/8/5K2/1N4n1/8 b - - 0 1", 15 },
                                    {"8/1k6/8/5N2/8/4n3/8/2K5 b - - 0 1", 16 },
                                    {"8/8/3K4/3Nn3/3nN3/4k3/8/8 b - - 0 1", 4 },
                                    {"K7/8/2n5/1n6/8/8/8/k6N b - - 0 1", 17 },
                                    {"k7/8/2N5/1N6/8/8/8/K6n b - - 0 1", 3 },
                                    {"B6b/8/8/8/2K5/4k3/8/b6B w - - 0 1", 17 },
                                    {"8/8/1B6/7b/7k/8/2B1b3/7K w - - 0 1", 21 },
                                    {"k7/B7/1B6/1B6/8/8/8/K6b w - - 0 1", 21 },
                                    {"K7/b7/1b6/1b6/8/8/8/k6B w - - 0 1", 7 },
                                    {"B6b/8/8/8/2K5/5k2/8/b6B b - - 0 1", 6 },
                                    {"8/8/1B6/7b/7k/8/2B1b3/7K b - - 0 1", 17 },
                                    {"k7/B7/1B6/1B6/8/8/8/K6b b - - 0 1", 7 },
                                    {"K7/b7/1b6/1b6/8/8/8/k6B b - - 0 1", 21 },
                                    {"7k/RR6/8/8/8/8/rr6/7K w - - 0 1", 19 },
                                    {"R6r/8/8/2K5/5k2/8/8/r6R w - - 0 1", 36 },
                                    {"7k/RR6/8/8/8/8/rr6/7K b - - 0 1", 19 },
                                    {"R6r/8/8/2K5/5k2/8/8/r6R b - - 0 1", 36 },
                                    {"6kq/8/8/8/8/8/8/7K w - - 0 1", 2 },
                                    {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 2 },
                                    {"K7/8/8/3Q4/4q3/8/8/7k w - - 0 1", 6 },
                                    {"6qk/8/8/8/8/8/8/7K b - - 0 1", 22 },
                                    {"6KQ/8/8/8/8/8/8/7k b - - 0 1", 2 },
                                    {"K7/8/8/3Q4/4q3/8/8/7k b - - 0 1", 6 },
                                    {"8/8/8/8/8/K7/P7/k7 w - - 0 1", 3 },
                                    {"8/8/8/8/8/7K/7P/7k w - - 0 1", 3 },
                                    {"K7/p7/k7/8/8/8/8/8 w - - 0 1", 1 },
                                    {"7K/7p/7k/8/8/8/8/8 w - - 0 1", 1 },
                                    {"8/2k1p3/3pP3/3P2K1/8/8/8/8 w - - 0 1", 7 },
                                    {"8/8/8/8/8/K7/P7/k7 b - - 0 1", 1 },
                                    {"8/8/8/8/8/7K/7P/7k b - - 0 1", 1 },
                                    {"K7/p7/k7/8/8/8/8/8 b - - 0 1", 3 },
                                    {"7K/7p/7k/8/8/8/8/8 b - - 0 1", 3 },
                                    {"8/2k1p3/3pP3/3P2K1/8/8/8/8 b - - 0 1", 5 },
                                    {"8/8/8/8/8/4k3/4P3/4K3 w - - 0 1", 2 },
                                    {"4k3/4p3/4K3/8/8/8/8/8 b - - 0 1", 2 },
                                    {"8/8/7k/7p/7P/7K/8/8 w - - 0 1", 3 },
                                    {"8/8/k7/p7/P7/K7/8/8 w - - 0 1", 3 },
                                    {"8/8/3k4/3p4/3P4/3K4/8/8 w - - 0 1", 5 },
                                    {"8/3k4/3p4/8/3P4/3K4/8/8 w - - 0 1", 8 },
                                    {"8/8/3k4/3p4/8/3P4/3K4/8 w - - 0 1", 8 },
                                    {"k7/8/3p4/8/3P4/8/8/7K w - - 0 1", 4 },
                                    {"8/8/7k/7p/7P/7K/8/8 b - - 0 1", 3 },
                                    {"8/8/k7/p7/P7/K7/8/8 b - - 0 1", 3 },
                                    {"8/8/3k4/3p4/3P4/3K4/8/8 b - - 0 1", 5 },
                                    {"8/3k4/3p4/8/3P4/3K4/8/8 b - - 0 1", 8 },
                                    {"8/8/3k4/3p4/8/3P4/3K4/8 b - - 0 1", 8 },
                                    {"k7/8/3p4/8/3P4/8/8/7K b - - 0 1", 4 },
                                    {"7k/3p4/8/8/3P4/8/8/K7 w - - 0 1", 4 },
                                    {"7k/8/8/3p4/8/8/3P4/K7 w - - 0 1", 5 },
                                    {"k7/8/8/7p/6P1/8/8/K7 w - - 0 1", 5 },
                                    {"k7/8/7p/8/8/6P1/8/K7 w - - 0 1", 4 },
                                    {"k7/8/8/6p1/7P/8/8/K7 w - - 0 1", 5 },
                                    {"k7/8/6p1/8/8/7P/8/K7 w - - 0 1", 4 },
                                    {"k7/8/8/3p4/4p3/8/8/7K w - - 0 1", 3 },
                                    {"k7/8/3p4/8/8/4P3/8/7K w - - 0 1", 4 },
                                    {"7k/3p4/8/8/3P4/8/8/K7 b - - 0 1", 5 },
                                    {"7k/8/8/3p4/8/8/3P4/K7 b - - 0 1", 4 },
                                    {"k7/8/8/7p/6P1/8/8/K7 b - - 0 1", 5 },
                                    {"k7/8/7p/8/8/6P1/8/K7 b - - 0 1", 4 },
                                    {"k7/8/8/6p1/7P/8/8/K7 b - - 0 1", 5 },
                                    {"k7/8/6p1/8/8/7P/8/K7 b - - 0 1", 4 },
                                    {"k7/8/8/3p4/4p3/8/8/7K b - - 0 1", 5 },
                                    {"k7/8/3p4/8/8/4P3/8/7K b - - 0 1", 4 },
                                    {"7k/8/8/p7/1P6/8/8/7K w - - 0 1", 5 },
                                    {"7k/8/p7/8/8/1P6/8/7K w - - 0 1", 4 },
                                    {"7k/8/8/1p6/P7/8/8/7K w - - 0 1", 5 },
                                    {"7k/8/1p6/8/8/P7/8/7K w - - 0 1", 4 },
                                    {"k7/7p/8/8/8/8/6P1/K7 w - - 0 1", 5 },
                                    {"k7/6p1/8/8/8/8/7P/K7 w - - 0 1", 5 },
                                    {"3k4/3pp3/8/8/8/8/3PP3/3K4 w - - 0 1", 7 },
                                    {"7k/8/8/p7/1P6/8/8/7K b - - 0 1", 5 },
                                    {"7k/8/p7/8/8/1P6/8/7K b - - 0 1", 4 },
                                    {"7k/8/8/1p6/P7/8/8/7K b - - 0 1", 5 },
                                    {"7k/8/1p6/8/8/P7/8/7K b - - 0 1", 4 },
                                    {"k7/7p/8/8/8/8/6P1/K7 b - - 0 1", 5 },
                                    {"k7/6p1/8/8/8/8/7P/K7 b - - 0 1", 5 },
                                    {"3k4/3pp3/8/8/8/8/3PP3/3K4 b - - 0 1", 7 },
                                    {"8/Pk6/8/8/8/8/6Kp/8 w - - 0 1", 11 },
                                    {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N w - - 0 1", 24 },
                                    {"8/PPPk4/8/8/8/8/4Kppp/8 w - - 0 1", 18 },
                                    {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1", 24 },
                                    {"8/Pk6/8/8/8/8/6Kp/8 b - - 0 1", 11 },
                                    {"n1n5/1Pk5/8/8/8/8/5Kp1/5N1N b - - 0 1", 24 },
                                    {"8/PPPk4/8/8/8/8/4Kppp/8 b - - 0 1", 18 },
                                    {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 24 },
                                    {"n1n5/PPPk4/8/8/8/8/4Kppp/5N1N b - - 0 1", 24},
        };

BOOST_AUTO_TEST_SUITE(Methods)

    BOOST_AUTO_TEST_CASE(CheckMagicBitboards){
        init_magics();
        init_lut();
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

    BOOST_AUTO_TEST_CASE(CheckPseudoMoves){
        const int size = 2;
        MoveList thePseudoMoves;
        Bitboard theBitboard;
        Bitboard solutions[size] = {0x5000000000040,0x200020000000000};

        std::string FEN_ArrayCheckPseudoMoves[size] = {"1B1k4/b1pP1p1p/PpP2P1P/1p6/1p1p4/1P1P3p/7P/7K w - - 0 1",
                                                        "3k4/b2P1p1p/PBP2P1P/1p6/1p1p4/1P1P3p/7P/7K b - - 0 1"};
        for(auto i = 0; i < size; i++){
            thePseudoMoves.Clear();
            init_position(FEN_ArrayCheckPseudoMoves[i]);
            thePseudoMoves = generate_all(thePseudoMoves, Position::sideToMove);
            theBitboard = Attacked2Bitboard(thePseudoMoves);
            BOOST_CHECK_EQUAL(theBitboard, solutions[i]);
        }
    }
    BOOST_AUTO_TEST_CASE(CheckCastlingPseudoMoves){
        const int size = 9;
        MoveList thePseudoMoves;
        // 0 no castles, 1 castles kingside, 2 castles queenside, 3 both castles
        Bitboard solutions[size] = {1,1,2, 0,3,1,0,0,1};

        std::string FEN_test[size] = {"rnbqk2r/p1pp1ppp/1p1bpn2/8/3P4/2P1PN2/PP2BPPP/RNBQK2R b KQkq - 1 5",
                                      "rnbqk2r/p1pp1ppp/1p1bpn2/8/3P4/2P1PN2/PP2BPPP/RNBQK2R w KQkq - 1 5",
                                      "rn1qk2r/p1p2ppp/1p1p1n2/4pb2/3P4/NQP1PN2/PP1BBPPb/R3K2R w KQkq - 2 9",
                                      "r1bqk2r/p1p2ppp/1p1p1n2/4p3/3P4/NnP1PN2/PP1BBPPb/R3K2R w KQkq - 1 8",
                                      "r6r/1b3kbq/8/8/7B/8/8/R3K2R w KQ - 4 3",
                                      "r6r/1b3kb1/7q/8/7B/8/8/R3K2R w KQ - 4 3",
                                      "r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1",
                                      "8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1",
                                      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"};
        for(auto i = 0; i < 9; i++){
            thePseudoMoves.Clear();
            int GeneratedCastlingType = 0;
            init_position(FEN_test[i]);
            thePseudoMoves = generate_all(thePseudoMoves, Position::sideToMove, KING);
            GeneratedCastlingType = 0;
            for(auto i=0; i<thePseudoMoves.size; i++){
                Move theMove = thePseudoMoves.list[i].move;
                MoveType mt = MoveType( theMove & 0xC000);
                Square from = ((theMove>>6) & 0x3f);
                Square to = ((theMove) & 0x3f);
                if(mt==CASTLING){              
                    // store what type of castling it was
                    if((to - from) == Castle_KingSide_KingDelta)
                        GeneratedCastlingType |= 1;
                    if((to - from) == Castle_QueenSide_KingDelta)
                        GeneratedCastlingType |= 2;
                }
            }
            // check matches with 
            BOOST_CHECK_EQUAL(GeneratedCastlingType, solutions[i]);
        }
    }

    BOOST_AUTO_TEST_CASE(CheckInBetweenLUT){
        Bitboard testResults[64] = {18049651735527936,0,0,0,0,0,0,36170086419038208,0,18049651735527424,0,0,0,0,0,36170086419005440,0,0,18049651735265280,0,0,0,0,36170086410616832,0,0,0,18049651601047552,0,0,0,36170084263133184,0,0,0,0,18049582881570816,0,0,36169534507319296,0,0,0,0,0,18014398509481984,0,36028797018963968,0,0,0,0,0,0,0,0,9079256848778919936,8935141660703064064,8646911284551352320,8070450532247928832,6917529027641081856,4611686018427387904,0,0};
        Square sq1 = 63;
        for(Square sq2=0; sq2<nCols*nRows; sq2++)
            BOOST_CHECK_EQUAL(calculate_inBetween(sq1,sq2),testResults[sq2]);
    }
    
    BOOST_AUTO_TEST_CASE(EnsureMakMove_UndoMove_opposite){

        MoveList theMoveList;
        Bitboard before,whiteBefore,blackBefore;
        StateInfo StateBefore;
        for (auto const& x : PERFT1){
            theMoveList.Clear();
            
            auto f = x.first;
            // std::cout<<f<<std::endl;
            init_position(f);
            StateBefore = Position::st;
            theMoveList = generate_legal(Position::sideToMove);
            for(int i= 0; i<theMoveList.size; i++ ){
                before = pieces();
                whiteBefore = pieces(WHITE);
                blackBefore = pieces(BLACK);
                MakeMove(theMoveList.list[i].move);    
                UndoMove(theMoveList.list[i].move);
                BOOST_CHECK_EQUAL(before,pieces());            
                BOOST_CHECK_EQUAL(whiteBefore,whiteBefore);            
                BOOST_CHECK_EQUAL(blackBefore,blackBefore);            
                BOOST_CHECK_EQUAL(StateBefore.capturedPiece,Position::st.capturedPiece);            
                BOOST_CHECK_EQUAL(StateBefore.rule50,Position::st.rule50);            
                BOOST_CHECK_EQUAL(StateBefore.castlingRights,Position::st.castlingRights);            
                BOOST_CHECK_EQUAL(StateBefore.epSquare,Position::st.epSquare);            
            }
        }

    }
    BOOST_AUTO_TEST_CASE(CheckFENGenerator){
        for(auto i=0; i< arrayLenght; i++){
            init_position(FEN_Array[i]);
            BOOST_CHECK_EQUAL(MakeFEN(),FEN_Array[i]);
        }
        
    }
    BOOST_AUTO_TEST_CASE(CheckPerftDepth1){
        for (auto const& x : PERFT1){
                BOOST_CHECK_EQUAL(StupidPerftCount(x.first,0),x.second);
                // std::cout<<x.first<<std::endl;
        }
    }
    BOOST_AUTO_TEST_CASE(CheckPerftDepth2){
        for (auto const& x : PERFT2)
            BOOST_CHECK_EQUAL(StupidPerftCount(x.first,1),x.second);
    }

    BOOST_AUTO_TEST_CASE(CheckMaterialCountConsistencyAtPerft4){
        
        MoveList mvl;
        int BeforeWhiteValue, AfterWhiteValue = 0;
        int BeforeBlackValue, AfterBlackValue = 0;
        for (auto const& x : PERFT2){
            init_position(x.first);
            BeforeWhiteValue = Position::st.nonPawnMaterial[WHITE];
            BeforeBlackValue = Position::st.nonPawnMaterial[BLACK];
            mvl = generate_legal(Position::sideToMove);
            // makes / unmakes all possible moves up to depth 4
            // so that at the end we should be back with the same
            // material count as beginning
            StupidPerftCount(mvl,4,4);
            AfterWhiteValue = Position::st.nonPawnMaterial[WHITE];
            AfterBlackValue = Position::st.nonPawnMaterial[BLACK];
            BOOST_CHECK_EQUAL(BeforeWhiteValue, AfterBlackValue);
            BOOST_CHECK_EQUAL(BeforeBlackValue, AfterBlackValue);
        }
    }
BOOST_AUTO_TEST_SUITE_END()




BOOST_AUTO_TEST_CASE(Init){
    init_lut();
    init_magics();
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth1){
    ExtMove theBest;
    uint64_t  n_calls= 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 1,1,n_calls, false);
    }
    std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth2){
    ExtMove theBest;
    uint64_t  n_calls= 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 2,2,n_calls, false);
    }
    std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth3){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 3,3,n_calls, false);
    }
    std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth4){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 4,4,n_calls, false);
    }
    std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth5){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 5,5,n_calls, false);
    }
    std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth6){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 6,6,n_calls, false);
    }
        std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth7){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 7,7,n_calls, false);
    }
        std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth8){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 8,8,n_calls, false);
    }
        std::cout<<"Searched moves: "<<n_calls<<std::endl;
}
BOOST_AUTO_TEST_CASE(MiniMaxSpeedDepth9){
    ExtMove theBest;
    uint64_t n_calls = 0;
    for (auto const& x : PERFT2){
        init_position(x.first);
        theBest = minmax(Position::sideToMove,-10000000, +10000000, 9,9,n_calls, false);
    }
        std::cout<<"Searched moves: "<<n_calls<<std::endl;
}    