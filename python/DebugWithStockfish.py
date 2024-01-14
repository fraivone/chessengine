#!/usr/bin/env python3
ExecutablePath = "/home/francesco/Programs/engine/build/chessengine"
StockfishPath = "/home/francesco/Programs/Stockfish/src/stockfish"
from PerftFromFile import read_stdout,send_command,test_fen,read_perftfile
import subprocess
import numpy as np

def ask_stockfish(process, FEN,depth,verbose=False):
    # if verbose: print('read_stdout(process, "see AUTHORS fil"')
    # read_stdout(process, "see AUTHORS fil")
    if verbose: print(f'send_command(process, "position fen {FEN}")')
    send_command(process, f"position fen {FEN}")
    if verbose: print(f'send_command(process, "go perft {depth}")')
    send_command(process, f"go perft {depth}")
    results = read_stdout(process, "Nodes searched:")
    total = results[-1]
    results.remove(total)
    
    
    total = total.replace("Nodes searched:","")
    while('' in results): 
        results.remove('')
    
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
read_stdout(processSTK, "see AUTHORS fil")
# Start the subprocess
processME = subprocess.Popen(
    ExecutablePath,
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    universal_newlines=True,
    shell=True
)


fen = input("Insert fen you want to compare\n")

for depth in range(1,7):
    print(f"Trying depth {depth}")
    total_stk , dict_stk = ask_stockfish(processSTK,fen,depth,False)
    total_me , dict_me = test_fen(processME, fen,depth-1)

    if total_stk != total_me:
        stk_firstMoves = list(dict_stk.keys())
        me_firstMoves = list(dict_me.keys())
        if len(stk_firstMoves) != len(me_firstMoves):
            print(sorted(stk_firstMoves))
            print(sorted(me_firstMoves))
            raise ValueError(f"FAILED at DEPTH {1}")
        else:
            for k in dict_stk:
                if(dict_stk[k]!=dict_me[k]):
                    raise ValueError(f"FAILED at DEPTH {depth}\t Move {k} problematic: Stk = {dict_stk[k]} vs Me {dict_me[k]}\tFEN = \n\t{fen}")
    print(f"Perfect match at depth {depth}\n---------------------------------")


        
    