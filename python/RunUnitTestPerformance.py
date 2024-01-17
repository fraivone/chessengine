import subprocess
from matplotlib import pyplot as plt
import pandas as pd
import mplhep as hep
import time
import matplotlib.ticker as mtick
# plt.style.use(hep.style.CMS)
ExecutablePath = "/home/francesco/Programs/engine/build/test/_unit_tests"
outputFolder = "/home/francesco/Programs/engine/python/performance"
CSVPath = f"{outputFolder}/SearchResults.csv"

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
df["Date"] = pd.Timestamp.now()

try:
    dfold = pd.read_csv(CSVPath)
    df = pd.concat([dfold, df], ignore_index=True)
    # else skip append
except:
    pass
df.to_csv(CSVPath,index=False)



### PLOTTING
fig, ax = plt.subplots(4,2, figsize=(20,15),sharex=True)
ax = ax.flatten()
ax2 = []
df = pd.read_csv(CSVPath)
df['DateTime'] = df['Date'].apply(lambda x: pd.Timestamp(x).to_pydatetime())
df.sort_values(by=['DateTime'], inplace=True)

for k in df['Depth'].to_list():
    ax[k-1].set_title(f"Search depth {k}")
    ax2 = ax[k-1].twinx()
    ax[k-1].set_xlabel("Date")
    ax[k-1].set_ylabel("Time(us)")
    ax2.set_ylabel("Nodes")
    ax[k-1].plot(df[df['Depth'] == k].DateTime, df[df['Depth'] == k].Time,label=f"Time(us)",linestyle='dashed', marker='s',c="b")
    ax2.plot(df[df['Depth'] == k].DateTime, df[df['Depth'] == k].Nodes,label=f"Nodes",linestyle='dashed',c="r")
    # ax[k-1].yaxis.set_major_formatter(formatter)
    ax[k-1].grid()
    ax[k-1].grid()
    ax2.yaxis.set_major_formatter(mtick.FormatStrFormatter('%.0e'))
# fig.legend(loc="center left",fontsize='x-small',handleheight=2.4, labelspacing=0.05)
fig.autofmt_xdate(rotation=45)
fig.tight_layout()
fig.savefig(f"{outputFolder}/SearchResults.png")
fig.savefig(f"{outputFolder}/SearchResults.pdf")