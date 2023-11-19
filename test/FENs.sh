cd /home/francesco/Programs/engine/build
make

echo "MATE FENs"

./chessengine "3rkbnr/1p1bp3/1q1p3p/p5pQ/3n4/PPR5/5PPP/6K1 b - - 2 2"
./chessengine "r1bqkb1r/pppp1Qpp/2n2n2/4p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4"
./chessengine "k6R/8/2N5/8/8/8/8/1R2K3 b - - 0 1"
./chessengine "4r1k1/ppp3pp/2p5/8/5P2/8/PP3R2/R1Q2K1q w - - 2 34"
./chessengine "k6R/P7/K7/8/8/8/8/8 b - - 0 1"

echo "SINGLE CHECK FENs"

./chessengine "3Rk3/5prp/p5b1/6B1/r6P/2P5/PP3P2/2K3R1 b - - 2 23"
./chessengine "8/2k5/4R3/8/8/8/7r/3K3r w - - 0 1"
./chessengine "1k6/P7/8/8/8/K7/8/8 b - - 0 1"

echo "DOUBLE CHECK"

./chessengine "4R3/2k5/4R3/8/8/4n3/8/3K3r w - - 0 1"

echo "STALEMATE FENs"

./chessengine "8/8/8/8/3b4/3k4/2p5/2K5 w - - 1 1"
./chessengine "8/8/8/8/3b4/3k4/p3p3/2K5 w - - 1 1"
./chessengine "K7/8/8/8/8/8/5Q2/7k b - - 1 1"
./chessengine "k7/1pp5/8/8/8/6q1/5r2/7K w - - 0 2"
./chessengine "8/8/6pk/8/6P1/8/2q5/K7 w - - 0 1"
./chessengine "8/2p5/1p6/pP1p2pp/P6k/8/5q2/7K w - - 0 2"
./chessengine "k7/P7/1K6/8/8/8/8/8 b - - 0 1"

echo "DRAW INSUFFICIENT MATERIAL"
./chessengine "8/1k6/8/8/3N1K2/8/8/8 b - - 0 4"

cd /home/francesco/Programs/engine/test