NOTES
---
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