#!/usr/bin/env python3
ExecutablePath = "/home/francesco/Programs/engine/build/chessengine"
import subprocess
import numpy as np

# Function to read from stdout
def read_stdout(process, exit_string):
    lines = []
    while True:
        line = process.stdout.readline()
        lines.append(line.strip())
        if line:
            print(line.strip())
        if(exit_string in line.strip() ):
            break
    return lines
# Function to send commands to stdin
def send_command(process, command):
    process.stdin.write(command + '\n')
    process.stdin.flush()


def test_fen(process, fen_string, depth):
    # Example: Sending commands to stdin
    read_stdout(process, "Enter fen")
    send_command(process, fen_string)
    read_stdout(process, "Enter depth")
    send_command(process, str(depth))
    read_stdout(process, "Got")
    results = read_stdout(process, "DONE")
    
    total = results[-2]
    results.remove("DONE")
    results.remove(total)
    
    per_move = {k.split(":")[0]:np.uint64(k.split(":")[-1]) for k in results}
    return np.uint64(total),per_move


def read_perftfile(file_path):
    # Open the file to parse the data into a list
    with open(file_path, 'r') as file:
        content = file.readlines()
        # list with the data
        data = []
        for line in content:
            stringa = line.strip()
            splitted = stringa.split(";")
            results = [None] * 7 # each line contains 1FEN + 6 perfts
            for token in splitted:
                if "D" not in token: fen = token
                else:
                    depth = int(token.replace("D","").split(" ")[0])
                    perft = np.uint64(token.replace("D","").split(" ")[1])
                    results[depth] = perft

            results[0] = fen
            data.append(results)
    return data


if __name__ == "__main__":

    file_path = 'perftstandard.txt'  # Replace with the actual path to your file
    data = read_perftfile(file_path)


    # Start the subprocess
    process = subprocess.Popen(
        ExecutablePath,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        shell=True
    )

    for test in data:
        for depth in range(1,6):
            value , _ = test_fen(process, test[0],depth-1)
            if value != test[depth]:
                raise ValueError(f"FAILED at DEPTH {depth}\t {value} differs from book {test[depth]} for FEN\n\t{test[0]}")
            
    