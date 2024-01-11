import subprocess
from matplotlib import pyplot as plt
import pandas as pd
import mplhep as hep
plt.style.use(hep.style.CMS)
ExecutablePath = "/home/francesco/Programs/engine/build/test/_unit_tests"


results = []
MAX_DEPTH = 8
testName = "Init,"
for depth in range(1,MAX_DEPTH+1):
    testName += f"MiniMaxSpeedDepth{depth},"
testName = testName[:-1] # remove extra comma
print(testName)
print(f"Testing up to depth {MAX_DEPTH}")
# Start the subprocess
a = process = subprocess.run(
    [ExecutablePath+f" -t  {testName} -l test_suite"],
    shell=True,
    capture_output=True
)
output = a.stdout.decode("utf-8")
print(output)
for depth in range(1,MAX_DEPTH+1):
    testName = f"MiniMaxSpeedDepth{depth}"
    cut = output[output.find(testName):]
    temp = cut[cut.find("Searched moves: "):]
    searchedMovesString = temp[0:temp.find("\n"):]
    searchedMoves = int(searchedMovesString.split(":")[-1])

    temp = cut[cut.find("testing time: "):]
    TestingTimeString = temp[0:temp.find("\n"):]
    TestingTime = int(TestingTimeString.split(":")[-1].replace("us",""  ))

    results.append([depth,searchedMoves,TestingTime])

df = pd.DataFrame(results, columns=["Depth","Nodes","Time"])
fig, ax1 = plt.subplots()
ax1.set_yscale('log')
ax1.set_xlabel("Depth")
ax1.plot(df.Depth, df.Time,label="Time(us)",linestyle='dashed', marker='s')
ax1.plot(df.Depth, df.Nodes,label="Nodes",linestyle='dashed', marker='s')
ax1.plot(df.Depth, 10**(6)*df.Nodes/df.Time,label="Nodes/s",color="g",linestyle='dashed', marker='s')
ax1.legend()
ax1.set_ylim(10**3,10**7)
fig.savefig("Performance.pdf")
fig.savefig("Performance.png")