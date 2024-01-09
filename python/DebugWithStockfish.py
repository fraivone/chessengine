#!/usr/bin/env python3
ExecutablePath = "/home/francesco/Programs/engine/build/chessengine"
StockfishPath = "/home/francesco/Programs/Stockfish/src/stockfish"
from PerftFromFile import read_stdout,send_command,test_fen,read_perftfile
import subprocess
import numpy as np

def ask_stockfish(process, FEN,depth):
    read_stdout(process, "see AUTHORS fil")
    send_command(process, f"position fen {FEN}")
    send_command(process, f"go perft {depth}")
    results = read_stdout(process, "Nodes searched:")
    total = results[-1]
    results.remove(total)
    
    
    total = total.replace("Nodes searched:","")
    if '' in results: results.remove('')
    
    per_move = {k.split(":")[0]:np.uint64(k.split(":")[-1]) for k in results}
    return np.uint64(total),per_move
    
# Start the subprocess
processSTK = subprocess.Popen(
    StockfishPath,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    universal_newlines=True,
    shell=True
)
# Start the subprocess
processME = subprocess.Popen(
    ExecutablePath,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    universal_newlines=True,
    shell=True
)


data = read_perftfile('perftstandard.txt')
for item in data:
    depth = 2
    fen = "r2k4/p1ppqpb1/bn2pQp1/3PN3/1p2P2r/2NB3p/PPPB1PPP/R3K2R w KQ - 1 3"#item[0]
    total_stk , dict_stk = ask_stockfish(processSTK,fen,depth)
    total_me , dict_me = test_fen(processME, fen,depth-1)

    if total_stk != total_me:
        stk_firstMoves = list(dict_stk.keys())
        me_firstMoves = list(dict_me.keys())
        if len(stk_firstMoves) != len(me_firstMoves):
            print(sorted(stk_firstMoves))
            print(sorted(me_firstMoves))
            # raise ValueError(f"FAILED at DEPTH {1}")
        else:
            for k in dict_stk:
                if(dict_stk[k]!=dict_me[k]):
                    raise ValueError(f"FAILED at DEPTH {depth}\t Move {k} problematic: Stk = {dict_stk[k]} vs Me {dict_me[k]}\tFEN = \n\t{fen}")

        
    