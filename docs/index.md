NOTES
---
### What is a landing?
Move generated only taking into account the allowed piece movements, ignoring:
* Do I leave my king in check?
* Am I in check?
* Is enpassant actually allowed?
* Can I still castle?

### What is a Pseudomove?
Move generated only taking into account the allowed piece movements, ignoring:
* Do I leave my king in check?
* Am I in check?
* Can I still castle?

### When is enpassant enabled in the StateInfo?
En passant square will be considered only if
* side to move has a pawn threatening epSquare
* there is an enemy pawn in front of epSquare
* there is no piece on epSquare or behind epSquare

### What are the blockers?
When a king is in check, it might be possible to block the check with a move that's not capturing the attacking piece.
It happens when
* a sliding piece is checking;
* it is not a double check;
* there is at least one empty square on the attacking line of sight;
The pieces that can actually block a check are called blockers;

### Vectorization

I am trying to understand what is the fastest way to handle operations on uint64 (bitboards)
I am trying to use the **SIMD** (Single Instruction Multiple Data).
I have started with the `immintrin.h` library. 
So far my conclusion is that the compiler already implements this parallelization when possible.
For example, when adding 2 vectors of int together the speed-up is noticeable:
* *without* compiler optimization
    ```bash
    gcc  main.cpp -lstdc++ -o main; ./main
    Took 1.97542
    ```
* *with* compiler optimization
    ```bash
    gcc -O3  -march=native -ftree-vectorize main.cpp -lstdc++ -o main; ./main
    Took 0.140917
    ```
Which is a factor 10 speed up.
Therefore, I have added these settings in the CMake
```
# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3  -march=native -ftree-vectorize -lstdc++")
```

### Magic Bitboard
It's a tool to speed up the move generation for Bishops, Rooks and Queens (sliding pieces). Let's take the rook as an example.
The rook can't move through pieces. Therefore, to calculate the landing positions one has to take into account the other pieces that stand on the lines seen by the rook. 
Few key points:
* The type / color of the pieces doesn't matter. Each piece obstruct in the same way. Just be careful not to capture your own piece;
* If a piece stands between the rook and the border of the board, it can be neglected as there are no squares beyond it;
* Several combinations of pieces can lead to the same "available" squares for the rook.


With this in mind, for the rook, there are at most 12 squares to take into account.
So for each square there are 2^(12) = 4096 combinations of blockers that have to be explored. These 4096 combinations are a subset of all board occupancies (2^(64) ~ 10^(19)). Therefore, for each square, the 4096 combinations that we are looking at are *sparse* among a very large number of possible combinations... a needle in a haystack.

The magic bitboard provide an elegant solution:
1. For each square consider the blocker configurations one at a time;
1. Map the blocker configuration to an index `[0,...,4096)` by mean of a hash function;
1. Calculate the legal landings for the rook as a bitboard, then store the bitboard in a LUT under the index previously found;
1. Repeat for all blocker configs, squares;
1. To search for rook moves, use the same hash function to calculate the LUT `index` based on the occupancy. 
1. Retrieve the rook moves by calling `LUT[index]`